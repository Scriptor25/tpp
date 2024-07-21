#pragma once

#include <TPP/Frontend/Frontend.hpp>

namespace tpp
{
	struct Arg
	{
		Arg(const TypePtr &type, const std::string &name);

		TypePtr Type;
		std::string Name;
	};

	std::ostream &operator<<(std::ostream &out, const Arg &arg);
}
