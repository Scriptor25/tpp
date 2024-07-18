#include <TPP/Type.hpp>
#include <memory>
#include <stdexcept>

tpp::TypePtr tpp::Type::Get(const std::string &name)
{
	if (const auto &type = TYPES[name]) return type;
	throw std::runtime_error("no such type");
}

tpp::TypePtr tpp::Type::GetNumber() { return Get("number"); }

tpp::TypePtr tpp::Type::GetChar() { return Get("char"); }

tpp::TypePtr tpp::Type::GetString() { return Get("string"); }

tpp::TypePtr tpp::Type::GetArray() { return Get("array"); }

tpp::TypePtr tpp::Type::GetObject() { return Get("object"); }

std::map<std::string, tpp::TypePtr> tpp::Type::TYPES = {
	{ "number", std::shared_ptr<Type>(new Type("number")) }, { "char", std::shared_ptr<Type>(new Type("char")) },	  { "string", std::shared_ptr<Type>(new Type("string")) },
	{ "array", std::shared_ptr<Type>(new Type("array")) },	 { "object", std::shared_ptr<Type>(new Type("object")) },
};

tpp::Type::Type(const std::string &name) : Name(name) {}
