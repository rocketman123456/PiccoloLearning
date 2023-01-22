#include "runtime/function/render/render_system.h"

#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"
// #include "runtime/function/render/debugdraw/debug_draw_manager.h"
// #include "runtime/function/render/render_camera.h"
// #include "runtime/function/render/render_pass.h"
// #include "runtime/function/render/render_pipeline.h"
// #include "runtime/function/render/render_resource.h"
// #include "runtime/function/render/render_resource_base.h"
// #include "runtime/function/render/render_scene.h"
#include "runtime/function/render/window_system.h"

// #include "runtime/function/render/passes/main_camera_pass.h"
// #include "runtime/function/render/passes/particle_pass.h"

#include "runtime/function/render/interface/vulkan/vulkan_rhi.h"

namespace Piccolo
{
    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {
        std::shared_ptr<ConfigManager> config_manager = g_runtime_global_context.m_config_manager;
        ASSERT(config_manager);
        std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
        ASSERT(asset_manager);

        // render context initialize
        // RHIInitInfo rhi_init_info;
        // rhi_init_info.window_system = init_info.window_system;

        // m_rhi = std::make_shared<VulkanRHI>();
        // m_rhi->initialize(rhi_init_info);
    }

    void RenderSystem::tick(float delta_time)
    {
        // // process swap data between logic and render contexts
        // processSwapData();

        // // prepare render command context
        // m_rhi->prepareContext();

        // // update per-frame buffer
        // m_render_resource->updatePerFrameBuffer(m_render_scene, m_render_camera);

        // // update per-frame visible objects
        // m_render_scene->updateVisibleObjects(std::static_pointer_cast<RenderResource>(m_render_resource), m_render_camera);

        // // prepare pipeline's render passes data
        // m_render_pipeline->preparePassData(m_render_resource);

        // g_runtime_global_context.m_debugdraw_manager->tick(delta_time);

        // // render one frame
        // if (m_render_pipeline_type == RENDER_PIPELINE_TYPE::FORWARD_PIPELINE)
        // {
        //     m_render_pipeline->forwardRender(m_rhi, m_render_resource);
        // }
        // else if (m_render_pipeline_type == RENDER_PIPELINE_TYPE::DEFERRED_PIPELINE)
        // {
        //     m_render_pipeline->deferredRender(m_rhi, m_render_resource);
        // }
        // else
        // {
        //     LOG_ERROR(__FUNCTION__, "unsupported render pipeline type");
        // }
    }

    void RenderSystem::clear()
    {
        // if (m_rhi)
        // {
        //     m_rhi->clear();
        // }
        // m_rhi.reset();

        // if (m_render_scene)
        // {
        //     m_render_scene->clear();
        // }
        // m_render_scene.reset();

        // if (m_render_resource)
        // {
        //     m_render_resource->clear();
        // }
        // m_render_resource.reset();

        // if (m_render_pipeline)
        // {
        //     m_render_pipeline->clear();
        // }
        // m_render_pipeline.reset();
    }
}
