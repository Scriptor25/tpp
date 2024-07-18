#pragma once

#include <TPP/TPP.hpp>
#include <map>
#include <string>
#include <vector>

namespace tpp
{
	struct ValueBase
	{
		virtual ~ValueBase();

		virtual TypePtr GetType() = 0;

		virtual bool GetBool();
		virtual int GetInt();
		virtual double GetDouble();

		virtual ValPtr GetAt(int index);

		virtual ValPtr GetField(const std::string &name);
	};

	struct NumberValue : ValueBase
	{
		NumberValue(double content);

		TypePtr GetType() override;

		bool GetBool() override;
		int GetInt() override;
		double GetDouble() override;

		double Content;
	};

	struct CharValue : ValueBase
	{
		CharValue(char content);

		TypePtr GetType() override;

		bool GetBool() override;
		int GetInt() override;
		double GetDouble() override;

		char Content;
	};

	struct StringValue : ValueBase
	{
		StringValue(const std::string &content);

		TypePtr GetType() override;

		bool GetBool() override;

		std::string Content;
	};

	struct ArrayValue : ValueBase
	{
		ArrayValue(const std::vector<ValPtr> &content);

		TypePtr GetType() override;

		bool GetBool() override;

		std::vector<ValPtr> Content;
	};

	struct ObjectValue : ValueBase
	{
		ObjectValue(const std::map<std::string, ValPtr> &content);

		TypePtr GetType() override;

		bool GetBool() override;

		std::map<std::string, ValPtr> Content;
	};

	template <typename T>
	tpp::ValPtr CreateValue(const T &content);
}
