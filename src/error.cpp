#include <TPP/TPP.hpp>
#include <cstdarg>
#include <cstdio>
#include <stdexcept>

void tpp::error(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	::putchar('\n');
	va_end(ap);
	throw std::runtime_error("error");
}
