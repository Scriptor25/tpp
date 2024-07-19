#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Name.hpp>
#include <TPP/Frontend/Type.hpp>
#include <map>
#include <memory>

std::map<tpp::Name, tpp::TypePtr> types;
std::map<tpp::TypePtr, tpp::ArrayTypePtr> array_types;

tpp::TypePtr tpp::Type::Get(const Name &name)
{
	auto &ref = types[name];
	if (ref) return ref;
	return ref = std::make_shared<Type>(name);
}

tpp::TypePtr tpp::Type::GetArray(const TypePtr &base)
{
	auto &ref = array_types[base];
	if (ref) return ref;
	return ref = std::make_shared<ArrayType>(base);
}

tpp::Type::Type(const Name &name) : MName(name) {}

tpp::Type::~Type() = default;

tpp::ArrayType::ArrayType(const TypePtr &base) : Type({ base->MName, "[]" }), Base(base) {}

std::ostream &tpp::operator<<(std::ostream &out, const TypePtr &ptr)
{
	if (!ptr) return out;
	if (auto p = std::dynamic_pointer_cast<ArrayType>(ptr)) return out << *p;

	return out << ptr->MName;
}

std::ostream &tpp::operator<<(std::ostream &out, const ArrayType &type) { return out << '[' << type.Base << ']'; }
