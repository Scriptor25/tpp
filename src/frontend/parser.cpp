#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Parser.hpp>
#include <TPP/Frontend/SourceLocation.hpp>
#include <fstream>
#include <memory>

std::map<std::string, unsigned> tpp::Parser::PRECEDENCES = {
	{ "=", 0 },	 { "<<=", 0 }, { ">>=", 0 }, { ">>>=", 0 }, { "+=", 0 },  { "-=", 0 }, { "*=", 0 }, { "/=", 0 }, { "%=", 0 }, { "&=", 0 },
	{ "|=", 0 }, { "^=", 0 },  { "&&", 1 },	 { "||", 1 },	{ "<", 2 },	  { ">", 2 },  { "<=", 2 }, { ">=", 2 }, { "==", 2 }, { "&", 3 },
	{ "|", 3 },	 { "^", 3 },   { "<<", 4 },	 { ">>", 4 },	{ ">>>", 4 }, { "+", 5 },  { "-", 5 },	{ "*", 6 },	 { "/", 6 },  { "%", 6 },
};

void tpp::Parser::ParseFile(const std::filesystem::path &filepath, std::vector<std::filesystem::path> &parsed, const TPPCallback &callback)
{
	if (std::find(parsed.begin(), parsed.end(), filepath) != parsed.end()) return;
	parsed.push_back(filepath);

	std::ifstream stream(filepath);
	if (!stream.is_open()) error(SourceLocation::UNKNOWN, "failed to open file: %s", filepath.string().c_str());
	Parser parser(stream, filepath, parsed, callback);
	for (ExprPtr expression; (expression = parser.GetNext());) { callback(expression); }
}

void tpp::Parser::ParseFile(const std::filesystem::path &filepath, const TPPCallback &callback)
{
	std::vector<std::filesystem::path> parsed;
	ParseFile(filepath, parsed, callback);
}

tpp::Parser::Parser(std::istream &stream, const std::filesystem::path &filepath, std::vector<std::filesystem::path> &parsed, const TPPCallback &callback)
	: m_Filepath(filepath), m_Parsed(parsed), m_Callback(callback), m_Location{ filepath, 1, 0 }, m_Stream(stream)
{
	Next();
}

tpp::ExprPtr tpp::Parser::GetNext()
{
	while (!AtEOF())
	{
		if (At("include"))
		{
			ParseInclude();
			continue;
		}
		if (At(":"))
		{
			ParseNamespace();
			continue;
		}
		return Parse();
	}

	return {};
}

int tpp::Parser::Get()
{
	++m_Location.Column;
	return m_Stream.get();
}

void tpp::Parser::Escape()
{
	if (chr != '\\') return;

	chr = Get();
	switch (chr)
	{
	case 'a': chr = '\a'; break;
	case 'b': chr = '\b'; break;
	case 'e': chr = '\e'; break;
	case 'f': chr = '\f'; break;
	case 'n': chr = '\n'; break;
	case 'r': chr = '\r'; break;
	case 't': chr = '\t'; break;
	case 'v': chr = '\v'; break;
	case 'u':
	{
		std::string value;
		for (unsigned i = 0; i < 4; ++i)
		{
			chr = Get();
			value += (char) chr;
		}
		chr = std::stoi(value, 0, 16);
		break;
	}
	case 'x':
	{
		std::string value;
		for (unsigned i = 0; i < 2; ++i)
		{
			chr = Get();
			value += (char) chr;
		}
		chr = std::stoi(value, 0, 16);
		break;
	}
	}
}

void tpp::Parser::NewLine()
{
	m_Location.Column = 0;
	++m_Location.Row;
}

static bool isOp(int chr) { return chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '%' || chr == '&' || chr == '|' || chr == '^' || chr == '=' || chr == '<' || chr == '>'; }

