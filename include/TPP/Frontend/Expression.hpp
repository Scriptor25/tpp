#pragma once

#include <TPP/Frontend/Arg.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Name.hpp>
#include <TPP/Frontend/SourceLocation.hpp>
#include <TPP/Frontend/StructField.hpp>
#include <string>
#include <vector>

namespace tpp
{
	struct Expression : std::enable_shared_from_this<Expression>
	{
		explicit Expression(const SourceLocation &location);
		virtual ~Expression();

		SourceLocation Location;
	};

	struct DefStructExpression : Expression
	{
		DefStructExpression();
		DefStructExpression(const SourceLocation &location, const Name &name, const std::vector<StructField> &fields);

		Name MName;
		std::vector<StructField> Fields;
	};

	struct DefFunctionExpression : Expression
	{
		DefFunctionExpression(const SourceLocation &location, const TypePtr &result, const Name &name, const std::vector<Arg> &args, bool var_arg, const ExprPtr &body);

		TypePtr Result;
		Name MName;
		std::vector<Arg> Args;
		bool VarArg;
		ExprPtr Body;
	};

	struct DefVariableExpression : Expression
	{
		DefVariableExpression(const SourceLocation &location, const TypePtr &type, const Name &name, const ExprPtr &size, const ExprPtr &init);

		TypePtr MType;
		Name MName;
		ExprPtr Size;
		ExprPtr Init;
	};

	struct ReturnExpression : Expression
	{
		ReturnExpression(const SourceLocation &location, const ExprPtr &result);

		ExprPtr Result;
	};

	struct ForExpression : Expression
	{
		ForExpression(const SourceLocation &location, const ExprPtr &from, const ExprPtr &to, const ExprPtr &step, const std::string &id, const ExprPtr &body);

		ExprPtr From;
		ExprPtr To;
		ExprPtr Step;
		std::string Id;
		ExprPtr Body;
	};

	struct WhileExpression : Expression
	{
		WhileExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &body);

		ExprPtr Condition;
		ExprPtr Body;
	};

	struct IfExpression : Expression
	{
		IfExpression(const SourceLocation &location, const ExprPtr &condition, const ExprPtr &branchTrue, const ExprPtr &branchFalse);

		ExprPtr Condition;
		ExprPtr BranchTrue;
		ExprPtr BranchFalse;
	};

	struct GroupExpression : Expression
	{
		GroupExpression(const SourceLocation &location, const std::vector<ExprPtr> &body);

		std::vector<ExprPtr> Body;
	};

	struct BinaryExpression : Expression
	{
		BinaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &lhs, const ExprPtr &rhs);

		std::string Operator;
		ExprPtr Lhs;
		ExprPtr Rhs;
	};

	struct CallExpression : Expression
	{
		CallExpression(const SourceLocation &location, const Name &callee, const std::vector<ExprPtr> &args);

		Name Callee;
		std::vector<ExprPtr> Args;
	};

	struct IndexExpression : Expression
	{
		IndexExpression(const SourceLocation &location, const ExprPtr &array, const ExprPtr &index);

		ExprPtr Array;
		ExprPtr Index;
	};

	struct MemberExpression : Expression
	{
		MemberExpression(const SourceLocation &location, const ExprPtr &object, const std::string &member);

		ExprPtr Object;
		std::string Member;
	};

	struct IDExpression : Expression
	{
		IDExpression(const SourceLocation &location, const Name &name);

		Name MName;
	};

	struct NumberExpression : Expression
	{
		NumberExpression(const SourceLocation &location, const std::string &value);

		double Value;
	};

	struct CharExpression : Expression
	{
		CharExpression(const SourceLocation &location, const std::string &value);

		char Value;
	};

	struct StringExpression : Expression
	{
		StringExpression(const SourceLocation &location, const std::string &value);

		std::string Value;
	};

	struct VarArgsExpression : Expression
	{
		explicit VarArgsExpression(const SourceLocation &location);
	};

	struct UnaryExpression : Expression
	{
		UnaryExpression(const SourceLocation &location, const std::string &op, const ExprPtr &operand);

		std::string Operator;
		ExprPtr Operand;
	};

	struct ObjectExpression : Expression
	{
		ObjectExpression(const SourceLocation &location, const std::vector<ExprPtr> &init);

		std::vector<ExprPtr> Init;
	};

	struct ArrayExpression : Expression
	{
		ArrayExpression(const SourceLocation &location, const ExprPtr &size, const ExprPtr &init);

		ExprPtr Size;
		ExprPtr Init;
	};

	std::ostream &operator<<(std::ostream &out, const DefStructExpression &e);
	std::ostream &operator<<(std::ostream &out, const DefFunctionExpression &e);
	std::ostream &operator<<(std::ostream &out, const DefVariableExpression &e);
	std::ostream &operator<<(std::ostream &out, const ReturnExpression &e);
	std::ostream &operator<<(std::ostream &out, const ForExpression &e);
	std::ostream &operator<<(std::ostream &out, const WhileExpression &e);
	std::ostream &operator<<(std::ostream &out, const IfExpression &e);
	std::ostream &operator<<(std::ostream &out, const GroupExpression &e);
	std::ostream &operator<<(std::ostream &out, const BinaryExpression &e);
	std::ostream &operator<<(std::ostream &out, const CallExpression &e);
	std::ostream &operator<<(std::ostream &out, const IndexExpression &e);
	std::ostream &operator<<(std::ostream &out, const MemberExpression &e);
	std::ostream &operator<<(std::ostream &out, const IDExpression &e);
	std::ostream &operator<<(std::ostream &out, const NumberExpression &e);
	std::ostream &operator<<(std::ostream &out, const CharExpression &e);
	std::ostream &operator<<(std::ostream &out, const StringExpression &e);
	std::ostream &operator<<(std::ostream &out, const VarArgsExpression &e);
	std::ostream &operator<<(std::ostream &out, const UnaryExpression &e);
	std::ostream &operator<<(std::ostream &out, const ObjectExpression &e);
	std::ostream &operator<<(std::ostream &out, const ArrayExpression &e);
}
