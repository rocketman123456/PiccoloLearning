#include "runtime/platform/file_system/basic/fs_utils.h"
#include "runtime/platform/file_system/basic/file_system.h"

#include "runtime/core/base/crc.h"
#include "runtime/core/base/macro.h"
#include "runtime/core/base/string_utils.h"

#include <cassert>

namespace Piccolo
{
    std::string preprocessFilePath(const std::string& path) { return ""; }

    VBlockPtr FSUtils::createVirtualBlock(VBlockPtr& parent, const std::vector<std::string>& dirs, int32_t level)
    {
        if (parent == nullptr)
            return nullptr;
        if (dirs.size() == 0 || level == dirs.size())
            return parent;

        auto found = parent->sub_blocks.find(dirs[level]);
        if (found == parent->sub_blocks.end())
        {
            // create middle vblock in path
            VBlockPtr block    = std::make_shared<VBlock>();
            block->parent      = parent;
            block->name        = dirs[level];
            block->path        = parent->path.string() + "/" + block->name;
            block->rpath       = parent->rpath.string() + "/" + block->name;
            block->file_system = parent->file_system;
            block->crc         = g_crc32.calculate((const char*)block->path.c_str());

            block->file_system->m_changed   = true;
            parent->sub_blocks[block->name] = block;

            LOG_DEBUG("Block Path: {}", block->path.string());
            return createVirtualBlock(block, dirs, level + 1);
        }
        else
        {
            return createVirtualBlock(found->second, dirs, level + 1);
        }
    }

    VBlockPtr FSUtils::createVirtualBlock(FSPtr root, const std::filesystem::path& path)
    {
        if (root == nullptr)
            return nullptr;

        std::string dir = replace_all(path.string(), "\\", "/");

        VBlockPtr root_block = root->getRoot();
        if (root_block->path == dir)
        {
            return root_block;
        }

        std::vector<std::string> dir_stack;
        split_single_char(dir, '/', dir_stack);

        VBlockPtr final_block = createVirtualBlock(root_block, dir_stack, 0);
        return final_block;
    }

    VNodePtr FSUtils::createVirtualNode(FSPtr root, const std::filesystem::path& path)
    {
        if (root == nullptr)
            return nullptr;

        std::string              vpath = replace_all(path.string(), "\\", "/");
        std::vector<std::string> dir_stack;
        split_single_char(vpath, '/', dir_stack);

        if (dir_stack.size() == 0)
            return nullptr;

        std::string file_name = dir_stack.back();
        std::string file_dir  = "";
        if (dir_stack.size() >= 2)
        {
            for (int i = 0; i < dir_stack.size() - 2; ++i)
            {
                file_dir += dir_stack[i] + "/";
            }
            file_dir += dir_stack[dir_stack.size() - 2];
        }

        VBlockPtr block = createVirtualBlock(root, file_dir);
        if (block == nullptr)
        {
            return nullptr;
        }
        else
        {
            auto found = block->sub_nodes.find(file_name);
            if (found == block->sub_nodes.end())
            {
                // Create Virtual Node
                auto node   = std::make_shared<VNode>();
                node->name  = file_name;
                node->path  = block->path.string() + "/" + file_name;
                node->rpath = block->rpath.string() + "/" + file_name;
                node->block = block;
                node->crc   = g_crc32.calculate((const char*)node->path.c_str());

                block->file_system->m_changed = true;
                block->sub_nodes[file_name]   = node;

                LOG_DEBUG("Node Path: {}", node->path.string());
                return node;
            }
            else
            {
                return found->second;
            }
        }
    }

    VBlockPtr FSUtils::findVirtualBlock(const VBlockPtr& root, const std::vector<std::string>& dirs, int32_t level)
    {
        // check invalid status
        if (dirs.size() == 0)
            return root;
        if (root == nullptr)
            return nullptr;
        if (level == dirs.size())
            return root;
        // find if dir exist
        auto found = root->sub_blocks.find(dirs[level]);
        // if not exist, return null
        if (found == root->sub_blocks.end())
        {
            return nullptr;
        }
        // check return or go deeper
        VBlockPtr sub_block = found->second;
        return findVirtualBlock(sub_block, dirs, level + 1);
    }

    VBlockPtr FSUtils::findVirtualBlock(FSPtr root, const std::filesystem::path& path)
    {
        if (root == nullptr)
            return nullptr;
        auto                     dir = replace_all(path.string(), "\\", "/");
        std::vector<std::string> dir_stack;
        split_single_char(dir, '/', dir_stack);
        auto block = findVirtualBlock(root->getRoot(), dir_stack, 0);
        return block;
    }

    VBlockPtr FSUtils::findDeepestExistVirtualBlock(const VBlockPtr& root, const std::vector<std::string>& dirs, int32_t level)
    {
        // check invalid status
        if (dirs.size() == 0)
            return root;
        if (root == nullptr)
            return nullptr;
        if (level == dirs.size())
            return root;
        // find if dir exist
        auto found = root->sub_blocks.find(dirs[level]);
        // we reach deepest dir
        if (found == root->sub_blocks.end())
        {
            return root;
        }
        // check return or go deeper
        VBlockPtr sub_block = found->second;
        return findDeepestExistVirtualBlock(sub_block, dirs, level + 1);
    }

    VBlockPtr FSUtils::findDeepestExistVirtualBlock(FSPtr root, const std::filesystem::path& path)
    {
        auto                     dir = replace_all(path.string(), "\\", "/");
        std::vector<std::string> dir_stack;
        split_single_char(dir, '/', dir_stack);
        auto block = findDeepestExistVirtualBlock(root->getRoot(), dir_stack, 0);
        return block;
    }

    VNodePtr FSUtils::findVirtualNode(const VBlockPtr& root, const std::vector<std::string>& dirs, const std::string& name)
    {
        auto block = findVirtualBlock(root, dirs, 0);
        if (block == nullptr)
            return nullptr;
        // find target file
        auto found = block->sub_nodes.find(name);
        if (found == block->sub_nodes.end())
            return nullptr;
        else
            return found->second;
    }

    VNodePtr FSUtils::findVirtualNode(FSPtr root, const std::filesystem::path& path, const std::string& name)
    {
        auto                     dir = replace_all(path.string(), "\\", "/");
        std::vector<std::string> dir_stack;
        split_single_char(dir, '/', dir_stack);
        return findVirtualNode(root->getRoot(), dir_stack, name);
    }

    VNodePtr FSUtils::findVirtualNode(FSPtr root, const std::filesystem::path& file_path)
    {
        auto        dir = replace_all(file_path.string(), "\\", "/");
        std::string dir;
        std::string file_name;
        split_last_single_char(dir, '/', dir, file_name);
        return findVirtualNode(root, dir, file_name);
    }
} // namespace Piccolo