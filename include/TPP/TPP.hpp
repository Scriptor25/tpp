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
	struct Type;

	typedef std::shared_ptr<Expression> ExprPtr;
}
