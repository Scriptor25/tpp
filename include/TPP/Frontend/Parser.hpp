#pragma once

#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Name.hpp>
#include <TPP/Frontend/Token.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>

namespace tpp
{
	typedef std::function<void(const ExprPtr &)> TPPCallback;

	enum ParserMode
	{
		ParserMode_,
		ParserMode_Normal,
		ParserMode_Comment,
		ParserMode_Number,
		ParserMode_Id,
		ParserMode_BinaryOperator,
		ParserMode_String,
		ParserMode_Char,
	};

	class Parser
	{
	private:
		static void ParseFile(const std::filesystem::path &filepath, std::vector<std::filesystem::path> &parsed, const TPPCallback &callback);

	public:
		static void ParseFile(const std::filesystem::path &filepath, const TPPCallback &callback);

	private:
		Parser(std::istream &stream, const std::filesystem::path &filepath, std::vector<std::filesystem::path> &parsed, const TPPCallback &callback);

		ExprPtr GetNext();

		int Get();
		void Escape();
		void NewLine();
		Token &Next();

		bool AtEOF();
		bool At(const TokenType type);
		bool At(const std::string &value);

		bool NextIfAt(const TokenType type);
		bool NextIfAt(const std::string &value);

		Token Expect(const TokenType type);
		void Expect(const std::string &value);

		Token Skip();

		void ParseInclude();
		void ParseNamespace();

		Name ParseName();
		TypePtr ParseType();

		ExprPtr Parse();
		ExprPtr ParseDef();
		ExprPtr ParseFor();
		ExprPtr ParseWhile();
		ExprPtr ParseIf();
		ExprPtr ParseGroup();
		ExprPtr ParseBinary();
		ExprPtr ParseBinary(ExprPtr lhs, unsigned min_prec);
		ExprPtr ParseCall();
		ExprPtr ParseIndex();
		ExprPtr ParseMember();
		ExprPtr ParseMember(ExprPtr object);
		ExprPtr ParsePrimary();

	private:
		static std::map<std::string, unsigned> PRECEDENCES;

	private:
		std::filesystem::path m_Filepath;
		std::vector<std::filesystem::path> &m_Parsed;
		TPPCallback m_Callback;

		std::istream &m_Stream;
		SourceLocation m_Location;
		int chr = -1;

		std::vector<std::string> m_Namespace;
		bool m_InFunction = false;
		Token m_Token;
	};
}
