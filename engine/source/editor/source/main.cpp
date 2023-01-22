#include "runtime/function/global/global_context.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/platform/file_system/vfs.h"
// #include "runtime/platform/path/path.h"

#include "runtime/core/math/matrix2.h"
#include "runtime/core/math/matrix3.h"
#include "runtime/core/math/matrix4.h"

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

    // {
    //     FilePtr    file = g_runtime_global_context.m_vfs->open("asset/world/test01.world.json", File::read_text);
    //     std::string buffer;
    //     file->read(buffer);
    //     LOG_DEBUG("{}", buffer);
    // }

    // {
    //     FilePtr    file = g_runtime_global_context.m_vfs->open("asset/level/test-02.level.json", File::read_text);
    //     std::string buffer;
    //     file->read(buffer);
    //     LOG_DEBUG("{}", buffer);
    // }

    // Matrix2x2 mat2;
    // g_runtime_global_context.m_asset_manager->loadVFSAsset<Matrix2x2>("asset/test/mat2.json", mat2);
    // Matrix3x3 mat3;
    // g_runtime_global_context.m_asset_manager->loadVFSAsset<Matrix3x3>("asset/test/mat3.json", mat3);
    // Piccolo::Matrix4x4 mat4;
    // g_runtime_global_context.m_asset_manager->loadVFSAsset<Piccolo::Matrix4x4>("asset/test/mat4.json", mat4);

    engine->run();

    engine->clear();
    engine->shutdownEngine();

    return 0;
}
