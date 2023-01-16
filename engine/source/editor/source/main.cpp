#include "runtime/function/global/global_context.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/platform/file_system/vfs.h"
// #include "runtime/platform/path/path.h"

// #include "runtime/platform/file_system/memory_file/memory_file.h"
// #include "runtime/platform/file_system/memory_file/memory_file_system.h"
// #include "runtime/platform/file_system/native_file/native_file.h"
// #include "runtime/platform/file_system/native_file/native_file_system.h"
// #include "runtime/platform/file_system/zip_file/zip_file.h"
// #include "runtime/platform/file_system/zip_file/zip_file_system.h"

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

    VFSConfig config;
    g_runtime_global_context.m_asset_manager->loadAsset<VFSConfig>("config/config.vfs.json", config);

    g_vfs.unmountAll();
    g_vfs.mount(config);

    {
        FilePtr    file = g_vfs.open("asset/world/test01.world.json", File::read_text);
        std::string buffer;
        file->read(buffer);
        LOG_DEBUG("{}", buffer);
    }

    {
        FilePtr    file = g_vfs.open("asset/level/test-02.level.json", File::read_text);
        std::string buffer;
        file->read(buffer);
        LOG_DEBUG("{}", buffer);
    }

    engine->run();

    engine->clear();
    engine->shutdownEngine();

    return 0;
}
