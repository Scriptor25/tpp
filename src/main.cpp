#include <TPP/Expression.hpp>
#include <TPP/Name.hpp>
#include <TPP/Parser.hpp>
#include <TPP/TPP.hpp>
#include <iostream>

int main(const int argc, const char **argv)
{
	if (argc != 2)
	{
		std::cout << "usage: t++ <filename>" << std::endl;
		return 1;
	}

	const std::string filename = argv[1];

	tpp::Parser::ParseFile(filename, [](const tpp::ExprPtr expression) { std::cout << expression << std::endl; });
}
