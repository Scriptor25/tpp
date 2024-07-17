#include <TPP/Value.hpp>
#include <stdexcept>

tpp::ValueBase::~ValueBase() = default;

bool tpp::ValueBase::GetBool()
{
    throw std::runtime_error("not implemented");
}

double tpp::ValueBase::GetDouble()
{
    throw std::runtime_error("not implemented");
}

tpp::Value<double>::Value(double content)
    : Content(content)
{
}

bool tpp::Value<double>::GetBool()
{
    return Content != 0.0;
}

double tpp::Value<double>::GetDouble()
{
    return Content;
}
