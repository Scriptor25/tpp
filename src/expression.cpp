#include <TPP/Environment.hpp>
#include <TPP/Expression.hpp>
#include <TPP/Function.hpp>
#include <TPP/SourceLocation.hpp>
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

tpp::ValPtr tpp::DefFunctionExpression::Evaluate(Environment &env) { throw std::runtime_error("TODO"); }

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &init)
	: Expression(location), NativeName(native_name), MName(name), Init(init)
{
}

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &size, const ExprPtr &init)
	: Expression(location), NativeName(native_name), MName(name), Size(size), Init(init)
{
}

tpp::ValPtr tpp::DefVariableExpression::Evaluate(Environment &env) { throw std::runtime_error("TODO"); }

tpp::ForExpression::ForExpression(const SourceLocation &location, const ExprPtr &from, const ExprPtr &to, const ExprPtr &step, const std::string &id, const ExprPtr &body)
	: Expression(location), From(from), To(to), Step(step), Id(id), Body(body)
{
}

tpp::ValPtr tpp::ForExpression::Evaluate(Environment &env)
{
	auto f = From->Evaluate(env)->GetDouble();
	auto t = To->Evaluate(env)->GetDouble();
	auto s = Step ? Step->Evaluate(env)->GetDouble() : 1.0;

	ValPtr result;
	for (double i = f; i < t; i += s)
	{
		Environment env1(env);
		if (!Id.empty()) env1.DefineVariable(Id, CreateValue(i));
		result = Body->Evaluate(env1);
	}

	return result;
}

tpp::WhileExpression::WhileExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &body) : Expression(location), Condition(condition), Body(body) {}

tpp::ValPtr tpp::WhileExpression::Evaluate(Environment &env)
{
	ValPtr result;
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

tpp::ValPtr tpp::IfExpression::Evaluate(Environment &env)
{
	auto c = Condition->Evaluate(env);
	if (c->GetBool()) return BranchTrue->Evaluate(env);

	if (BranchFalse) return BranchFalse->Evaluate(env);

	return {};
}

tpp::GroupExpression::GroupExpression(const SourceLocation &location, const std::vector<ExprPtr> &body) : Expression(location), Body(body) {}

tpp::ValPtr tpp::GroupExpression::Evaluate(Environment &env)
{
	ValPtr result;
	Environment env1(env);
	for (auto &expression : Body) result = expression->Evaluate(env1);
	return result;
}

tpp::BinaryExpression::BinaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &lhs, const ExprPtr &rhs) : Expression(location), Operator(op), Lhs(lhs), Rhs(rhs) {}

tpp::ValPtr tpp::BinaryExpression::Evaluate(Environment &env) { throw std::runtime_error("TODO"); }

tpp::CallExpression::CallExpression(const SourceLocation &location, const Name &callee, const std::vector<ExprPtr> &args)
	: Expression(location), Callee(Environment::GetFunction(callee, args.size())), Args(args)
{
}

tpp::ValPtr tpp::CallExpression::Evaluate(Environment &env)
{
	std::vector<ValPtr> args;
	for (const auto &arg : Args) args.push_back(arg->Evaluate(env));

	if (!Callee->IsComplete()) throw std::runtime_error("cannot call incomplete function");
	return Callee->Call(env.GetGlobal(), args);
}

tpp::IndexExpression::IndexExpression(const SourceLocation &location, const ExprPtr &array, const ExprPtr &index) : Expression(location), Array(array), Index(index) {}

tpp::ValPtr tpp::IndexExpression::Evaluate(Environment &env)
{
	auto index = Index->Evaluate(env);
	auto array = Array->Evaluate(env);

	auto i = index->GetInt();
	return array->GetAt(i);
}

tpp::MemberExpression::MemberExpression(const SourceLocation &location, const ExprPtr &object, const std::string &member) : Expression(location), Object(object), Member(member) {}

tpp::ValPtr tpp::MemberExpression::Evaluate(Environment &env)
{
	auto object = Object->Evaluate(env);
	return object->GetField(Member);
}

tpp::IDExpression::IDExpression(const SourceLocation &location, const Name &name) : Expression(location), MName(name) {}

tpp::ValPtr tpp::IDExpression::Evaluate(Environment &env) { return env.GetVariable(MName); }

tpp::NumberExpression::NumberExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(std::stod(value)) {}

tpp::ValPtr tpp::NumberExpression::Evaluate(Environment &env) { return std::make_shared<NumberValue>(Value); }

tpp::CharExpression::CharExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(value[0]) {}

tpp::ValPtr tpp::CharExpression::Evaluate(Environment &env) { return std::make_shared<CharValue>(Value); }

tpp::StringExpression::StringExpression(const SourceLocation &location, const std::string &value) : Expression(location), Value(value) {}

tpp::ValPtr tpp::StringExpression::Evaluate(Environment &env) { return std::make_shared<StringValue>(Value); }

tpp::VarArgsExpression::VarArgsExpression(const SourceLocation &location) : Expression(location) {}

tpp::ValPtr tpp::VarArgsExpression::Evaluate(Environment &env) { return env.GetVarArgs(); }

tpp::UnaryExpression::UnaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &operand) : Expression(location), Operator(op), Operand(operand) {}

tpp::ValPtr tpp::UnaryExpression::Evaluate(Environment &env)
{
	auto value = Operand->Evaluate(env);
	auto op = Environment::GetUnaryOperator(Operator, value->GetType());
	return op.Operator(value);
}

tpp::SizedArrayExpression::SizedArrayExpression(const SourceLocation &location, const ExprPtr &size, const ExprPtr &init) : Expression(location), Size(size), Init(init) {}

tpp::ValPtr tpp::SizedArrayExpression::Evaluate(Environment &env)
{
	auto size = Size->Evaluate(env)->GetInt();
	std::vector<ValPtr> values;
	for (int i = 0; i < size; ++i) values.push_back(Init ? Init->Evaluate(env) : CreateValue(0.0));
	return std::make_shared<ArrayValue>(values);
}

tpp::ObjectExpression::ObjectExpression(const SourceLocation &location, const std::map<std::string, ExprPtr> &fields) : Expression(location), Fields(fields) {}

tpp::ValPtr tpp::ObjectExpression::Evaluate(Environment &env)
{
	std::map<std::string, ValPtr> fields;
	for (const auto &[name, init] : Fields) fields[name] = init->Evaluate(env);
	return std::make_shared<ObjectValue>(fields);
}

tpp::ArrayExpression::ArrayExpression(const SourceLocation &location, const std::vector<ExprPtr> &values) : Expression(location), Values(values) {}

tpp::ValPtr tpp::ArrayExpression::Evaluate(Environment &env)
{
	std::vector<ValPtr> values;
	for (const auto &value : Values) values.push_back(value->Evaluate(env));
	return std::make_shared<ArrayValue>(values);
}
