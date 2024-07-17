#pragma once

#include <TPP/TPP.hpp>

namespace tpp
{
    struct ValueBase
    {
        virtual ~ValueBase();

        virtual bool GetBool();
        virtual double GetDouble();
    };

    template <typename T>
    struct Value : ValueBase
    {
        Value(const T &content)
            : Content(content)
        {
        }

        T Content;
    };

    template <>
    struct Value<double> : ValueBase
    {
        Value(double content);

        bool GetBool() override;
        double GetDouble() override;

        double Content;
    };

    template <typename T>
    std::shared_ptr<Value<T>>
    CreateValue(const T &content)
    {
        return std::make_shared<Value<T>>(content);
    }
}
