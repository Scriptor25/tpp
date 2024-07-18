#pragma once

#include <TPP/Name.hpp>
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
		std::function<ValPtr(ValPtr value)> Operator;
		TypePtr Result;
		bool Reassign;
	};

	class Environment
	{
	public:
		static FunPtr &GetFunction(const Name &name, size_t arg_count);
		static UnOpInfo GetUnaryOperator(const std::string &op, const TypePtr type);

	private:
		static std::map<Name, std::map<int, FunPtr>> FUNCTIONS;

	public:
		Environment();
		explicit Environment(Environment &parent);
		Environment(Environment &parent, const std::vector<ValPtr> &varargs);

		Environment &GetGlobal();
		std::shared_ptr<ArrayValue> GetVarArgs();

		void DefineVariable(const Name &name, const ValPtr &value);
		ValPtr &GetVariable(const Name &name);

		template <typename T>
		T Call(const Name &name, const std::vector<ValPtr> &args)
		{
			return {};
		}

	private:
		Environment *m_Parent;
		Environment *m_Global;
		std::vector<ValPtr> m_VarArgs;
		std::map<Name, ValPtr> m_Variables;
	};
}
