#include "runtime/function/global/global_context.h"

#include "runtime/core/base/string_utils.h"

#include "runtime/platform/file_system/basic/file_system.h"
#include "runtime/platform/file_system/basic/fs_utils.h"

#include <iostream>
#include <string>
#include <filesystem>

using namespace Piccolo;
using namespace std;

// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#define PICCOLO_XSTR(s) PICCOLO_STR(s)
#define PICCOLO_STR(s) #s

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "PiccoloEditor.ini";

    g_runtime_global_context.startSystems(config_file_path);

    FSPtr fs = std::make_shared<FileSystem>("", executable_path.parent_path());

    return 0;
}
