#pragma once

#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Name.hpp>

namespace tpp
{
	struct Arg
	{
		Arg(const TypePtr &type, const Name &name);

		TypePtr MType;
		Name MName;
	};

	std::ostream &operator<<(std::ostream &out, const Arg &arg);
}
