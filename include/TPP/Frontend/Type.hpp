#pragma once

#include <TPP/Frontend/Frontend.hpp>
#include <string>
#include <vector>

namespace tpp
{
	struct Type
	{
		static TypePtr Get(const std::string &name);
		static TypePtr GetArray(const TypePtr &base);
		static TypePtr GetFunction(const TypePtr &result, const std::vector<TypePtr> &args, bool is_var_arg);

		explicit Type(const std::string &name);
		virtual ~Type();

		std::string Name;
	};

	struct ArrayType : Type
	{
		ArrayType(const std::string &name, const TypePtr &base);

		TypePtr Base;
	};

	struct FunctionType : Type
	{
		FunctionType(const std::string &name, const TypePtr &result, const std::vector<TypePtr> &args, bool is_var_arg);

		TypePtr Result;
		std::vector<TypePtr> Args;
		bool IsVarArg;
	};

	std::ostream &operator<<(std::ostream &out, const TypePtr &ptr);
}
