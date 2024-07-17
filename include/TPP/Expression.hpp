#pragma once

#include "TPP/Value.hpp"
#include <TPP/Name.hpp>
#include <TPP/SourceLocation.hpp>
#include <TPP/TPP.hpp>
#include <map>
#include <string>
#include <vector>

namespace tpp
{
	struct Expression : std::enable_shared_from_this<Expression>
	{
		Expression(const SourceLocation &location);
		virtual ~Expression();

		virtual ExprPtr MakeConstant();
		virtual std::shared_ptr<ValueBase> Evaluate(Environment &env) = 0;

		SourceLocation Location;
	};

	struct DefFunctionExpression : Expression
	{
		DefFunctionExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const std::vector<std::string> &arg_names, bool has_var_args, const ExprPtr &body);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::string NativeName;
		Name MName;
		std::vector<std::string> ArgNames;
		bool HasVarArgs;
		ExprPtr Body;
	};

	struct DefVariableExpression : Expression
	{
		DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &init);
		DefVariableExpression(const SourceLocation &location, const std::string &native_name, const Name &name, const ExprPtr &size, const ExprPtr &init);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::string NativeName;
		Name MName;
		ExprPtr Size;
		ExprPtr Init;
	};

	struct ForExpression : Expression
	{
		ForExpression(const SourceLocation &location, const ExprPtr &from, const ExprPtr &to, const ExprPtr &step, const std::string &id, const ExprPtr &body);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		ExprPtr From;
		ExprPtr To;
		ExprPtr Step;
		std::string Id;
		ExprPtr Body;
	};

	struct WhileExpression : Expression
	{
		WhileExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &body);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		ExprPtr Condition;
		ExprPtr Body;
	};

	struct IfExpression : Expression
	{
		IfExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &branchTrue, const ExprPtr &branchFalse);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		ExprPtr Condition;
		ExprPtr BranchTrue;
		ExprPtr BranchFalse;
	};

	struct GroupExpression : Expression
	{
		GroupExpression(const SourceLocation &location, const std::vector<ExprPtr> &body);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::vector<ExprPtr> Body;
	};

	struct BinaryExpression : Expression
	{
		BinaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &lhs, const ExprPtr &rhs);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::string Operator;
		ExprPtr Lhs;
		ExprPtr Rhs;
	};

	struct CallExpression : Expression
	{
		CallExpression(const SourceLocation &location, const ExprPtr &callee, const std::vector<ExprPtr> &args);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::shared_ptr<FunctionValue> Callee;
		std::vector<ExprPtr> Args;
	};

	struct IndexExpression : Expression
	{
		IndexExpression(const SourceLocation &location, const ExprPtr &array, const ExprPtr &index);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		ExprPtr Array;
		ExprPtr Index;
	};

	struct MemberExpression : Expression
	{
		MemberExpression(const SourceLocation &location, const ExprPtr &object, const std::string &member);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		ExprPtr Object;
		std::string Member;
	};

	struct IDExpression : Expression
	{
		IDExpression(const SourceLocation &location, const Name &name);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		Name MName;
	};

	struct NumberExpression : Expression
	{
		NumberExpression(const SourceLocation &location, const std::string &value);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		double Value;
	};

	struct CharExpression : Expression
	{
		CharExpression(const SourceLocation &location, const std::string &value);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		char Value;
	};

	struct StringExpression : Expression
	{
		StringExpression(const SourceLocation &location, const std::string &value);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::string Value;
	};

	struct VarArgsExpression : Expression
	{
		VarArgsExpression(const SourceLocation &location);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;
	};

	struct UnaryExpression : Expression
	{
		UnaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &operand);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::string Operator;
		ExprPtr Operand;
	};

	struct SizedArrayExpression : Expression
	{
		SizedArrayExpression(const SourceLocation &location, const ExprPtr &size, const ExprPtr &init);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		ExprPtr Size;
		ExprPtr Init;
	};

	struct ObjectExpression : Expression
	{
		ObjectExpression(const SourceLocation &location, const std::map<std::string, ExprPtr> &fields);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::map<std::string, ExprPtr> Fields;
	};

	struct ArrayExpression : Expression
	{
		ArrayExpression(const SourceLocation &location, const std::vector<ExprPtr> &values);

		std::shared_ptr<ValueBase> Evaluate(Environment &env) override;

		std::vector<ExprPtr> Values;
	};
}
