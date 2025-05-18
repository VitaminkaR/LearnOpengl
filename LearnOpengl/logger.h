#pragma once

#define COLOR_RED "91"
#define COLOR_YELLOW "93"
#define COLOR_GREEN "92"

void setColor(const char* color);

void resetColor();

void log(const char* str, ...);

void error(const char* str, ...);

void warning(const char* str, ...);
