#pragma once

#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Name.hpp>

namespace tpp
{
	struct Type
	{
		static TypePtr Get(const Name &name);
		static TypePtr GetArray(const TypePtr &base);

		explicit Type(const Name &name);
		virtual ~Type();

		Name MName;
	};

	struct ArrayType : Type
	{
		explicit ArrayType(const TypePtr &base);

		TypePtr Base;
	};

	std::ostream &operator<<(std::ostream &out, const TypePtr &ptr);
	std::ostream &operator<<(std::ostream &out, const ArrayType &type);
}
