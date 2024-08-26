#pragma once

#include <TPP/Frontend/Frontend.hpp>
#include <string>
#include <vector>

namespace tpp
{
	struct Type
	{
		static void Init();

		static TypePtr Create(const std::string &name);
		static TypePtr CreateStruct(const std::string &name, const std::vector<StructElement> &elements);

		static TypePtr Get(const std::string &name, bool unsafe = false);
		static TypePtr GetArray(const TypePtr &base);
		static TypePtr GetFunction(const TypePtr &result, const std::vector<TypePtr> &args, bool is_var_arg);

		static TypePtr GetI1();
		static TypePtr GetI8();
		static TypePtr GetI16();
		static TypePtr GetI32();
		static TypePtr GetI64();
		static TypePtr GetI128();
		static TypePtr GetF16();
		static TypePtr GetF32();
		static TypePtr GetF64();
		static TypePtr GetVoid();

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

	struct StructType : Type
	{
		StructType(const std::string &name, const std::vector<StructElement> &elements);

		std::vector<StructElement> Elements;
	};

	std::ostream &operator<<(std::ostream &out, const TypePtr &ptr);
}
