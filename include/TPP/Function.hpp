#pragma once

#include <TPP/Environment.hpp>
#include <TPP/Name.hpp>
#include <TPP/SourceLocation.hpp>
#include <TPP/Value.hpp>
#include <memory>
#include <vector>

namespace tpp
{
	struct Function
	{
		SourceLocation GetLocation();
		Name GetName();
		size_t GetArgCount();
		bool HasVarArgs();
		bool IsComplete();

		std::shared_ptr<ValueBase> Call(Environment &parent, const std::vector<std::shared_ptr<ValueBase>> &args);
	};
}
