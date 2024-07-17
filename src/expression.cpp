#include <TPP/Environment.hpp>
#include <TPP/Expression.hpp>
#include <TPP/Value.hpp>

tpp::Expression::Expression(const SourceLocation &location)
    : Location(location)
{
}

tpp::Expression::~Expression() = default;

tpp::ExprPtr tpp::Expression::MakeConstant()
{
    return shared_from_this();
}

tpp::DefFunctionExpression::DefFunctionExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const std::vector<std::string> &arg_names, bool has_var_args, const ExprPtr &body)
    : Expression(location), NativeName(native_name), MName(name), ArgNames(arg_names), HasVarArgs(has_var_args), Body(body)
{
}

std::shared_ptr<tpp::ValueBase> tpp::DefFunctionExpression::Evaluate(Environment &env)
{
    return {};
}

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &init)
    : Expression(location), NativeName(native_name), MName(name), Init(init)
{
}

tpp::DefVariableExpression::DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &size, const ExprPtr &init)
    : Expression(location), NativeName(native_name), MName(name), Size(size), Init(init)
{
}

std::shared_ptr<tpp::ValueBase> tpp::DefVariableExpression::Evaluate(Environment &env)
{
    return {};
}

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
        if (!Id.empty())
            env1.DefineVariable(Location, Id, CreateValue(i));
        result = Body->Evaluate(env1);
    }

    return result;
}

tpp::WhileExpression::WhileExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &body)
    : Expression(location), Condition(condition), Body(body)
{
}

std::shared_ptr<tpp::ValueBase> tpp::WhileExpression::Evaluate(Environment &env)
{
    std::shared_ptr<ValueBase> result;
    while (true)
    {
        auto c = Condition->Evaluate(env);
        if (!c->GetBool())
            break;
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
    if (c->GetBool())
        return BranchTrue->Evaluate(env);

    if (BranchFalse)
        return BranchFalse->Evaluate(env);

    return {};
}

tpp::GroupExpression::GroupExpression(const SourceLocation &location, const std::vector<ExprPtr> &body)
    : Expression(location), Body(body)
{
}

std::shared_ptr<tpp::ValueBase> tpp::GroupExpression::Evaluate(Environment &env)
{
    std::shared_ptr<ValueBase> result;
    Environment env1(env);
    for (auto &expression : Body)
        result = expression->Evaluate(env1);
    return result;
}

tpp::BinaryExpression::BinaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &lhs, const ExprPtr &rhs)
    : Expression(location), Operator(op), Lhs(lhs), Rhs(rhs)
{
}

std::shared_ptr<tpp::ValueBase> tpp::BinaryExpression::Evaluate(Environment &env)
{
    return {};
}
