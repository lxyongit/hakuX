// xemu User Interface localization helpers.
#include "i18n.hh"

#include <SDL3/SDL.h>
#include "imgui.h"

#include <cstdlib>
#include <cinttypes>
#include <cstring>
#include <string>

namespace {

struct Translation {
    const char *english;
    const char *chinese;
};

constexpr Translation kChineseTranslations[] = {
    {"About", "关于"},
    {"All settings...", "全部设置..."},
    {"Aspect Ratio", "画面比例"},
    {"Audio", "音频"},
    {"AV Pack", "AV视频线"},
    {"Backend", "渲染后端"},
    {"Build Information", "构建信息"},
    {"Cache shaders to disk", "缓存着色器到磁盘"},
    {"Check for updates", "检查更新"},
    {"Community", "社区"},
    {"Configure machine settings to get started", "请配置主机设置以开始使用"},
    {"Config Information", "配置信息"},
    {"Create Snapshot", "创建快照"},
    {"Debug", "调试"},
    {"Display", "显示"},
    {"Display Mode", "显示模式"},
    {"Eject Disc", "弹出光盘"},
    {"EEPROM", "EEPROM"},
    {"Exit", "退出"},
    {"Files", "文件"},
    {"Filter Method", "滤镜方式"},
    {"First Boot", "首次启动"},
    {"Flash ROM (BIOS)", "Flash ROM（BIOS）"},
    {"Fullscreen", "全屏"},
    {"General", "通用"},
    {"Games", "游戏"},
    {"Games directory", "游戏目录"},
    {"Hard Disk", "硬盘镜像"},
    {"Help", "帮助"},
    {"Input", "输入"},
    {"MCPX Boot ROM", "MCPX 启动 ROM"},
    {"Machine", "主机"},
    {"Miscellaneous", "其他"},
    {"Monitor", "监视器"},
    {"Network", "网络"},
    {"No games found", "未找到游戏"},
    {"Pause", "暂停"},
    {"Performance", "性能"},
    {"Reset", "重置"},
    {"Restart", "重新启动"},
    {"Resume", "继续"},
    {"Save Snapshot", "保存快照"},
    {"Screenshot", "截图"},
    {"Screenshot output directory", "截图输出目录"},
    {"Settings", "设置"},
    {"Settings...", "设置..."},
    {"Snapshots", "快照"},
    {"Snapshots...", "快照..."},
    {"System", "系统"},
    {"System Configuration", "系统配置"},
    {"System Information", "系统信息"},
    {"System Memory", "系统内存"},
    {"UI Scale", "界面缩放"},
    {"Updates", "更新"},
    {"Video", "视频"},
    {"View", "视图"},
    {"Volume", "音量"},
    {"Adapter", "网卡"},
    {"Add", "添加"},
    {"Animations", "动画"},
    {"Aspect ratio", "画面比例"},
    {"Attached to", "连接方式"},
    {"Auto", "自动"},
    {"Auto (Default)", "自动（默认）"},
    {"Auto-bind controllers", "自动绑定控制器"},
    {"Background controller input capture", "后台捕获控制器输入"},
    {"Bind Address", "绑定地址"},
    {"Bind newly connected controllers to any open port", "将新连接的控制器绑定到任意空闲端口"},
    {"Bind to F%d", "绑定到 F%d"},
    {"Bound to F%d", "已绑定到 F%d"},
    {"Cancel", "取消"},
    {"Center", "居中"},
    {"Check for updates whenever xemu is opened", "每次打开 xemu 时检查更新"},
    {"Configure xemu to forward connections to guest on these ports", "配置 xemu 将这些端口的连接转发到来宾系统"},
    {"Connect virtual network cable to machine", "将虚拟网线连接到主机"},
    {"Controllers", "控制器"},
    {"Create", "创建"},
    {"Created new snapshot", "已创建新快照"},
    {"Current Image: %s", "当前镜像：%s"},
    {"Display mode", "显示模式"},
    {"Display notifications in upper-right corner", "在右上角显示通知"},
    {"DSP JIT engine", "DSP JIT 引擎"},
    {"Emulated Device", "模拟设备"},
    {"Emulated Input", "模拟输入"},
    {"Enable", "启用"},
    {"Enable fullscreen now", "立即启用全屏"},
    {"Enable improved audio accuracy (experimental)", "提高音频准确性（实验性）"},
    {"Enable Rumble", "启用振动"},
    {"Enable xemu user interface animations", "启用 xemu 用户界面动画"},
    {"Expansion Slots", "扩展插槽"},
    {"Filter by current title", "按当前游戏筛选"},
    {"Fullscreen on startup", "启动时全屏"},
    {"Hard FPU emulation", "硬件 FPU 模拟"},
    {"Hide mouse cursor", "隐藏鼠标光标"},
    {"Hide the mouse cursor when it is not moving", "鼠标静止时隐藏光标"},
    {"Host Input", "主机输入"},
    {"Host network interface to bridge with", "要桥接的主机网络接口"},
    {"Host Port", "主机端口"},
    {"Image", "镜像"},
    {"Input Device", "输入设备"},
    {"Input Mapping", "输入映射"},
    {"Install npcap", "安装 npcap"},
    {"Interface", "界面"},
    {"Interface element scale", "界面元素缩放比例"},
    {"Internal resolution scale", "内部渲染分辨率"},
    {"Invert Left X Axis", "反转左摇杆 X 轴"},
    {"Invert Left Y Axis", "反转左摇杆 Y 轴"},
    {"Invert Right X Axis", "反转右摇杆 X 轴"},
    {"Invert Right Y Axis", "反转右摇杆 Y 轴"},
    {"Keybinding", "快捷键绑定"},
    {"Last Used", "上次使用"},
    {"Linear", "线性"},
    {"Load", "加载"},
    {"Load Disc...", "加载光盘..."},
    {"Mapping", "映射"},
    {"Native", "原始比例"},
    {"Nearest", "最近邻"},
    {"New Image", "新建镜像"},
    {"No", "否"},
    {"No snapshots match filter criteria", "没有符合筛选条件的快照"},
    {"No snapshots to display", "没有可显示的快照"},
    {"None", "无"},
    {"Not Connected", "未连接"},
    {"Options", "选项"},
    {"Output volume limit", "输出音量上限"},
    {"Port Forwarding", "端口转发"},
    {"Press Create to create new snapshot", "点击“创建”以新建快照"},
    {"Press a key to rebind", "按任意键重新绑定"},
    {"Remove", "移除"},
    {"Remote Address", "远程地址"},
    {"Renderer", "渲染器"},
    {"Replace", "替换"},
    {"Reset to Default", "恢复默认设置"},
    {"Select desired renderer implementation", "选择要使用的渲染器"},
    {"Select how the framebuffer should fit or scale into the window", "选择画面在窗口中的适配方式"},
    {"Select preferred startup window size", "选择首选启动窗口大小"},
    {"Select the attached AV pack", "选择已连接的AV视频线"},
    {"Select the displayed aspect ratio", "选择显示的画面比例"},
    {"Sent! Thanks.", "已发送，感谢！"},
    {"Show main menu bar", "显示主菜单栏"},
    {"Show main menu bar when mouse is activated", "鼠标激活时显示主菜单栏"},
    {"Show notifications", "显示通知"},
    {"Skip startup animation", "跳过启动动画"},
    {"Skip the full Xbox boot animation sequence", "跳过完整的 Xbox 启动动画"},
    {"Snapshot", "快照"},
    {"Snapshot Options", "快照选项"},
    {"Start xemu in fullscreen when opened", "打开 xemu 时以全屏模式启动"},
    {"Stretch", "拉伸"},
    {"The DVD drive disc image mounted when the snapshot was created does not appear to be loaded:", "创建快照时已挂载的 DVD 光盘镜像似乎尚未加载："},
    {"UI scale", "界面缩放"},
    {"Unable to create XMU image at %s", "无法在 %s 创建 XMU 镜像"},
    {"Unbind", "解除绑定"},
    {"Vertical refresh sync", "垂直同步"},
    {"Window", "窗口"},
    {"Window size", "窗口大小"},
    {"Would you like to load it now?", "现在要加载它吗？"},
    {"Yes", "是"},
    {"Load '%s'", "加载“%s”"},
    {"Load F%d (Unbound)", "加载 F%d（未绑定）"},
    {"Save '%s'", "保存“%s”"},
    {"Save F%d (Unbound)", "保存 F%d（未绑定）"},
    {"Scale", "缩放"},
    {"Report Compatibility...", "报告兼容性..."},
    {"Check for Updates...", "检查更新..."},
    {"A snapshot with the name \"%s\" already exists. This button will overwrite the existing snapshot.", "名为“%s”的快照已存在。此按钮将覆盖现有快照。"},
    {"Application restart required to apply settings", "需要重启应用才能应用设置"},
    {"Background controller input capture", "后台捕获控制器输入"},
    {"Capture even if window is unfocused (requires restart)", "即使窗口未获得焦点也捕获输入（需要重启）"},
    {"Destination addr:port to forward packets to (1.2.3.4:9968)", "要转发数据包的目标地址：端口（1.2.3.4:9968）"},
    {"Expected Image: %s", "预期镜像：%s"},
    {"Guest Port", "来宾端口"},
    {"Memory Unit", "记忆卡"},
    {"Network interface", "网络接口"},
    {"Only display snapshots created while running the currently running XBE", "仅显示运行当前 XBE 时创建的快照"},
    {"Please select an available input device", "请选择可用的输入设备"},
    {"Protocol", "协议"},
    {"Search or name new snapshot...", "搜索或命名新快照..."},
    {"Setting AV Pack to NONE disables video output.", "将AV视频线设为“无”会禁用视频输出。"},
    {"Sync to screen vertical refresh to reduce tearing artifacts", "同步屏幕垂直刷新率以减少画面撕裂"},
    {"Virtual network connected (disable to change network settings)", "虚拟网络已连接（请先禁用才能更改网络设置）"},
    {"Action", "操作"},
    {"Advanced", "高级"},
    {"Audio Debug", "音频调试"},
    {"Broken", "无法运行"},
    {"Check for Update", "检查更新"},
    {"Controls how the rendered content should be scaled into the window", "控制渲染内容在窗口中的缩放方式"},
    {"Delete", "删除"},
    {"Description", "描述"},
    {"Exclusive fullscreen", "独占全屏"},
    {"Expected Image: %s", "预期镜像：%s"},
    {"Guest Port", "来宾端口"},
    {"Hard Disk", "硬盘镜像"},
    {"HRTF Filtering\n", "HRTF 滤波\n"},
    {"Intro", "可进入开场"},
    {"JIT Engine\n", "JIT 引擎\n"},
    {"May improve responsiveness, but slows window switching", "可能提升响应速度，但会降低窗口切换速度"},
    {"Monitor", "监视器"},
    {"Network interface", "网络接口"},
    {"No snapshots match filter criteria", "没有符合筛选条件的快照"},
    {"Output volume limit", "输出音量上限"},
    {"Perfect", "完美运行"},
    {"Playable", "可游玩"},
    {"Quality", "质量"},
    {"Real-time DSP processing", "实时 DSP 处理"},
    {"Release notes...", "发行说明..."},
    {"Report Compatibility", "报告兼容性"},
    {"Restart", "重新启动"},
    {"Select the attached AV pack", "选择已连接的AV视频线"},
    {"Setting AV Pack to NONE disables video output.", "将AV视频线设为“无”会禁用视频输出。"},
    {"Show notifications", "显示通知"},
    {"Starts", "可启动"},
    {"Update", "更新"},
    {"Update availability unknown.", "更新可用性未知。"},
    {"User Token", "用户令牌"},
    {"Video Debug", "视频调试"},
    {"Visit", "访问"},
    {"for more information", "以了解更多信息"},
    {"(None)", "（无）"},
    {"DVD Drive Image", "DVD 光盘镜像"},
    {"Error", "错误"},
    {"Ok", "确定"},
    {"An error has occured. Try again.", "发生错误，请重试。"},
    {"Checking for update...", "正在检查更新..."},
    {"Current version: %s", "当前版本：%s"},
    {"Downloading update...", "正在下载更新..."},
    {"Latest version: %s", "最新版本：%s"},
    {"This version of xemu is up to date.", "当前 xemu 已是最新版本。"},
    {"An updated version of xemu is available!", "有可用的 xemu 更新版本！"},
    {"Update successful! Restart to launch updated version of xemu.", "更新成功！重启后将运行新版 xemu。"},
    {"A", "A"},
    {"B", "B"},
    {"Back", "返回"},
    {"Black", "黑键"},
    {"Bridged Adapter", "桥接网卡"},
    {"Capture even if window is unfocused (requires restart)", "即使窗口未获得焦点也捕获输入（需要重启）"},
    {"Controls what the virtual network controller interfaces with", "设置虚拟网络控制器连接的对象"},
    {"DPad Down", "方向键下"},
    {"DPad Left", "方向键左"},
    {"DPad Right", "方向键右"},
    {"DPad Up", "方向键上"},
    {"Guide", "指南"},
    {"Increase to 128 MiB for debug or homebrew applications", "调试或自制程序可提高到 128 MiB"},
    {"Invalid", "无效"},
    {"Left Stick Axis X", "左摇杆 X 轴"},
    {"Left Stick Axis Y", "左摇杆 Y 轴"},
    {"Left Stick Button", "左摇杆按键"},
    {"Left Stick Down", "左摇杆下"},
    {"Left Stick Left", "左摇杆左"},
    {"Left Stick Right", "左摇杆右"},
    {"Left Stick Up", "左摇杆上"},
    {"Left Trigger", "左扳机"},
    {"Left Trigger Axis", "左扳机轴"},
    {"Limit output volume (%d%%)", "限制输出音量（%d%%）"},
    {"Local addr:port to receive packets on (0.0.0.0:9968)", "接收数据包的本地地址：端口（0.0.0.0:9968）"},
    {"NAT", "NAT"},
    {"Reduce stutter in games by caching previously generated shaders", "缓存已生成的着色器以减少游戏卡顿"},
    {"Right Stick Axis X", "右摇杆 X 轴"},
    {"Right Stick Axis Y", "右摇杆 Y 轴"},
    {"Right Stick Button", "右摇杆按键"},
    {"Right Stick Down", "右摇杆下"},
    {"Right Stick Left", "右摇杆左"},
    {"Right Stick Right", "右摇杆右"},
    {"Right Stick Up", "右摇杆上"},
    {"Right Trigger", "右扳机"},
    {"Right Trigger Axis", "右扳机轴"},
    {"Start", "开始"},
    {"UDP Tunnel", "UDP 隧道"},
    {"Use DSP JIT engine", "使用 DSP JIT 引擎"},
    {"Use hardware-accelerated floating point emulation (requires restart)", "使用硬件加速的浮点模拟（需要重启）"},
    {"White", "白键"},
    {"X", "X"},
    {"Y", "Y"},
    {"Get Token", "获取令牌"},
    {"Intro/Menus", "开场/菜单"},
    {"Playability", "可玩性"},
    {"Privacy Disclosure (Please read before submission!)", "隐私说明（提交前请阅读）"},
    {"Report Details", "报告详情"},
    {"Send", "发送"},
    {"(Paused)", "（已暂停）"},
    {"AC97", "AC97"},
    {"EP Only", "仅 EP"},
    {"EP Realtime\n", "EP 实时\n"},
    {"GP Only", "仅 GP"},
    {"GP Realtime\n", "GP 实时\n"},
    {"GP/EP if enabled", "若启用则使用 GP/EP"},
    {"Mono", "单声道"},
    {"Stereo", "立体声"},
    {"Throttle", "节流"},
    {"VP Only", "仅 VP"},
    {"VP Workers", "VP 工作线程"},
    {"Voice 0x%x/%d %s", "语音 0x%x/%d %s"},
    {"Container Size: %s, Sample Size: %s, %s: %d", "容器大小：%s，采样大小：%s，%s：%d"},
    {"Deviation: %" PRId64 "/%" PRId64 "/%" PRId64 " us", "偏差：%" PRId64 "/%" PRId64 "/%" PRId64 " 微秒"},
    {"EBO=%d CBO=%d LBO=%d BA=%x", "EBO=%d CBO=%d LBO=%d BA=%x"},
    {"EP Cycles:   %04d", "EP 周期：%04d"},
    {"Frames:      %04d", "帧数：%04d"},
    {"GP Cycles:   %04d", "GP 周期：%04d"},
    {"Latency:   %.1f/%.1f/%.1f ms", "延迟：%.1f/%.1f/%.1f 毫秒"},
    {"Mix: ", "混音："},
    {"Multipass Dest Voice: 0x%02x", "多通道目标语音：0x%02x"},
    {"Multipass Src Voices:", "多通道源语音："},
    {"Queue/Pacing:", "队列/节奏："},
    {"Rate: %f (%d Hz)", "速率：%f（%d Hz）"},
    {"Stream: %-3s Loop: %-3s Persist: %-3s Multipass: %-3s Linked: %-3s", "流：%-3s 循环：%-3s 保持：%-3s 多通道：%-3s 链接：%-3s"},
    {"Utilization: %.2f%%", "利用率：%.2f%%"},
    {"VP:          %4d us", "VP：%4d 微秒"},
    {"1 byte", "1 字节"},
    {"2 bytes", "2 字节"},
    {"4 bytes", "4 字节"},
    {"Multipass Bin", "多通道组"},
    {"NO", "否"},
    {"Samples per Block", "每块采样数"},
    {"Signed 16b PCM", "有符号 16 位 PCM"},
    {"Signed 24b PCM", "有符号 24 位 PCM"},
    {"Signed 32b PCM", "有符号 32 位 PCM"},
    {"Unsigned 8b PCM", "无符号 8 位 PCM"},
    {"YES", "是"},
    {"Error: %s (%d)", "错误：%s（%d）"},
    {"%s (Port %d)", "%s（端口 %d）"},
    {"Keyboard", "键盘"},
};

bool preferred_locale_is_chinese()
{
    int locale_count = 0;
    SDL_Locale **locales = SDL_GetPreferredLocales(&locale_count);
    if (!locales) {
        return false;
    }

    // SDL returns these in the user's order of preference. Only the primary
    // language selects the UI language: an English system with Chinese as a
    // secondary input language should still use the English interface.
    const char *language = locale_count > 0 && locales[0]
                               ? locales[0]->language
                               : nullptr;
    const bool is_chinese = language && std::strncmp(language, "zh", 2) == 0;
    SDL_free(locales);
    return is_chinese;
}

} // namespace

