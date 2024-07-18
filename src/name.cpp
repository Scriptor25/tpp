#include <TPP/Name.hpp>

static std::vector<std::string> split(const std::string &str, const std::string delim)
{
	std::vector<std::string> path;
	std::string cpy = str;
	for (size_t pos; (pos = cpy.find(delim)) != std::string::npos;)
	{
		path.push_back(cpy.substr(0, pos));
		cpy = cpy.substr(pos + delim.size());
	}
	if (!cpy.empty()) path.push_back(cpy);
	return path;
}

tpp::Name::Name(const char *name) : Name(std::string(name)) {}

tpp::Name::Name(const std::string &name) : Name(split(name, ":")) {}

tpp::Name::Name(const std::vector<std::string> &path) : Path(path)
{
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (i > 0) Str += ':';
		Str += path[i];
	}
}

tpp::Name::Name(const std::vector<std::string> &ns, const std::string &name) : Path(ns)
{
	Path.push_back(name);
	for (size_t i = 0; i < Path.size(); ++i)
	{
		if (i > 0) Str += ':';
		Str += Path[i];
	}
}

const char *tpp::Name::c_str() const { return Str.c_str(); }

bool tpp::operator<(const Name &a, const Name &b) { return a.Str < b.Str; }
