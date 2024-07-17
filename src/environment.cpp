#include <TPP/Environment.hpp>
#include <stdexcept>

tpp::Environment::Environment() : m_Parent(nullptr), m_Global(this) {}

tpp::Environment::Environment(Environment &parent) : m_Parent(&parent), m_Global(parent.m_Global) {}

tpp::Environment::Environment(Environment &parent, const std::vector<std::shared_ptr<ValueBase>> &varargs) : m_Parent(&parent), m_Global(parent.m_Global), m_VarArgs(varargs) {}

tpp::Environment &tpp::Environment::GetGlobal() { return *m_Global; }

void tpp::Environment::DefineVariable(const SourceLocation &location, const Name &name, const std::shared_ptr<ValueBase> &value)
{
	if (m_Variables[name]) throw std::runtime_error("variable already defined");

	m_Variables[name] = value;
}
