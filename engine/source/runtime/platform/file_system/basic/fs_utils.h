#pragma once
#include "runtime/platform/file_system/basic/file_system.h"
#include "runtime/platform/file_system/basic/vblock.h"
#include "runtime/platform/file_system/basic/vnode.h"

#include <filesystem>
#include <string>

namespace Piccolo
{
    class FSUtils
    {
    public:
        static VBlockPtr createVirtualBlock(FSPtr root, const std::filesystem::path& path);
        static VBlockPtr findVirtualBlock(FSPtr root, const std::filesystem::path& dir);
        static VBlockPtr findDeepestExistVirtualBlock(FSPtr root, const std::filesystem::path& dir);

        static VNodePtr createVirtualNode(FSPtr root, const std::filesystem::path& dir);
        static VNodePtr findVirtualNode(FSPtr root, const std::filesystem::path& dir, const std::string& name);
        static VNodePtr findVirtualNode(FSPtr root, const std::filesystem::path& file_path);

    private:
        static VBlockPtr createVirtualBlock(VBlockPtr& parent, const std::vector<std::string>& dirs, int32_t level);
        static VBlockPtr findDeepestExistVirtualBlock(const VBlockPtr& root, const std::vector<std::string>& dirs, int32_t level);
        static VNodePtr  findVirtualNode(const VBlockPtr& root, const std::vector<std::string>& dirs, const std::string& name);
    };
} // namespace Piccolo
