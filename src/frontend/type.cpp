#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/SourceLocation.hpp>
#include <TPP/Frontend/StructElement.hpp>
#include <TPP/Frontend/Type.hpp>
#include <map>
#include <memory>

std::map<std::string, tpp::TypePtr> types;

void tpp::Type::Init()
{
	types.clear();
	Create("i1");
	Create("i8");
	Create("i16");
	Create("i32");
	Create("i64");
	Create("i128");
	Create("f16");
	Create("f32");
	Create("f64");
	Create("void");
}

tpp::TypePtr tpp::Type::Create(const std::string &name) { return types[name] = std::make_shared<Type>(name); }

tpp::TypePtr tpp::Type::CreateStruct(const std::string &name, const std::vector<StructElement> &elements) { return types[name] = std::make_shared<StructType>(name, elements); }

tpp::TypePtr tpp::Type::Get(const std::string &name, bool unsafe)
{
	if (auto &ref = types[name]) return ref;
	if (unsafe) return std::make_shared<Type>(name);
	error(SourceLocation::UNKNOWN, "no such type: %s", name.c_str());
}

tpp::TypePtr tpp::Type::GetArray(const TypePtr &base)
{
	std::string name = '[' + base->Name + ']';
	auto &ref = types[name];
	if (ref) return ref;
	return ref = std::make_shared<ArrayType>(name, base);
}

tpp::TypePtr tpp::Type::GetFunction(const TypePtr &result, const std::vector<TypePtr> &args, bool is_var_arg)
{
	std::string args_string;
	for (size_t i = 0; i < args.size(); ++i)
	{
		if (i > 0) args_string += ',';
		args_string += args[i]->Name;
	}

	std::string name = result->Name + '(' + args_string + ')';
	auto &ref = types[name];
	if (ref) return ref;
	return ref = std::make_shared<FunctionType>(name, result, args, is_var_arg);
}

tpp::TypePtr tpp::Type::GetI1() { return Get("i1"); }

tpp::TypePtr tpp::Type::GetI8() { return Get("i8"); }

tpp::TypePtr tpp::Type::GetI16() { return Get("i16"); }

tpp::TypePtr tpp::Type::GetI32() { return Get("i32"); }

tpp::TypePtr tpp::Type::GetI64() { return Get("i64"); }

tpp::TypePtr tpp::Type::GetI128() { return Get("i128"); }

tpp::TypePtr tpp::Type::GetF16() { return Get("f16"); }

tpp::TypePtr tpp::Type::GetF32() { return Get("f32"); }

tpp::TypePtr tpp::Type::GetF64() { return Get("f64"); }

tpp::TypePtr tpp::Type::GetVoid() { return Get("void"); }

tpp::Type::Type(const std::string &name) : Name(name) {}

tpp::Type::~Type() = default;

tpp::ArrayType::ArrayType(const std::string &name, const TypePtr &base) : Type(name), Base(base) {}

tpp::FunctionType::FunctionType(const std::string &name, const TypePtr &result, const std::vector<TypePtr> &args, bool is_var_arg) : Type(name), Result(result), Args(args), IsVarArg(is_var_arg) {}

tpp::StructType::StructType(const std::string &name, const std::vector<StructElement> &elements) : Type(name), Elements(elements) {}

std::ostream &tpp::operator<<(std::ostream &out, const TypePtr &ptr)
{
	if (!ptr) return out;
	return out << ptr->Name;
}
