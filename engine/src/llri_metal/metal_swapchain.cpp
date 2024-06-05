#include "metal_swapchain.h"
#include <chrono>
#include "../mercury_imgui.h"
#include "../engine.h"

using namespace mercury;

#ifdef MERCURY_GRAPHICS_API_METAL
MTK::View *gView = nullptr;
int gNumFrames = 3;
u64 gCurrentFrameIndex = 0;
u64 gFrameID = 0;

bool llri::swapchain::create(void* nativeWindowHandle)
{
    gView = static_cast<MTK::View *>(nativeWindowHandle);
    mercury::write_log_message("Get MTK VIEW: %s",gView->debugDescription()->cString(NS::StringEncoding::UTF8StringEncoding));

	return true;
}

bool llri::swapchain::destroy()
{

	return true;
}

void llri::swapchain::setBackGroundColor(float r, float g, float b, float a)
{
    gView->setClearColor(MTL::ClearColor(r,g,b,a));
}


bool llri::swapchain::update()
{
	gCurrentFrameIndex = gFrameID % gNumFrames;


    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //get the current command buffer object to encode commands for execution in the GPU
    auto* commandBuffer = gCommandQueue->commandBuffer();
    //get the current render pass descriptor that will be populated with different render targets and their information
    auto* renderPassDescriptor = gView->currentRenderPassDescriptor();
    //encodes the renderPass descriptor into actually commands
    auto* renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
    //YOU SHALL NOT ENCODE ANYMORE - end encoding
    renderCommandEncoder->endEncoding();
    //tell gpu we got something to draw
    commandBuffer->presentDrawable(gView->currentDrawable());
    //this ain't a marriage, commit to the damn draw
    commandBuffer->commit();

    pool->release();

	gFrameID++;
	return true;
}
#endif