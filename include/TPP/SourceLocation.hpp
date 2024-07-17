#pragma once

#include <filesystem>

namespace tpp
{
    struct SourceLocation
    {
        std::filesystem::path Filepath{};
        size_t Row{};
        size_t Column{};
    };
}
