#include "runtime/function/global/global_context.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/memory/buddy_allocator.h"
#include "runtime/core/memory/cache_aligned_allocator.h"
#include "runtime/core/memory/free_list_allocator.h"
#include "runtime/core/memory/free_tree_allocator.h"
#include "runtime/core/memory/huge_page_allocator.h"
#include "runtime/core/memory/linear_allocator.h"
#include "runtime/core/memory/pool_allocator.h"
#include "runtime/core/memory/simple_allocator.h"
#include "runtime/core/memory/stack_allocator.h"

#include "runtime/platform/file_system/vfs.h"
#include "runtime/platform/path/path.h"

#include "runtime/engine.h"

#include <iostream>

using namespace Piccolo;
using namespace std;

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "PiccoloEditor.ini";

    Piccolo::PiccoloEngine* engine = new Piccolo::PiccoloEngine();

    engine->startEngine(config_file_path.generic_string());
    engine->initialize();

    auto relative = Path::getRelativePath("asset-test", "asset-test/world/test01.world.json");
    std::cout << relative << std::endl;

    auto path = g_runtime_global_context.m_config_manager->getRootFolder();

    {
        // auto fs = std::make_shared<NativeFileSystem>("asset", path.string() + "/asset-test");
        // fs->buildFSCache();

        // auto file = fs->open("asset/world/test01.world.json", File::read_bin);

        // FileBuffer buffer;
        // file->read(buffer);
        // file->close();
        // std::cout << (char*)buffer.data() << std::endl;
    }

    {
        auto fs = std::make_shared<ZipFileSystem>("asset", path.string() + "/asset-test.zip");
        fs->buildFSCache();

        auto file = fs->open("asset/asset-test/world/test01.world.json", File::read_bin);

        FileBuffer buffer;
        file->read(buffer);
        file->close();
        std::cout << (char*)buffer.data() << std::endl;
    }

    // engine->run();

    engine->clear();
    engine->shutdownEngine();

    return 0;
}
