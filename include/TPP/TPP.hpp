#pragma once

#include <iostream>
#include <memory>

namespace tpp
{
	struct Name;

	class Parser;
	struct SourceLocation;
	struct Token;
	struct Expression;

	typedef std::shared_ptr<Expression> ExprPtr;

	std::ostream &operator<<(std::ostream &out, const ExprPtr &ptr);

	[[__noreturn__]]
	void error(const char *format, ...);
}
