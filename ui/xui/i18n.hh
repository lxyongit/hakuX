// xemu User Interface localization helpers.
#pragma once

struct ImFontGlyphRangesBuilder;

// The UI language is selected once from the operating system's preferred
// locales. Only Simplified Chinese is currently bundled; every other locale
// deliberately falls back to the original English text.
bool xemu_ui_uses_chinese();
const char *xemu_ui_text(const char *english);
const char *xemu_ui_chinese_font_path();
void xemu_ui_add_chinese_glyphs(ImFontGlyphRangesBuilder &builder);

#define XEMU_TR(text) xemu_ui_text(text)
