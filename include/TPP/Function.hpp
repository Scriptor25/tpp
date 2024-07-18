#pragma once

#include <TPP/Environment.hpp>
#include <TPP/Name.hpp>
#include <TPP/SourceLocation.hpp>
#include <TPP/Value.hpp>
#include <vector>

namespace tpp
{
	struct Function
	{
		virtual ~Function();

		virtual SourceLocation GetLocation() = 0;
		virtual Name GetName() = 0;
		virtual size_t GetArgCount() = 0;
		virtual bool HasVarArgs() = 0;
		virtual bool IsComplete() = 0;

		virtual ValPtr Call(Environment &parent, const std::vector<ValPtr> &args) = 0;
	};
}
