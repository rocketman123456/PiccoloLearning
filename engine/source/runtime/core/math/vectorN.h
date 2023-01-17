#pragma once
#include "runtime/core/math/math.h"
#include "runtime/core/meta/reflection/reflection.h"

#include <cassert>
#include <cstdlib>
#include <vector>

namespace Piccolo
{
    REFLECTION_TYPE(VectorN)
    CLASS(VectorN, Fields)
    {
        REFLECTION_BODY(VectorN);

    public:
        VectorN() : data {} {}

        VectorN(int N) { data.resize(N); }

        VectorN(const VectorN& rhs)
        {
            data.resize(rhs.data.size());
            std::memcpy(data.data(), rhs.data.data(), sizeof(float) * data.size());
        }

        VectorN& operator=(const VectorN& rhs)
        {
            data.clear();
            data.resize(rhs.data.size());
            std::memcpy(data.data(), rhs.data.data(), sizeof(float) * data.size());
            return *this;
        }

        ~VectorN() = default;

        float operator[](const int idx) const { return data[idx]; }

        float& operator[](const int idx) { return data[idx]; }

        const VectorN& operator*=(float rhs)
        {
            for (int i = 0; i < data.size(); i++)
            {
                data[i] *= rhs;
            }
            return *this;
        }

        VectorN operator*(float rhs) const
        {
            VectorN tmp = *this;
            tmp *= rhs;
            return tmp;
        }

        VectorN operator+(const VectorN& rhs) const
        {
            VectorN tmp = *this;
            for (int i = 0; i < tmp.data.size(); i++)
            {
                tmp.data[i] += rhs.data[i];
            }
            return tmp;
        }

        VectorN operator-(const VectorN& rhs) const
        {
            VectorN tmp = *this;
            for (int i = 0; i < tmp.data.size(); i++)
            {
                tmp.data[i] -= rhs.data[i];
            }
            return tmp;
        }

        const VectorN& operator+=(const VectorN& rhs)
        {
            for (int i = 0; i < data.size(); i++)
            {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        const VectorN& operator-=(const VectorN& rhs)
        {
            for (int i = 0; i < data.size(); i++)
            {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        float dotProduct(const VectorN& rhs) const
        {
            float sum = 0;
            for (int i = 0; i < data.size(); i++)
            {
                sum += data[i] * rhs.data[i];
            }
            return sum;
        }

        void zero()
        {
            for (int i = 0; i < data.size(); i++)
            {
                data[i] = 0.0f;
            }
        }

        bool isValid() const
        {
            for (int i = 0; i < data.size(); ++i)
            {
                if (data[i] * 0.0f != data[i] * 0.0f)
                {
                    return false;
                }
            }
            return true;
        }

        bool isNaN() const
        {
            for (auto x : data)
            {
                if (Math::isNan(x))
                    return true;
            }
            return false;
        }

    public:
        std::vector<float> data;
    };
} // namespace Piccolo
