#pragma once

#include <filesystem>

namespace tpp
{
	struct SourceLocation
	{
		static const SourceLocation UNKNOWN;

		std::filesystem::path Filepath{};
		size_t Row{};
		size_t Column{};
	};
}
