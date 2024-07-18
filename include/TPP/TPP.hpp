#pragma once

#include <memory>

namespace tpp
{
	struct Name;

	class Parser;
	struct SourceLocation;
	struct Token;
	struct Expression;

	class Environment;
	struct UnOpInfo;
	struct ValueBase;
	class Type;
	struct Function;

	typedef std::shared_ptr<Expression> ExprPtr;
	typedef std::shared_ptr<ValueBase> ValPtr;
	typedef std::shared_ptr<Type> TypePtr;
	typedef std::shared_ptr<Function> FunPtr;
}
