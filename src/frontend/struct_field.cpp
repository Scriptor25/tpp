#include <TPP/Frontend/StructField.hpp>
#include <TPP/Frontend/Type.hpp>

tpp::StructField::StructField(const TypePtr &type, const Name &name, const ExprPtr &init) : MType(type), MName(name), Init(init) {}

std::ostream &tpp::operator<<(std::ostream &out, const StructField &field)
{
	out << field.MType << ' ' << field.MName;
	if (field.Init) out << " = " << field.Init;
	return out;
}
