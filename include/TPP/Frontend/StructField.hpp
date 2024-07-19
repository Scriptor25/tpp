#pragma once

#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Name.hpp>

namespace tpp
{
	struct StructField
	{
		StructField(const TypePtr &type, const Name &name, const ExprPtr &init);

		TypePtr MType;
		Name MName;
		ExprPtr Init;
	};

	std::ostream &operator<<(std::ostream &out, const StructField &field);
}