bool xemu_ui_uses_chinese()
{
    static const bool uses_chinese = preferred_locale_is_chinese();
    return uses_chinese;
}

const char *xemu_ui_text(const char *english)
{
    if (!xemu_ui_uses_chinese() || !english) {
        return english;
    }
    for (const auto &translation : kChineseTranslations) {
        if (std::strcmp(translation.english, english) == 0) {
            return translation.chinese;
        }
    }
    return english;
}

void xemu_ui_add_chinese_glyphs(ImFontGlyphRangesBuilder &builder)
{
    for (const auto &translation : kChineseTranslations) {
        builder.AddText(translation.chinese);
    }
}

const char *xemu_ui_chinese_font_path()
{
    if (!xemu_ui_uses_chinese()) {
        return nullptr;
    }
#if defined(_WIN32)
    static const std::string path = [] {
        const char *windows_directory = std::getenv("WINDIR");
        return std::string(windows_directory ? windows_directory : "C:\\Windows") +
               "\\Fonts\\msyh.ttc";
    }();
    return path.c_str();
#elif defined(__APPLE__)
    // PingFang is not present on every supported macOS installation. Hiragino
    // Sans GB is a system CJK font and is available on those installations as
    // well, so use it for the Simplified Chinese UI.
    return "/System/Library/Fonts/Hiragino Sans GB.ttc";
#else
    return "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc";
#endif
}
