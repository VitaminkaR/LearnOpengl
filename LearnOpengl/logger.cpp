#include "logger.h"

#include <iostream>
#include <stdio.h>
#include <stdarg.h>

namespace LOGL
{

	void setColor(const char* color)
	{
		std::cout << "\033[" << color << "m";
	}

	void resetColor()
	{
		std::cout << "\033[0m";
	}

	void log(const char* str, ...)
	{
		setColor(COLOR_GREEN);
		std::cout << "[LOG] ";

		va_list args;
		va_start(args, str);
		vprintf(str, args);
		va_end(args);
		std::cout << std::endl;

		resetColor();
	}

	void error(const char* str, ...)
	{
		setColor(COLOR_RED);
		std::cout << "[ERROR] ";

		va_list args;
		va_start(args, str);
		vprintf(str, args);
		va_end(args);
		std::cout << std::endl;

		resetColor();
	}

	void warning(const char* str, ...)
	{
		setColor(COLOR_YELLOW);

		std::cout << "[WARNING] ";
		va_list args;
		va_start(args, str);
		vprintf(str, args);
		va_end(args);
		std::cout << std::endl;

		resetColor();
	}

}