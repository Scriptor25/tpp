#pragma

#include <TPP/SourceLocation.hpp>

namespace tpp
{
	enum TokenType
	{
		TokenType_,
		TokenType_Id,
		TokenType_Number,
		TokenType_Char,
		TokenType_String,
		TokenType_BinaryOperator,
		TokenType_Other,
	};

	struct Token
	{
		SourceLocation Location;
		TokenType Type = TokenType_;
		std::string Value;
	};
}
