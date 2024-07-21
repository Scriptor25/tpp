#include <TPP/Frontend/Arg.hpp>
#include <TPP/Frontend/Type.hpp>

tpp::Arg::Arg(const TypePtr &type, const std::string &name) : Type(type), Name(name) {}

std::ostream &tpp::operator<<(std::ostream &out, const Arg &arg) { return out << arg.Type << ' ' << arg.Name; }
