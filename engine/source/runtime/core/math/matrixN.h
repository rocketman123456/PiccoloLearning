#pragma once
#include "runtime/core/math/vectorN.h"

namespace Piccolo
{
    REFLECTION_TYPE(MatrixN)
    CLASS(MatrixN, Fields)
    {
        REFLECTION_BODY(MatrixN);
    
    public:
        std::vector<VectorN> m_rows {};

    };
}
