#pragma once

#include <iostream>
#include <memory>

namespace tpp
{
	class Parser;

	struct SourceLocation;
	struct Name;
	struct Token;
	struct Expression;

	struct StructField;
	struct Arg;

	struct Type;
	struct ArrayType;

	typedef std::shared_ptr<Type> TypePtr;
	typedef std::shared_ptr<ArrayType> ArrayTypePtr;
	typedef std::shared_ptr<Expression> ExprPtr;

	std::ostream &operator<<(std::ostream &out, const ExprPtr &ptr);

	[[__noreturn__]]
	void error(const SourceLocation &location, const char *format, ...);
}
