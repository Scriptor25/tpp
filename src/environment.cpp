#include <TPP/Environment.hpp>
#include <TPP/Function.hpp>
#include <stdexcept>

tpp::FunPtr &tpp::Environment::GetFunction(const Name &name, size_t arg_count)
{
	if (auto &funcs = FUNCTIONS[name]; !funcs.empty())
	{
		if (auto &function = funcs[arg_count]) return function;
		for (auto &[argc, function] : funcs)
		{
			if (arg_count < argc) continue;
			if (arg_count > argc && !function->HasVarArgs()) continue;
			return function;
		}
	}

	throw std::runtime_error("undefined function");
}

tpp::UnOpInfo tpp::Environment::GetUnaryOperator(const std::string &op, const TypePtr type) { throw std::runtime_error("TODO"); }

std::map<tpp::Name, std::map<int, tpp::FunPtr>> tpp::Environment::FUNCTIONS;

tpp::Environment::Environment() : m_Parent(nullptr), m_Global(this) {}

tpp::Environment::Environment(Environment &parent) : m_Parent(&parent), m_Global(parent.m_Global) {}

tpp::Environment::Environment(Environment &parent, const std::vector<ValPtr> &varargs) : m_Parent(&parent), m_Global(parent.m_Global), m_VarArgs(varargs) {}

tpp::Environment &tpp::Environment::GetGlobal() { return *m_Global; }

std::shared_ptr<tpp::ArrayValue> tpp::Environment::GetVarArgs() { return std::make_shared<ArrayValue>(m_VarArgs); }

void tpp::Environment::DefineVariable(const Name &name, const ValPtr &value)
{
	if (m_Variables[name]) throw std::runtime_error("variable already defined");

	m_Variables[name] = value;
}

tpp::ValPtr &tpp::Environment::GetVariable(const Name &name)
{
	if (auto &variable = m_Variables[name]) return variable;
	if (m_Parent) m_Parent->GetVariable(name);
	throw std::runtime_error("undefined variable");
}
