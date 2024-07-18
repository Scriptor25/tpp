#include <TPP/Expression.hpp>
#include <TPP/SourceLocation.hpp>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

tpp::Expression::Expression(const SourceLocation &location) : Location(location) {}

tpp::Expression::~Expression() = default;

tpp::DefFunctionExpression::DefFunctionExpression(
	const SourceLocation &location, const std::string &native_name, const Name &name, const std::vector<std::string> &arg_names, bool has_var_args, const ExprPtr &body)
	: Expression(location), NativeName(native_name), MName(name), ArgNames(arg_names), HasVarArgs(has_var_args), Body(body)
{
}

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &init)
	: Expression(location), NativeName(native_name), MName(name), Init(init)
{
}

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &size, const ExprPtr &init)
	: Expression(location), NativeName(native_name), MName(name), Size(size), Init(init)
{
}

tpp::ForExpression::ForExpression(const SourceLocation &location, const ExprPtr &from, const ExprPtr &to, const ExprPtr &step, const std::string &id, const ExprPtr &body)
	: Expression(location), From(from), To(to), Step(step), Id(id), Body(body)
{
}

tpp::WhileExpression::WhileExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &body) : Expression(location), Condition(condition), Body(body) {}

tpp::IfExpression::IfExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &branchTrue, const ExprPtr &branchFalse)
	: Expression(location), Condition(condition), BranchTrue(branchTrue), BranchFalse(branchFalse)
{
}

tpp::GroupExpression::GroupExpression(const SourceLocation &location, const std::vector<ExprPtr> &body) : Expression(location), Body(body) {}

tpp::BinaryExpression::BinaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &lhs, const ExprPtr &rhs) : Expression(location), Operator(op), Lhs(lhs), Rhs(rhs) {}

tpp::CallExpression::CallExpression(const SourceLocation &location, const Name &callee, const std::vector<ExprPtr> &args) : Expression(location), Callee(callee), Args(args) {}

tpp::IndexExpression::IndexExpression(const SourceLocation &location, const ExprPtr &array, const ExprPtr &index) : Expression(location), Array(array), Index(index) {}

tpp::MemberExpression::MemberExpression(const SourceLocation &location, const ExprPtr &object, const std::string &member) : Expression(location), Object(object), Member(member) {}

tpp::IDExpression::IDExpression(const SourceLocation &location, const Name &name) : Expression(location), MName(name) {}

tpp::NumberExpression::NumberExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(std::stod(value)) {}

tpp::CharExpression::CharExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(value[0]) {}

tpp::StringExpression::StringExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(value) {}

tpp::VarArgsExpression::VarArgsExpression(const SourceLocation &location) : Expression(location) {}

tpp::UnaryExpression::UnaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &operand) : Expression(location), Operator(op), Operand(operand) {}

tpp::SizedArrayExpression::SizedArrayExpression(const SourceLocation &location, const ExprPtr &size, const ExprPtr &init) : Expression(location), Size(size), Init(init) {}

tpp::ObjectExpression::ObjectExpression(const SourceLocation &location, const std::map<std::string, ExprPtr> &fields) : Expression(location), Fields(fields) {}

tpp::ArrayExpression::ArrayExpression(const SourceLocation &location, const std::vector<ExprPtr> &values) : Expression(location), Values(values) {}

std::ostream &tpp::operator<<(std::ostream &out, const ExprPtr &ptr)
{
	if (!ptr) return out << "<null>";

	if (auto p = std::dynamic_pointer_cast<DefFunctionExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<DefVariableExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<ForExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<WhileExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<IfExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<GroupExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<BinaryExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<CallExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<IndexExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<MemberExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<IDExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<NumberExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<CharExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<StringExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<VarArgsExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<UnaryExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<SizedArrayExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<ObjectExpression>(ptr)) return out << *p;
	if (auto p = std::dynamic_pointer_cast<ArrayExpression>(ptr)) return out << *p;
	throw std::runtime_error("TODO");
}

static unsigned depth = 0;

static std::string get_spaces()
{
	std::string str;
	for (unsigned i = 0; i < depth; ++i) str += "  ";
	return str;
}

static std::string replace(const std::string &str, const std::function<bool(char)> filter, char d)
{
	auto cpy = str;
	for (auto &c : cpy)
		if (filter(c)) c = d;
	return cpy;
}

