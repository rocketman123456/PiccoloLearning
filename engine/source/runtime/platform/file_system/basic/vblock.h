#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Piccolo
{
    class VNode;
    class FileSystem;
    // Store File System Info, Just like linux block and inode, indicate dir
    // Should Use in VirtualFileSystem
    struct VBlock
    {
        using SubBlockMap = std::unordered_map<std::string, std::shared_ptr<VBlock>>;
        using NodeMap     = std::unordered_map<std::string, std::shared_ptr<VNode>>;

        // main fs
        std::shared_ptr<FileSystem> file_system = nullptr; // if file_system == null, should look to parent block

        std::shared_ptr<VBlock> parent = nullptr; // if parent == nullptr, means that this is root dir
        std::string             name   = {};      // current dir name
        std::filesystem::path   path   = {};      // current dir full path
        std::filesystem::path   rpath  = {};      // current dir real path

        // Store data
        SubBlockMap sub_blocks = {};
        NodeMap     sub_nodes  = {};
        uint32_t    crc        = 0;
    };

    using VBlockPtr = std::shared_ptr<VBlock>;
} // namespace Piccolo