#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/SourceLocation.hpp>
#include <cstdarg>
#include <cstdio>

void tpp::error(const SourceLocation &location, const char *format, ...)
{
	printf("At %s(%zu,%zu): ", location.Filepath.string().c_str(), location.Row, location.Column);

	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	::putchar('\n');
	va_end(ap);

	throw;
}
