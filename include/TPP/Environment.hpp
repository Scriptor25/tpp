#pragma once

#include <TPP/Name.hpp>
#include <TPP/TPP.hpp>
#include <map>
#include <vector>

namespace tpp
{
	class Environment
	{
	public:
		Environment();
		Environment(Environment &parent);
		Environment(Environment &parent, const std::vector<std::shared_ptr<ValueBase>> &varargs);

		void DefineVariable(const SourceLocation &location, const Name &name, const std::shared_ptr<ValueBase> &value);

		template <typename T> T Call(const Name &name, const std::vector<std::shared_ptr<ValueBase>> &args) { return {}; }

	private:
		Environment *m_Parent;
		Environment *m_Global;
		std::vector<std::shared_ptr<ValueBase>> m_VarArgs;
		std::map<Name, std::shared_ptr<ValueBase>> m_Variables;
	};
}
