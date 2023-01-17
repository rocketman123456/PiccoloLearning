#pragma once
#include "runtime/core/math/math.h"
#include "runtime/core/math/vector2.h"

#include <vector>

namespace Piccolo
{
    // TODO : make reflection support [] array
    REFLECTION_TYPE(Matrix2x2)
    CLASS(Matrix2x2, Fields)
    {
        REFLECTION_BODY(Matrix2x2);

    public:
        std::vector<Vector2> m_rows;

        Matrix2x2() { operator=(IDENTITY); }

        explicit Matrix2x2(float arr[2][2])
        {
            m_rows.resize(2);
            memcpy(m_rows[0].ptr(), arr[0], 2 * sizeof(float));
            memcpy(m_rows[1].ptr(), arr[1], 2 * sizeof(float));
        }

        Matrix2x2(float (&float_array)[4])
        {
            m_rows.resize(2);
            m_rows[0][0] = float_array[0];
            m_rows[0][1] = float_array[1];
            m_rows[1][0] = float_array[2];
            m_rows[1][1] = float_array[3];
        }

        Matrix2x2(float entry00, float entry01, float entry10, float entry11)
        {
            m_rows.resize(2);
            m_rows[0][0] = entry00;
            m_rows[0][1] = entry01;
            m_rows[1][0] = entry10;
            m_rows[1][1] = entry11;
        }

        Matrix2x2(const Vector2& row0, const Vector2& row1)
        {
            m_rows.resize(2);
            m_rows[0] = row0;
            m_rows[1] = row1;
        }

        Matrix2x2(const Matrix2x2& rhs)
        {
            m_rows.resize(2);
            m_rows[0] = rhs.m_rows[0];
            m_rows[1] = rhs.m_rows[1];
        }

        Matrix2x2& operator=(const Matrix2x2& rhs)
        {
            m_rows[0] = rhs.m_rows[0];
            m_rows[1] = rhs.m_rows[1];
            return *this;
        }

        const Matrix2x2& operator*=(const float rhs)
        {
            m_rows[0] *= rhs;
            m_rows[1] *= rhs;
            return *this;
        }

        const Matrix2x2& operator+=(const Matrix2x2& rhs)
        {
            m_rows[0] += rhs.m_rows[0];
            m_rows[1] += rhs.m_rows[1];
            return *this;
        }

        static const Matrix2x2 ZERO;
        static const Matrix2x2 IDENTITY;
    };
} // namespace Piccolo