tpp::Token &tpp::Parser::Next()
{
	if (chr < 0) chr = Get();

	while (0x00 <= chr && chr <= 0x20)
	{
		if (chr == '\n') NewLine();
		chr = Get();
	}

	auto mode = ParserMode_Normal;
	std::string value;
	SourceLocation loc;

	while (chr >= 0)
	{
		switch (mode)
		{
		case ParserMode_Normal:
			switch (chr)
			{
			case '#': mode = ParserMode_Comment; break;

			case '"':
				loc = m_Location;
				mode = ParserMode_String;
				break;

			case '\'':
				loc = m_Location;
				mode = ParserMode_Char;
				break;

			default:
				if (chr <= 0x20)
				{
					if (chr == '\n') NewLine();
					break;
				}

				if (isdigit(chr))
				{
					loc = m_Location;
					mode = ParserMode_Number;
					value += (char) chr;
					break;
				}

				if (isalnum(chr) || chr == '_')
				{
					loc = m_Location;
					mode = ParserMode_Id;
					value += (char) chr;
					break;
				}

				if (isOp(chr))
				{
					loc = m_Location;
					mode = ParserMode_BinaryOperator;
					value += (char) chr;
					break;
				}

				loc = m_Location;
				value += (char) chr;
				chr = Get();
				return m_Token = { loc, TokenType_Other, value };
			}
			break;

		case ParserMode_Comment:
			if (chr == '#') { mode = ParserMode_Normal; }
			break;

		case ParserMode_String:
			if (chr == '"')
			{
				chr = Get();
				return m_Token = { loc, TokenType_String, value };
			}
			if (chr == '\\') { Escape(); }
			value += (char) chr;
			break;

		case ParserMode_Char:
			if (chr == '\'')
			{
				chr = Get();
				return m_Token = { loc, TokenType_Char, value };
			}
			if (chr == '\\') { Escape(); }
			value += (char) chr;
			break;

		case ParserMode_Number:
			if (chr == '.')
			{
				value += (char) chr;
				break;
			}
			if (!isdigit(chr)) return m_Token = { loc, TokenType_Number, value };
			value += (char) chr;
			break;

		case ParserMode_Id:
			if (!isalnum(chr) && chr != '_') return m_Token = { loc, TokenType_Id, value };
			value += (char) chr;
			break;

		case ParserMode_BinaryOperator:
			if (!isOp(chr)) return m_Token = { loc, TokenType_BinaryOperator, value };
			value += (char) chr;
			break;

		default: break;
		}

		chr = Get();
	}

	return m_Token = {};
}

bool tpp::Parser::AtEOF() { return !m_Token.Type; }

bool tpp::Parser::At(const TokenType type) { return m_Token.Type == type; }

bool tpp::Parser::At(const std::string &value) { return m_Token.Value == value; }

bool tpp::Parser::NextIfAt(const TokenType type)
{
	if (At(type))
	{
		Next();
		return true;
	}
	return false;
}

bool tpp::Parser::NextIfAt(const std::string &value)
{
	if (At(value))
	{
		Next();
		return true;
	}
	return false;
}

tpp::Token tpp::Parser::Expect(const TokenType type)
{
	if (At(type))
	{
		auto token = m_Token;
		Next();
		return token;
	}
	error(m_Token.Location, "unexpected token: %s", m_Token.Value.c_str());
}

void tpp::Parser::Expect(const std::string &value)
{
	if (At(value))
	{
		Next();
		return;
	}
	error(m_Token.Location, "unexpected token: %s", m_Token.Value.c_str());
}

tpp::Token tpp::Parser::Skip()
{
	auto token = m_Token;
	Next();
	return token;
}

void tpp::Parser::ParseInclude()
{
	Expect("include");
	auto filename = Expect(TokenType_String).Value;
	std::filesystem::path path(filename);
	if (!path.is_absolute()) path = m_Filepath.parent_path() / filename;

	ParseFile(path, m_Parsed, m_Callback);
}

void tpp::Parser::ParseNamespace()
{
	Expect(":");
	auto name = Expect(TokenType_Id).Value;

	if (m_Namespace.empty() || m_Namespace.back() != name) m_Namespace.push_back(name);
	else
		m_Namespace.pop_back();
}

