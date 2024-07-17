#pragma once

#include <TPP/Name.hpp>
#include <TPP/SourceLocation.hpp>
#include <TPP/TPP.hpp>
#include <TPP/Value.hpp>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace tpp
{
	struct UnOpInfo
	{
		std::function<std::shared_ptr<ValueBase>(std::shared_ptr<ValueBase> value)> Operator;
		std::shared_ptr<Type> Result;
		bool Reassign;
	};

	class Environment
	{
	public:
		static std::shared_ptr<FunctionValue> GetFunction(const SourceLocation &location, const Name &name, size_t arg_count);
		static UnOpInfo GetUnaryOperator(const SourceLocation &location, const std::string &op, const std::shared_ptr<Type> type);

	public:
		Environment();
		Environment(Environment &parent);
		Environment(Environment &parent, const std::vector<std::shared_ptr<ValueBase>> &varargs);

		Environment &GetGlobal();
		std::shared_ptr<ArrayValue> GetVarArgs(const SourceLocation &location);

		void DefineVariable(const SourceLocation &location, const Name &name, const std::shared_ptr<ValueBase> &value);
		std::shared_ptr<ValueBase> &GetVariable(const SourceLocation &location, const Name &name);

		template <typename T>
		T Call(const Name &name, const std::vector<std::shared_ptr<ValueBase>> &args)
		{
			return {};
		}

	private:
		Environment *m_Parent;
		Environment *m_Global;
		std::vector<std::shared_ptr<ValueBase>> m_VarArgs;
		std::map<Name, std::shared_ptr<ValueBase>> m_Variables;
	};
}
