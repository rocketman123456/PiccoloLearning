#include "runtime/function/global/global_context.h"

#include "runtime/core/base/string_utils.h"

#include "runtime/platform/file_system/vfs.h"

#include "editor/include/vk_app.h"

using namespace Piccolo;
using namespace std;

int main(int argc, char** argv)
{
    auto result = volkInitialize();
    if(result != VK_SUCCESS)
    {
        printf("failed to load volk\n");
        return -1;
    }

    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
