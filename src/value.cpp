#include <TPP/Function.hpp>
#include <TPP/Type.hpp>
#include <TPP/Value.hpp>
#include <stdexcept>

tpp::ValueBase::~ValueBase() = default;

bool tpp::ValueBase::GetBool() { throw std::runtime_error("not implemented"); }

int tpp::ValueBase::GetInt() { throw std::runtime_error("not implemented"); }

double tpp::ValueBase::GetDouble() { throw std::runtime_error("not implemented"); }

tpp::ValPtr tpp::ValueBase::GetAt(int index) { throw std::runtime_error("not implemented"); }

tpp::ValPtr tpp::ValueBase::GetField(const std::string &name) { throw std::runtime_error("not implemented"); }

tpp::NumberValue::NumberValue(double content) : Content(content) {}

tpp::TypePtr tpp::NumberValue::GetType() { return Type::GetNumber(); }

bool tpp::NumberValue::GetBool() { return Content != 0.0; }

int tpp::NumberValue::GetInt() { return (int) Content; }

double tpp::NumberValue::GetDouble() { return Content; }

tpp::CharValue::CharValue(char content) : Content(content) {}

tpp::TypePtr tpp::CharValue::GetType() { return Type::GetChar(); }

bool tpp::CharValue::GetBool() { return Content != 0; }

int tpp::CharValue::GetInt() { return (int) Content; }

double tpp::CharValue::GetDouble() { return (double) Content; }

tpp::StringValue::StringValue(const std::string &content) : Content(content) {}

tpp::TypePtr tpp::StringValue::GetType() { return Type::GetString(); }

bool tpp::StringValue::GetBool() { return !Content.empty(); }

tpp::ArrayValue::ArrayValue(const std::vector<ValPtr> &content) : Content(content) {}

tpp::TypePtr tpp::ArrayValue::GetType() { return Type::GetArray(); }

bool tpp::ArrayValue::GetBool() { return !Content.empty(); }

tpp::ObjectValue::ObjectValue(const std::map<std::string, ValPtr> &content) : Content(content) {}

tpp::TypePtr tpp::ObjectValue::GetType() { return Type::GetObject(); }

bool tpp::ObjectValue::GetBool() { return !Content.empty(); }

template <>
tpp::ValPtr tpp::CreateValue<double>(const double &content)
{
	return std::make_shared<NumberValue>(content);
}

template <>
tpp::ValPtr tpp::CreateValue<char>(const char &content)
{
	return std::make_shared<CharValue>(content);
}

template <>
tpp::ValPtr tpp::CreateValue<std::string>(const std::string &content)
{
	return std::make_shared<StringValue>(content);
}
