#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Parser.hpp>
#include <TPP/Frontend/SourceLocation.hpp>
#include <TPP/Frontend/StructElement.hpp>
#include <TPP/Frontend/Type.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

std::map<std::string, unsigned> tpp::Parser::PRECEDENCES = {
	{ "=", 0 },	 { "<<=", 0 }, { ">>=", 0 }, { ">>>=", 0 }, { "+=", 0 },  { "-=", 0 }, { "*=", 0 }, { "/=", 0 }, { "%=", 0 }, { "&=", 0 },
	{ "|=", 0 }, { "^=", 0 },  { "&&", 1 },	 { "||", 1 },	{ "<", 2 },	  { ">", 2 },  { "<=", 2 }, { ">=", 2 }, { "==", 2 }, { "&", 3 },
	{ "|", 3 },	 { "^", 3 },   { "<<", 4 },	 { ">>", 4 },	{ ">>>", 4 }, { "+", 5 },  { "-", 5 },	{ "*", 6 },	 { "/", 6 },  { "%", 6 },
};

void tpp::Parser::ParseFile(const std::filesystem::path &filepath, std::vector<std::filesystem::path> &parsed, const TPPCallback &callback)
{
	auto fp = std::filesystem::canonical(filepath);

	if (std::find(parsed.begin(), parsed.end(), fp) != parsed.end()) return;
	parsed.push_back(fp);

	std::ifstream stream(fp);
	if (!stream.is_open()) error(SourceLocation::UNKNOWN, "failed to open file: %s", fp.string().c_str());
	Parser parser(stream, fp, parsed, callback);
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
		if (At("struct"))
		{
			ParseStruct();
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

static bool isOp(int chr)
{
	return chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '%' || chr == '&' || chr == '|' || chr == '^' || chr == '=' || chr == '<' || chr == '>' || chr == '?';
}

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

void tpp::Parser::ParseStruct()
{
	Expect("struct");
	auto name = Expect(TokenType_Id).Value;

	std::vector<StructElement> elements;
	if (NextIfAt("{"))
		while (!NextIfAt("}"))
		{
			auto etype = ParseType();
			auto ename = ParseName();
			ExprPtr einit;
			if (NextIfAt("=")) einit = Parse();
			elements.emplace_back(etype, ename, einit);
			if (!At("}")) Expect(",");
		}

	Type::CreateStruct(name, elements);
}

tpp::Name tpp::Parser::ParseName()
{
	std::vector<std::string> path;
	path.push_back(Expect(TokenType_Id).Value);

	if (!At(":")) { return path; }

	while (NextIfAt(":")) path.push_back(Expect(TokenType_Id).Value);
	return path;
}

tpp::TypePtr tpp::Parser::ParseType()
{
	if (NextIfAt("["))
	{
		auto base = ParseType();
		Expect("]");
		return Type::GetArray(base);
	}

	auto name = Expect(TokenType_Id).Value;
	return Type::Get(name, true);
}

tpp::ExprPtr tpp::Parser::Parse()
{
	if (At("def")) return ParseDef();
	if (At("->")) return ParseReturn();

	return ParseBinary();
}

tpp::ExprPtr tpp::Parser::ParseDef()
{
	auto location = m_Token.Location;
	Expect("def");

	auto type = ParseType();
	Name name;

	if (!(At("=") || At("(") || At("["))) { name = ParseName(); }
	else
	{
		name = type->Name;
		type = {};
	}

	if (!m_InFunction) name = { m_Namespace, name };

	if (NextIfAt("("))
	{
		std::vector<Arg> args;
		bool var_arg = false;

		while (!NextIfAt(")"))
		{
			if (NextIfAt("?"))
			{
				var_arg = true;
				Expect(")");
				break;
			}

			auto arg_type = ParseType();
			auto arg_name = Expect(TokenType_Id).Value;
			args.emplace_back(arg_type, arg_name);
			if (!At(")")) Expect(",");
		}

		m_InFunction = true;

		ExprPtr body;
		if (NextIfAt("=")) body = Parse();

		m_InFunction = false;

		return std::make_shared<DefFunctionExpression>(location, type, name, args, var_arg, body);
	}

	ExprPtr size;
	if (NextIfAt("["))
	{
		size = Parse();
		Expect("]");
	}

	ExprPtr init;
	if (NextIfAt("=")) init = Parse();

	return std::make_shared<DefVariableExpression>(location, type, name, size, init);
}

tpp::ExprPtr tpp::Parser::ParseReturn()
{
	auto location = m_Token.Location;
	Expect("->");
	auto result = Parse();
	return std::make_shared<ReturnExpression>(location, result);
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
	if (NextIfAt(":")) id = Expect(TokenType_Id).Value;

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
		std::vector<ExprPtr> init;
		while (!NextIfAt("}"))
		{
			init.push_back(Parse());
			if (!At("}")) Expect(",");
		}
		return std::make_shared<ObjectExpression>(location, init);
	}

	if (NextIfAt("["))
	{
		auto size = Parse();
		ExprPtr init;
		if (NextIfAt(",")) init = Parse();
		Expect("]");
		return std::make_shared<ArrayExpression>(location, size, init);
	}

	error(m_Token.Location, "unhandled token: %s", m_Token.Value.c_str());
}
