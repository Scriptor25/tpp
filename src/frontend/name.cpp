#include <TPP/Frontend/Name.hpp>

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

tpp::Name::Name() {}

tpp::Name::Name(const char *name) : Name(std::string(name)) {}

tpp::Name::Name(const std::string &name) : Name(split(name, ":")) {}

tpp::Name::Name(const std::vector<std::string> &path) : Path(path) {}

tpp::Name::Name(const std::vector<std::string> &ns, const std::string &name) : Path(ns) { Path.push_back(name); }

tpp::Name::Name(const Name &ns, const std::string &name) : Path(ns.Path) { Path.push_back(name); }

tpp::Name::Name(const std::vector<std::string> &ns, const Name &name) : Path(ns)
{
	for (const auto &p : name.Path) Path.push_back(p);
}

std::string tpp::Name::String() const
{
	std::string str;
	for (size_t i = 0; i < Path.size(); ++i)
	{
		if (i > 0) str += ':';
		str += Path[i];
	}
	return str;
}

bool tpp::Name::operator!() const { return Path.empty(); }

bool tpp::operator<(const Name &a, const Name &b) { return a.Path < b.Path; }

std::ostream &tpp::operator<<(std::ostream &out, const Name &name) { return out << name.String(); }
