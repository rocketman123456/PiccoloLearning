#include "runtime/platform/file_system/basic/file_utils.h"

#include "runtime/core/string/string_utils.h"

namespace Piccolo
{
    std::string getNormalizedPath(const std::string& path)
    {
        // replace '\\'
        auto temp = replace_all(path, "\\", "/");
        // remove front '/'
        if (temp.size() > 0 && temp.at(0) == '/')
        {
            temp = temp.substr(1, temp.size() - 1);
        }
        return temp;
    }
} // namespace Piccolo
