#include "KitsuneEngine/console_colors.h"

#include <format>

std::string ColorText(const std::string_view &str, const Color &color) {
	return std::format("\033[38;2;{};{};{}m{}\033[0m", color.Red(), color.Green(), color.Blue(), str);
}
