#include <TPP/Backend/Builder.hpp>
#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Name.hpp>
#include <TPP/Frontend/Parser.hpp>
#include <iostream>

int main(const int argc, const char **argv)
{
	if (argc != 2)
	{
		std::cout << "usage: t++ <filename>" << std::endl;
		return 1;
	}

	const std::string filename = argv[1];
	tpp::Builder builder;

	tpp::Parser::ParseFile(
		filename,
		[&builder](const tpp::ExprPtr &ptr)
		{
			std::cout << ptr << std::endl;
			builder.GenIR(ptr);
		});
}
