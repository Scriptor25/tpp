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
	struct ValueBase;

	typedef std::shared_ptr<Expression> ExprPtr;
}
