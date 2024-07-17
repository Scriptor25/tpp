#include "TPP/SourceLocation.hpp"
#include <TPP/Environment.hpp>
#include <TPP/Expression.hpp>
#include <TPP/Function.hpp>
#include <TPP/Value.hpp>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

tpp::Expression::Expression(const SourceLocation &location) : Location(location) {}

tpp::Expression::~Expression() = default;

tpp::ExprPtr tpp::Expression::MakeConstant() { return shared_from_this(); }

tpp::DefFunctionExpression::DefFunctionExpression(
	const SourceLocation &location, const std::string &native_name, const Name &name, const std::vector<std::string> &arg_names, bool has_var_args, const ExprPtr &body)
	: Expression(location), NativeName(native_name), MName(name), ArgNames(arg_names), HasVarArgs(has_var_args), Body(body)
{
}

std::shared_ptr<tpp::ValueBase> tpp::DefFunctionExpression::Evaluate(Environment &env) { return {}; }

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &init)
	: Expression(location), NativeName(native_name), MName(name), Init(init)
{
}

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &size, const ExprPtr &init)
	: Expression(location), NativeName(native_name), MName(name), Size(size), Init(init)
{
}

std::shared_ptr<tpp::ValueBase> tpp::DefVariableExpression::Evaluate(Environment &env) { return {}; }

tpp::ForExpression::ForExpression(const SourceLocation &location, const ExprPtr &from, const ExprPtr &to, const ExprPtr &step, const std::string &id, const ExprPtr &body)
	: Expression(location), From(from), To(to), Step(step), Id(id), Body(body)
{
}

std::shared_ptr<tpp::ValueBase> tpp::ForExpression::Evaluate(Environment &env)
{
	auto f = From->Evaluate(env)->GetDouble();
	auto t = To->Evaluate(env)->GetDouble();
	auto s = Step ? Step->Evaluate(env)->GetDouble() : 1.0;

	std::shared_ptr<ValueBase> result;
	for (double i = f; i < t; i += s)
	{
		Environment env1(env);
		if (!Id.empty()) env1.DefineVariable(Location, Id, CreateValue(i));
		result = Body->Evaluate(env1);
	}

	return result;
}

tpp::WhileExpression::WhileExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &body) : Expression(location), Condition(condition), Body(body) {}

std::shared_ptr<tpp::ValueBase> tpp::WhileExpression::Evaluate(Environment &env)
{
	std::shared_ptr<ValueBase> result;
	while (true)
	{
		auto c = Condition->Evaluate(env);
		if (!c->GetBool()) break;
		result = Body->Evaluate(env);
	}

	return result;
}

tpp::IfExpression::IfExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &branchTrue, const ExprPtr &branchFalse)
	: Expression(location), Condition(condition), BranchTrue(branchTrue), BranchFalse(branchFalse)
{
}

std::shared_ptr<tpp::ValueBase> tpp::IfExpression::Evaluate(Environment &env)
{
	auto c = Condition->Evaluate(env);
	if (c->GetBool()) return BranchTrue->Evaluate(env);

	if (BranchFalse) return BranchFalse->Evaluate(env);

	return {};
}

tpp::GroupExpression::GroupExpression(const SourceLocation &location, const std::vector<ExprPtr> &body) : Expression(location), Body(body) {}

std::shared_ptr<tpp::ValueBase> tpp::GroupExpression::Evaluate(Environment &env)
{
	std::shared_ptr<ValueBase> result;
	Environment env1(env);
	for (auto &expression : Body) result = expression->Evaluate(env1);
	return result;
}

tpp::BinaryExpression::BinaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &lhs, const ExprPtr &rhs) : Expression(location), Operator(op), Lhs(lhs), Rhs(rhs) {}

std::shared_ptr<tpp::ValueBase> tpp::BinaryExpression::Evaluate(Environment &env) { return {}; }

tpp::CallExpression::CallExpression(const SourceLocation &location, const ExprPtr &callee, const std::vector<ExprPtr> &args) : Expression(location), Args(args)
{
	auto name = std::dynamic_pointer_cast<IDExpression>(callee)->MName;
	Callee = Environment::GetFunction(location, name, args.size());
}