tpp::Name tpp::Parser::ParseName(bool inherit)
{
	std::string name = Expect(TokenType_Id).Value;

	if (!At(":"))
	{
		if (inherit) return { m_Namespace, name };
		return { name };
	}

	while (NextIfAt(":")) { name += ':' + Expect(TokenType_Id).Value; }

	return { name };
}

tpp::ExprPtr tpp::Parser::Parse()
{
	if (At("def")) return ParseDef();

	return ParseBinary();
}

tpp::ExprPtr tpp::Parser::ParseDef()
{
	auto location = m_Token.Location;

	Expect("def");

	std::string native_name;
	if (NextIfAt("native"))
	{
		Expect("(");
		native_name = Expect(TokenType_String).Value;
		Expect(")");
	}

	auto name = ParseName(!m_InFunction);

	if (NextIfAt("("))
	{
		std::vector<std::string> arg_names;
		bool has_var_args = false;
		while (!At(")"))
		{
			if (NextIfAt("?"))
			{
				has_var_args = true;
				break;
			}

			arg_names.push_back(Expect(TokenType_Id).Value);

			if (!At(")")) Expect(",");
		}
		Expect(")");

		std::string promise;
		if (NextIfAt("->")) promise = Expect(TokenType_Id).Value;

		ExprPtr body;
		if (NextIfAt("="))
		{
			auto backup = m_InFunction;
			m_InFunction = true;
			body = Parse();
			m_InFunction = backup;
		}

		return std::make_shared<DefFunctionExpression>(location, native_name, name, arg_names, has_var_args, promise, body);
	}

	if (NextIfAt("["))
	{
		auto size = Parse();
		Expect("]");

		ExprPtr init;
		if (NextIfAt("=")) init = Parse();

		return std::make_shared<DefVariableExpression>(location, native_name, name, size, init);
	}

	ExprPtr init;
	if (NextIfAt("=")) init = Parse();

	return std::make_shared<DefVariableExpression>(location, native_name, name, init);
}

tpp::ExprPtr tpp::Parser::ParseFor()
{
	auto location = m_Token.Location;

	Expect("for");
	Expect("[");
	auto from = Parse();
	Expect(",");
	auto to = Parse();

	ExprPtr step;
	if (!NextIfAt("]"))
	{
		Expect(",");
		step = Parse();
		Expect("]");
	}

	std::string id;
	if (NextIfAt("->")) id = Expect(TokenType_Id).Value;

	auto body = Parse();
	return std::make_shared<ForExpression>(location, from, to, step, id, body);
}

tpp::ExprPtr tpp::Parser::ParseWhile()
{
	auto location = m_Token.Location;

	Expect("while");
	Expect("[");
	auto condition = Parse();
	Expect("]");
	auto body = Parse();

	return std::make_shared<WhileExpression>(location, condition, body);
}

tpp::ExprPtr tpp::Parser::ParseIf()
{
	auto location = m_Token.Location;

	Expect("if");
	Expect("[");
	auto condition = Parse();
	Expect("]");
	auto branchTrue = Parse();

	ExprPtr branchFalse;
	if (NextIfAt("else")) branchFalse = Parse();

	return std::make_shared<IfExpression>(location, condition, branchTrue, branchFalse);
}

tpp::ExprPtr tpp::Parser::ParseGroup()
{
	auto location = m_Token.Location;

	Expect("(");
	std::vector<ExprPtr> body;
	while (!NextIfAt(")")) body.push_back(Parse());

	return std::make_shared<GroupExpression>(location, body);
}

tpp::ExprPtr tpp::Parser::ParseBinary() { return ParseBinary(ParseCall(), 0); }

