#include "runtime/function/global/global_context.h"

#include "runtime/core/base/string_utils.h"

#include "runtime/platform/file_system/vfs.h"

#include "runtime/engine.h"

using namespace Piccolo;
using namespace std;

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "PiccoloEditor.ini";

    Piccolo::PiccoloEngine* engine = new Piccolo::PiccoloEngine();

    engine->startEngine(config_file_path.generic_string());
    engine->initialize();

    engine->run();

    engine->clear();
    engine->shutdownEngine();

    return 0;
}