std::shared_ptr<tpp::ValueBase> tpp::CallExpression::Evaluate(Environment &env)
{
	std::vector<std::shared_ptr<ValueBase>> args;
	for (const auto &arg : Args) args.push_back(arg->Evaluate(env));

	const auto function = Callee->MFunction;
	if (!function->IsComplete()) throw std::runtime_error("cannot call incomplete function");

	return function->Call(env.GetGlobal(), args);
}

tpp::IndexExpression::IndexExpression(const SourceLocation &location, const ExprPtr &array, const ExprPtr &index) : Expression(location), Array(array), Index(index) {}

std::shared_ptr<tpp::ValueBase> tpp::IndexExpression::Evaluate(Environment &env)
{
	auto index = Index->Evaluate(env);
	auto array = Array->Evaluate(env);

	auto i = index->GetInt();
	return array->GetAt(Location, i);
}

tpp::MemberExpression::MemberExpression(const SourceLocation &location, const ExprPtr &object, const std::string &member) : Expression(location), Object(object), Member(member) {}

std::shared_ptr<tpp::ValueBase> tpp::MemberExpression::Evaluate(Environment &env)
{
	auto object = Object->Evaluate(env);
	return object->GetField(Location, Member);
}

tpp::IDExpression::IDExpression(const SourceLocation &location, const Name &name) : Expression(location), MName(name) {}

std::shared_ptr<tpp::ValueBase> tpp::IDExpression::Evaluate(Environment &env) { return env.GetVariable(Location, MName); }

tpp::NumberExpression::NumberExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(std::stod(value)) {}

std::shared_ptr<tpp::ValueBase> tpp::NumberExpression::Evaluate(Environment &env) { return CreateValue(Value); }

tpp::CharExpression::CharExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(value[0]) {}

std::shared_ptr<tpp::ValueBase> tpp::CharExpression::Evaluate(Environment &env) { return CreateValue(Value); }

tpp::StringExpression::StringExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(value) {}

std::shared_ptr<tpp::ValueBase> tpp::StringExpression::Evaluate(Environment &env) { return CreateValue(Value); }

tpp::VarArgsExpression::VarArgsExpression(const SourceLocation &location) : Expression(location) {}

std::shared_ptr<tpp::ValueBase> tpp::VarArgsExpression::Evaluate(Environment &env) { return env.GetVarArgs(Location); }

tpp::UnaryExpression::UnaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &operand) : Expression(location), Operator(op), Operand(operand) {}

std::shared_ptr<tpp::ValueBase> tpp::UnaryExpression::Evaluate(Environment &env)
{
	auto value = Operand->Evaluate(env);
	auto op = Environment::GetUnaryOperator(Location, Operator, value->GetType(Location));
	return op.Operator(value);
}

tpp::SizedArrayExpression::SizedArrayExpression(const SourceLocation &location, const ExprPtr &size, const ExprPtr &init) : Expression(location), Size(size), Init(init) {}

std::shared_ptr<tpp::ValueBase> tpp::SizedArrayExpression::Evaluate(Environment &env)
{
	auto size = Size->Evaluate(env)->GetInt();
	std::vector<std::shared_ptr<ValueBase>> values;
	for (int i = 0; i < size; ++i) values.push_back(Init ? Init->Evaluate(env) : CreateValue(0.0));
	return std::make_shared<ArrayValue>(Location, values);
}

tpp::ObjectExpression::ObjectExpression(const SourceLocation &location, const std::map<std::string, ExprPtr> &fields) : Expression(location), Fields(fields) {}

std::shared_ptr<tpp::ValueBase> tpp::ObjectExpression::Evaluate(Environment &env)
{
	std::map<std::string, std::shared_ptr<ValueBase>> fields;
	for (const auto &[name, init] : Fields) fields[name] = init->Evaluate(env);
	return std::make_shared<ObjectValue>(Location, fields);
}

tpp::ArrayExpression::ArrayExpression(const SourceLocation &location, const std::vector<ExprPtr> &values) : Expression(location), Values(values) {}

std::shared_ptr<tpp::ValueBase> tpp::ArrayExpression::Evaluate(Environment &env)
{
	std::vector<std::shared_ptr<ValueBase>> values;
	for (const auto &value : Values) values.push_back(value->Evaluate(env));
	return std::make_shared<ArrayValue>(Location, values);
}
