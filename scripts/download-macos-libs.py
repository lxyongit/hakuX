#!/usr/bin/env python3
"""
Downloads required libraries for xemu builds on macOS from MacPorts repositories
"""
# Based on https://github.com/tpoechtrager/osxcross/blob/master/tools/osxcross-macports
# which is based on https://github.com/maci0/pmmacports
from urllib.request import urlopen
import re
import os.path
from tarfile import TarFile

MIRROR = 'https://packages.macports.org'

# FIXME: Inline macports key
# FIXME: Move packages to archive directory to track used vs unused
# FIXME: Support multiple mirrors

class LibInstaller:
	DARWIN_TARGET_X64="darwin_21" # macOS 11
	DARWIN_TARGET_ARM64="darwin_23" # macOS 14.x

	def __init__(self, arch):
		self._queue = []
		self._installed = []
		if arch == 'x86_64':
			self._darwin_target = self.DARWIN_TARGET_X64
		elif arch == 'arm64':
			self._darwin_target = self.DARWIN_TARGET_ARM64
		else:
			assert False, "Add arch"
		self._arch = arch

		self._extract_path = os.path.realpath(f'./macos-libs/{self._arch}')
		if not os.path.exists(self._extract_path):
			os.makedirs(self._extract_path)
		self._installed_path = os.path.join(self._extract_path, 'INSTALLED')
		self._pkgs_path = os.path.realpath(os.path.join(f'./macos-pkgs'))
		if not os.path.exists(self._pkgs_path):
			os.makedirs(self._pkgs_path)

	def get_latest_pkg_filename_url(self, pkg_name):
		pkg_base_url = f'{MIRROR}/{pkg_name}'
		pkg_list = urlopen(pkg_base_url).read().decode('utf-8')
		pkgs = re.findall(pkg_name + r'[\w\.\-\_\+]*?\.(?:any_any|darwin_any|' + self._darwin_target + r')\.(?:noarch|' + self._arch + r')\.tbz2', pkg_list)
		if len(pkgs) < 1:
			print(f'    [*] [ERROR] Unable to find version of {pkg_name} compatible with {self._darwin_target}.{self._arch}')
			exit(1)

		pkg_filename = pkgs[-1]
		return pkg_filename, f'{pkg_base_url}/{pkg_filename}'

	def is_pkg_installed(self, pkg_name):
		if not os.path.exists(self._installed_path):
			return False
		with open(self._installed_path) as f:
			installed = [l.strip().split('=')[0] for l in f.readlines()]
			return pkg_name in installed

	def mark_pkg_installed(self, pkg_name, pkg_version):
		if self.is_pkg_installed(pkg_name):
			return
		with open(os.path.join(self._extract_path, 'INSTALLED'), 'a+') as f:
			f.write(f'{pkg_name}={pkg_version}\n')

	def download_file(self, desc, url, dst):
		if os.path.exists(dst):
			print(f'    [+] Already have {desc}')
		else:
			print(f'    [+] Downloading {desc}')
			with open(dst, 'wb') as f:
				f.write(urlopen(url).read())

	def is_pkg_skipped(self, pkg_name):
		return any(pkg_name.startswith(n) for n in ('python', 'ncurses', 'mesa', 'llvm', 'libsndfile'))

	def install_pkg(self, pkg_name):
		if self.is_pkg_installed(pkg_name):
			print(f'[*] Package {pkg_name} already installed')
			return

		if self.is_pkg_skipped(pkg_name):
			print(f'[*] Skipping package {pkg_name}')
			return

		print(f'[*] Fetching {pkg_name}')
		pkg_filename, pkg_url = self.get_latest_pkg_filename_url(pkg_name)
		pkg_version = pkg_filename[re.search(r'-\d', pkg_filename).span()[0]+1:]
		pkg_version = pkg_version[:pkg_version.find('.'+self._darwin_target)]
		dst_pkg_filename = os.path.join(self._pkgs_path, pkg_filename)
		print(f'    [*] Found package {pkg_filename}')
		self.download_file(pkg_filename, pkg_url, dst_pkg_filename)

		print(f'    [+] Looking for dependencies')
		tb = TarFile.open(dst_pkg_filename)
		pkg_contents_file = tb.extractfile('./+CONTENTS').read().decode('utf-8')
		for dep in re.findall(r'@pkgdep (.+)', pkg_contents_file):
			print(f'        [>] {dep}')
			s = re.search(r'-\d', dep)
			if s:
				dep = dep[0:s.span()[0]]
			self._queue.append(dep)

		print(f'    [*] Checking tarball...')

		for fpath in tb.getnames():
			extracted_path = os.path.realpath(os.path.join(self._extract_path, fpath))
			assert extracted_path.startswith(self._extract_path), f'tarball has a global file: {fname}'

		print(f'    [*] Extracting to {self._extract_path}')
		# Python 3.14 defaults to the stricter ``data`` filter, which rejects
		# the absolute symlinks present in MacPorts archives. The ``tar`` filter
		# keeps those links while still preventing archive members from escaping
		# the destination directory.
		try:
			tb.extractall(self._extract_path, numeric_owner=True, filter='tar')
		except TypeError:
			# Python versions before 3.12 do not provide extraction filters.
			tb.extractall(self._extract_path, numeric_owner=True)

		for fpath in tb.getnames():
			# FIXME: Symlinks
			extracted_path = os.path.realpath(os.path.join(self._extract_path, fpath))
			if extracted_path.endswith('.pc'):
				print(f'    [*] Fixing {extracted_path}')
				with open(extracted_path, 'r') as f:
					lines = f.readlines()
				for i, l in enumerate(lines):
					if l.strip().startswith('prefix'):
						new_prefix = f'prefix={self._extract_path}/opt/local\n'
						if pkg_name.startswith('openssl'): # FIXME
							new_prefix = f'prefix={self._extract_path}/opt/local/libexec/openssl11\n'
						lines[i] = new_prefix
					elif l.strip().startswith('Requires.private:'):
						if pkg_name.startswith('libepoxy'):
							lines[i] = ''
				with open(extracted_path, 'w') as f:
					f.write(''.join(lines))

		if pkg_name == 'glib2':
			fpath = './opt/local/include/glib-2.0/glib/gi18n.h'
			extracted_path = os.path.realpath(os.path.join(self._extract_path, fpath))
			print(f'    [*] Fixing {extracted_path}')
			with open(extracted_path, 'r') as f:
				lines = f.read()
			s = '/opt/local/include/libintl.h'
			lines = lines.replace(s, self._extract_path + s)
			with open(extracted_path, 'w') as f:
				f.write(lines)

		self.mark_pkg_installed(pkg_name, pkg_version)

	def install_pkgs(self, requested):
		self._queue.extend(requested)
		while len(self._queue) > 0:
			pkg_name = self._queue.pop(0)
			self.install_pkg(pkg_name)

def main():
	import argparse
	ap = argparse.ArgumentParser()
	ap.add_argument('arch', choices=('arm64', 'x86_64'))
	args = ap.parse_args()
	li = LibInstaller(args.arch)
	li.install_pkgs([
		'SDL3',
		'glib2',
		'libsamplerate',
		'libpixman',
		'libepoxy',
		'libpcap',
		'libslirp',
		'libusb'])

if __name__ == '__main__':
	main()
