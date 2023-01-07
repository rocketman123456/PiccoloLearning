#pragma once
#include <filesystem>
#include <memory>
#include <string>

namespace Piccolo
{
    class VBlock;
    class FileSystem;
    // Contain Basic and Additional File Infos, indicate file
    // Can be modified if needed
    struct VNode
    {
        // File Full Name will be path + name
        std::string                 name {}; // file name
        std::filesystem::path       path {}; // file basic path in vfs
        std::filesystem::path       rpath {};
        std::shared_ptr<VBlock>     block {nullptr};
        std::shared_ptr<FileSystem> file_system {nullptr};
        uint32_t                    crc = 0;
    };

    using VNodePtr = std::shared_ptr<VNode>;
} // namespace Piccolo