std::ostream &tpp::operator<<(std::ostream &out, const DefFunctionExpression &e)
{
	out << "def ";
	if (!e.NativeName.empty()) out << "native(\"" << e.NativeName << "\") ";
	out << e.MName.Str << '(';
	for (int i = 0; i < e.ArgNames.size(); ++i)
	{
		if (i > 0) out << ", ";
		out << e.ArgNames[i];
	}
	if (e.HasVarArgs)
	{
		if (!e.ArgNames.empty()) out << ", ";
		out << '?';
	}
	out << ')';
	if (!e.Body) return out;
	return out << " = " << e.Body;
}

std::ostream &tpp::operator<<(std::ostream &out, const DefVariableExpression &e)
{
	out << "def ";
	if (!e.NativeName.empty()) out << "native(\"" << e.NativeName << "\") ";
	out << e.MName.Str;
	if (e.Size) out << '[' << e.Size << ']';
	if (!e.Init) return out;
	return out << " = " << e.Init;
}

std::ostream &tpp::operator<<(std::ostream &out, const ForExpression &e)
{
	out << "for [" << e.From << ", " << e.To;
	if (e.Step) out << ", " << e.Step;
	out << "] ";
	if (!e.Id.empty()) out << "-> " << e.Id << ' ';
	return out << e.Body;
}

std::ostream &tpp::operator<<(std::ostream &out, const WhileExpression &e) { return out << "while [" << e.Condition << "] " << e.Body; }

std::ostream &tpp::operator<<(std::ostream &out, const IfExpression &e)
{
	out << "if [" << e.Condition << "] " << e.BranchTrue;
	if (e.BranchFalse) return out << " else " << e.BranchFalse;
	return out;
}

std::ostream &tpp::operator<<(std::ostream &out, const GroupExpression &e)
{
	if (e.Body.empty()) return out << "()";
	if (e.Body.size() == 1) return out << '(' << e.Body.front() << ')';
	out << '(';
	++depth;
	auto spaces = get_spaces();
	for (const auto &ptr : e.Body) { out << std::endl << spaces << ptr; }
	--depth;
	spaces = get_spaces();
	return out << std::endl << spaces << ')';
}

std::ostream &tpp::operator<<(std::ostream &out, const BinaryExpression &e) { return out << e.Lhs << ' ' << e.Operator << ' ' << e.Rhs; }

std::ostream &tpp::operator<<(std::ostream &out, const CallExpression &e)
{
	out << e.Callee.Str << '(';
	for (size_t i = 0; i < e.Args.size(); ++i)
	{
		if (i > 0) out << ", ";
		out << e.Args[i];
	}
	return out << ')';
}

std::ostream &tpp::operator<<(std::ostream &out, const IndexExpression &e) { return out << e.Array << '[' << e.Index << ']'; }

std::ostream &tpp::operator<<(std::ostream &out, const MemberExpression &e) { return out << e.Object << '.' << e.Member; }

std::ostream &tpp::operator<<(std::ostream &out, const IDExpression &e) { return out << e.MName.Str; }

std::ostream &tpp::operator<<(std::ostream &out, const NumberExpression &e) { return out << e.Value; }

std::ostream &tpp::operator<<(std::ostream &out, const CharExpression &e) { return out << '\'' << (char) (e.Value > 0x20 ? e.Value : 0) << '\''; }

std::ostream &tpp::operator<<(std::ostream &out, const StringExpression &e)
{
	return out << '"' << replace(e.Value, [](char c) { return c <= 0x20; }, 0) << '"';
}

std::ostream &tpp::operator<<(std::ostream &out, const VarArgsExpression &e) { return out << '?'; }

std::ostream &tpp::operator<<(std::ostream &out, const UnaryExpression &e) { return out << e.Operator << e.Operand; }

std::ostream &tpp::operator<<(std::ostream &out, const SizedArrayExpression &e)
{
	out << '[' << e.Size << ']';
	if (!e.Init) return out;
	return out << " = " << e.Init;
}

std::ostream &tpp::operator<<(std::ostream &out, const ObjectExpression &e)
{
	if (e.Fields.empty()) return out << "{}";
	if (e.Fields.size() == 1) return out << '{' << e.Fields.begin()->second << '}';

	out << '{';
	++depth;
	auto spaces = get_spaces();
	size_t i = 0;
	for (const auto &[name, init] : e.Fields)
	{
		if (i++ > 0) out << ',';
		out << std::endl << spaces << name << " = " << init;
	}
	--depth;
	spaces = get_spaces();
	return out << std::endl << spaces << '}';
}

std::ostream &tpp::operator<<(std::ostream &out, const ArrayExpression &e)
{
	out << "[ ";
	for (size_t i = 0; i < e.Values.size(); ++i)
	{
		if (i > 0) out << ", ";
		out << e.Values[i];
	}
	return out << " ]";
}
