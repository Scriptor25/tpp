#pragma once

#include <string>
#include <vector>

namespace tpp
{
	struct Name
	{
		Name(const char *name);
		Name(const std::string &name);
		Name(const std::vector<std::string> &path);
		Name(const std::vector<std::string> &ns, const std::string &name);

		const char *c_str() const;

		std::vector<std::string> Path;
		std::string Str;
	};

	bool operator<(const Name &a, const Name &b);
}
