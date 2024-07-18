#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Name.hpp>
#include <TPP/Frontend/Parser.hpp>
#include <TPP/Frontend/TPP.hpp>
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
