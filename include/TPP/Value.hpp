#pragma once

#include "TPP/SourceLocation.hpp"
#include <TPP/TPP.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace tpp
{
	struct ValueBase
	{
		virtual ~ValueBase();

		virtual std::shared_ptr<Type> GetType(const SourceLocation &location);

		virtual bool GetBool();
		virtual int GetInt();
		virtual double GetDouble();

		virtual std::shared_ptr<ValueBase> GetAt(const SourceLocation &location, int index);

		virtual std::shared_ptr<ValueBase> GetField(const SourceLocation &location, const std::string &name);
	};

	template <typename T>
	struct Value : ValueBase
	{
		Value(const T &content) : Content(content) {}

		T Content;
	};

	template <>
	struct Value<double> : ValueBase
	{
		Value(double content);

		bool GetBool() override;
		double GetDouble() override;

		double Content;
	};

	struct ArrayValue : ValueBase
	{
		ArrayValue(const SourceLocation &location, const std::vector<std::shared_ptr<ValueBase>> &values);
	};

	struct ObjectValue : ValueBase
	{
		ObjectValue(const SourceLocation &location, const std::map<std::string, std::shared_ptr<ValueBase>> &fields);
	};

	struct FunctionValue : ValueBase
	{
		std::shared_ptr<class Function> MFunction;
	};

	template <typename T>
	std::shared_ptr<Value<T>> CreateValue(const T &content)
	{
		return std::make_shared<Value<T>>(content);
	}
}
