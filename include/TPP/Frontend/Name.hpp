#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace tpp
{
	struct Name
	{
		Name();
		Name(const char *name);
		Name(const std::string &name);
		Name(const std::vector<std::string> &path);
		Name(const std::vector<std::string> &ns, const std::string &name);
		Name(const Name &ns, const std::string &name);
		Name(const std::vector<std::string> &ns, const Name &name);

		std::string String() const;

		bool operator!() const;

		std::vector<std::string> Path;
	};

	bool operator<(const Name &a, const Name &b);

	std::ostream &operator<<(std::ostream &out, const Name &name);
}