tpp::ExprPtr tpp::Parser::ParseBinary(ExprPtr lhs, unsigned min_prec)
{
	while (At(TokenType_BinaryOperator) && PRECEDENCES[m_Token.Value] >= min_prec)
	{
		auto op = Skip().Value;
		auto op_prec = PRECEDENCES[op];
		auto rhs = ParseCall();
		while (At(TokenType_BinaryOperator) && PRECEDENCES[m_Token.Value] > op_prec)
		{
			auto la_prec = PRECEDENCES[m_Token.Value];
			rhs = ParseBinary(rhs, op_prec + (la_prec > op_prec ? 1 : 0));
		}
		lhs = std::make_shared<BinaryExpression>(lhs->Location, op, lhs, rhs);
	}
	return lhs;
}

tpp::ExprPtr tpp::Parser::ParseCall()
{
	auto callee = ParseIndex();
	if (NextIfAt("("))
	{
		std::vector<ExprPtr> args;
		while (!NextIfAt(")"))
		{
			args.push_back(Parse());
			if (!At(")")) Expect(",");
		}

		auto name = std::dynamic_pointer_cast<IDExpression>(callee)->MName;
		callee = std::make_shared<CallExpression>(callee->Location, name, args);
	}

	return callee;
}

tpp::ExprPtr tpp::Parser::ParseIndex()
{
	auto array = ParseMember();
	while (NextIfAt("["))
	{
		auto index = Parse();
		Expect("]");

		array = std::make_shared<IndexExpression>(array->Location, array, index);
	}

	return array;
}

tpp::ExprPtr tpp::Parser::ParseMember() { return ParseMember(ParsePrimary()); }

tpp::ExprPtr tpp::Parser::ParseMember(ExprPtr object)
{
	while (NextIfAt("."))
	{
		auto member = Expect(TokenType_Id).Value;
		object = std::make_shared<MemberExpression>(object->Location, object, member);
	}

	return object;
}

tpp::ExprPtr tpp::Parser::ParsePrimary()
{
	if (AtEOF()) error(SourceLocation::UNKNOWN, "reached end of file");

	auto location = m_Token.Location;

	if (At("for")) return ParseFor();

	if (At("while")) return ParseWhile();

	if (At("if")) return ParseIf();

	if (At(TokenType_Id))
	{
		auto name = ParseName();
		return std::make_shared<IDExpression>(location, name);
	}

	if (At(TokenType_Number))
	{
		auto value = Skip().Value;
		return std::make_shared<NumberExpression>(location, value);
	}

	if (At(TokenType_Char))
	{
		auto value = Skip().Value;
		return std::make_shared<CharExpression>(location, value);
	}

	if (At(TokenType_String))
	{
		auto value = Skip().Value;
		return std::make_shared<StringExpression>(location, value);
	}

	if (At("(")) return ParseGroup();

	if (NextIfAt("?")) return std::make_shared<VarArgsExpression>(location);

	if (NextIfAt("!"))
	{
		auto operand = Parse();
		return std::make_shared<UnaryExpression>(location, "!", operand);
	}

	if (NextIfAt("-"))
	{
		auto operand = Parse();
		return std::make_shared<UnaryExpression>(location, "-", operand);
	}

	if (NextIfAt("{"))
	{
		std::map<std::string, ExprPtr> fields;
		while (!NextIfAt("}"))
		{
			auto name = Expect(TokenType_Id).Value;

			ExprPtr value;
			if (At("["))
			{
				auto loc = m_Token.Location;

				Next();
				auto size = Parse();
				Expect("]");

				ExprPtr init;
				if (NextIfAt("=")) init = Parse();

				value = std::make_shared<SizedArrayExpression>(loc, size, init);
			}
			else
			{
				Expect("=");
				value = Parse();
			}

			fields[name] = value;

			if (!At("}")) Expect(",");
		}
		return std::make_shared<ObjectExpression>(location, fields);
	}

	if (NextIfAt("["))
	{
		std::vector<ExprPtr> values;
		while (!NextIfAt("]"))
		{
			values.push_back(Parse());
			if (!At("]")) Expect(",");
		}
		return std::make_shared<ArrayExpression>(location, values);
	}

	error(m_Token.Location, "unhandled token: %s", m_Token.Value.c_str());
}
