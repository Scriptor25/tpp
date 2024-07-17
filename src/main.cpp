#include <TPP/Environment.hpp>
#include <TPP/Expression.hpp>
#include <TPP/Name.hpp>
#include <TPP/Parser.hpp>
#include <TPP/TPP.hpp>
#include <TPP/Value.hpp>
#include <iostream>
#include <string>
#include <vector>

int main(const int argc, const char **argv)
{
	if (argc != 2)
	{
		std::cout << "usage: t++ <filename>" << std::endl;
		return 1;
	}

	const std::string filename = argv[1];
	tpp::Environment env;

	tpp::Parser::ParseFile(filename,
						   [&env](const tpp::ExprPtr expression)
						   {
							   expression->Evaluate(env);
						   });

	std::vector<std::shared_ptr<tpp::ValueBase>> args;
	for (int a = 1; a < argc; ++a)
		args.push_back(tpp::CreateValue<std::string>(argv[a]));

	const auto result = env.Call<double>("main", args);
	std::cout << "Exit Code " << result << std::endl;
}
