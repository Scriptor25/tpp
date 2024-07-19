#include <TPP/Frontend/Arg.hpp>
#include <TPP/Frontend/Type.hpp>

tpp::Arg::Arg(const TypePtr &type, const Name &name) : MType(type), MName(name) {}

std::ostream &tpp::operator<<(std::ostream &out, const Arg &arg) { return out << arg.MType << ' ' << arg.MName; }
