#!/usr/bin/env bash
# Build an unsigned macOS xemu package using the same architecture matrix as
# .github/workflows/build-macos.yml.

set -euo pipefail

usage() {
    cat <<'EOF'
Usage: ./build-macos.sh [options]

Options:
  --arch <arm64|x86_64|universal>  Architecture to package (default: universal)
  --debug                          Create a debug build (default: release)
  --lto                            Enable ThinLTO (off by default for local builds)
  --version <version>              Package version; defaults to XEMU_VERSION
  --output-dir <path>              Directory for the generated .zip
  -h, --help                       Show this help text

The universal package contains both architecture-specific dependency folders
and a lipo-merged xemu executable. It is ad-hoc signed and not notarized.
EOF
}

project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
architecture="universal"
configuration="release"
use_lto=false
package_version=""
output_dir="${project_root}/dist-macos"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --arch)
            [[ $# -ge 2 ]] || { echo "Missing value for --arch" >&2; exit 2; }
            architecture="$2"
            shift 2
            ;;
        --debug)
            configuration="debug"
            shift
            ;;
        --lto)
            use_lto=true
            shift
            ;;
        --version)
            [[ $# -ge 2 ]] || { echo "Missing value for --version" >&2; exit 2; }
            package_version="$2"
            shift 2
            ;;
        --output-dir)
            [[ $# -ge 2 ]] || { echo "Missing value for --output-dir" >&2; exit 2; }
            output_dir="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

case "$architecture" in
    arm64|x86_64|universal) ;;
    *)
        echo "Unsupported architecture: $architecture" >&2
        exit 2
        ;;
esac

if [[ "$(uname -s)" != "Darwin" ]]; then
    echo "build-macos.sh must be run on macOS." >&2
    exit 1
fi

cd "$project_root"

if [[ -z "$package_version" ]]; then
    if [[ -f XEMU_VERSION ]]; then
        package_version="$(cut -d- -f1 XEMU_VERSION)"
    else
        package_version="0.0.0"
    fi
fi

for command in brew git python3 xcrun lipo codesign ditto zip; do
    command -v "$command" >/dev/null || {
        echo "Required command not found: $command" >&2
        exit 1
    }
done

export HOMEBREW_NO_AUTO_UPDATE=1
export HOMEBREW_NO_INSTALL_CLEANUP=1
export CCACHE_DIR="${project_root}/xemu-ccache"
export CCACHE_MAXSIZE=512M
python_venv="${project_root}/.build-venv/macos"

echo 'Installing build dependencies...'
brew install ccache coreutils dylibbundler nlohmann-json
if [[ ! -x "${python_venv}/bin/python3" ]]; then
    python3 -m venv "$python_venv"
fi
"${python_venv}/bin/python3" -m pip install pyyaml requests
export PATH="$(brew --prefix ccache)/libexec:${python_venv}/bin:${PATH}"
json_prefix="$(brew --prefix nlohmann-json)"
export PKG_CONFIG_LIBDIR="${json_prefix}/share/pkgconfig${PKG_CONFIG_LIBDIR:+:${PKG_CONFIG_LIBDIR}}"

clang_version="$(xcrun clang --version | sed -n 's/^Apple clang version \([0-9.]*\).*/\1/p')"
if [[ -z "$clang_version" ]]; then
    echo 'Unable to determine the Apple Clang version.' >&2
    exit 1
fi
# ThinLTO summaries are not compatible across all Clang releases. Keep cache
# entries isolated by compiler version instead of reusing an old summary index.
export LTO_CACHE_DIR="${project_root}/xemu-lto-cache/clang-${clang_version}"

mkdir -p "$CCACHE_DIR" "$LTO_CACHE_DIR" "$output_dir"
ccache -z

remove_incomplete_meson_subproject() {
    local name="$1"
    local subproject_dir="${project_root}/subprojects/${name}"

    [[ -d "$subproject_dir" && ! -f "${subproject_dir}/meson.build" ]] || return 0

    # Meson leaves an empty Git repository behind if a wrap download fails.
    # Only remove that exact no-commit state; a checked-out or user-provided
    # source tree is left untouched and reported instead.
    if [[ -d "${subproject_dir}/.git" ]] && \
       ! git -C "$subproject_dir" rev-parse --verify HEAD >/dev/null 2>&1; then
        echo "Removing incomplete Meson subproject: ${name}"
        rm -rf "$subproject_dir"
        return
    fi

    echo "Subproject ${name} exists but has no meson.build; resolve it manually." >&2
    exit 1
}

remove_incomplete_meson_subproject keycodemapdb

build_options=()
package_suffix=""
if [[ "$configuration" == "debug" ]]; then
    build_options+=(--debug)
    package_suffix="-dbg"
else
    build_options+=(-Dx86_version=3)
    if [[ "$use_lto" == true ]]; then
        build_options+=(
            -Db_lto=true
            -Db_lto_mode=thin
            -Db_thinlto_cache=true
            "-Db_thinlto_cache_dir=${LTO_CACHE_DIR}"
        )
    else
        # The bundled DSP56300 library can be produced by a newer LLVM than
        # the local Xcode linker understands. Keep regular Release
        # optimization, but do not ask ld to consume its ThinLTO metadata.
        build_options+=(-Db_lto=false)
    fi
fi

staging_dir="$(mktemp -d "${TMPDIR:-/tmp}/xemu-macos-build.XXXXXX")"
trap 'rm -rf "$staging_dir"' EXIT

build_architecture() {
    local arch="$1"

    echo "Building ${arch} (${configuration})..."
    ./build.sh -a "$arch" "${build_options[@]}"

    if [[ ! -d dist/xemu.app || ! -f dist/xemu.app/Contents/MacOS/xemu ]]; then
        echo "Expected xemu.app was not produced for ${arch}." >&2
        exit 1
    fi

    mkdir -p "${staging_dir}/${arch}"
    ditto dist/xemu.app "${staging_dir}/${arch}/xemu.app"
    if [[ -f dist/LICENSE.txt ]]; then
        cp dist/LICENSE.txt "${staging_dir}/${arch}/LICENSE.txt"
    fi
}

if [[ "$architecture" == "universal" ]]; then
    build_architecture x86_64
    build_architecture arm64

    package_dir="${staging_dir}/universal"
    mkdir -p "$package_dir"
    # This mirrors the CI job's two unzip operations: library directories for
    # both architectures remain in the final bundle.
    ditto "${staging_dir}/x86_64/xemu.app" "${package_dir}/xemu.app"
    ditto "${staging_dir}/arm64/xemu.app" "${package_dir}/xemu.app"
    lipo -create \
        -output "${package_dir}/xemu.app/Contents/MacOS/xemu" \
        "${staging_dir}/x86_64/xemu.app/Contents/MacOS/xemu" \
        "${staging_dir}/arm64/xemu.app/Contents/MacOS/xemu"
    codesign --force --deep \
        --preserve-metadata=entitlements,requirements,flags,runtime \
        --sign - "${package_dir}/xemu.app/Contents/MacOS/xemu"
    cp "${staging_dir}/arm64/LICENSE.txt" "${package_dir}/LICENSE.txt"
    archive_name="xemu-${package_version}${package_suffix}-macos-universal-unsigned.zip"
else
    build_architecture "$architecture"
    package_dir="${staging_dir}/${architecture}"
    archive_name="xemu-${package_version}${package_suffix}-macos-${architecture}-unsigned.zip"
fi

archive_path="${output_dir}/${archive_name}"
(
    cd "$package_dir"
    zip -r -FS "$archive_path" xemu.app LICENSE.txt
)

ccache -s
echo "Created ${archive_path}"
