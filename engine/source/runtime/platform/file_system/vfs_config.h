#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <vector>
#include <string>

namespace Piccolo
{
    // stroe mount points
    REFLECTION_TYPE(VFSConfig)
    CLASS(VFSConfig, Fields)
    {
        REFLECTION_BODY(VFSConfig)
    public:
        std::vector<std::string> m_vpath;
        std::vector<std::string> m_rpath;
    };
}
