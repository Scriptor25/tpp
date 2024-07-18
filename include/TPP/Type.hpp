#pragma once

#include <TPP/TPP.hpp>
#include <map>
#include <string>

namespace tpp
{
	class Type
	{
	public:
		static TypePtr Get(const std::string &name);
		static TypePtr GetNumber();
		static TypePtr GetChar();
		static TypePtr GetString();
		static TypePtr GetArray();
		static TypePtr GetObject();

	private:
		static std::map<std::string, TypePtr> TYPES;

	private:
		explicit Type(const std::string &name);

	private:
		std::string Name;
	};
}
