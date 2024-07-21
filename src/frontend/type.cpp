#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Type.hpp>
#include <map>
#include <memory>

std::map<std::string, tpp::TypePtr> types;

tpp::TypePtr tpp::Type::Get(const std::string &name)
{
	auto &ref = types[name];
	if (ref) return ref;
	return ref = std::make_shared<Type>(name);
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

tpp::Type::Type(const std::string &name) : Name(name) {}

tpp::Type::~Type() = default;

tpp::ArrayType::ArrayType(const std::string &name, const TypePtr &base) : Type(name), Base(base) {}

tpp::FunctionType::FunctionType(const std::string &name, const TypePtr &result, const std::vector<TypePtr> &args, bool is_var_arg) : Type(name), Result(result), Args(args), IsVarArg(is_var_arg) {}

std::ostream &tpp::operator<<(std::ostream &out, const TypePtr &ptr)
{
	if (!ptr) return out;
	return out << ptr->Name;
}
