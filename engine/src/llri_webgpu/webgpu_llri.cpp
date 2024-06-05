#include "webgpu_llri.h"
#ifdef MERCURY_GRAPHICS_API_WEBGPU
#include "../application.h"
#include "../platform.h"

#include "../swapchain.h"
#include "../context.h"
#include "../engine.h"
#include "../mercury_imgui.h"

#include <iostream>
using namespace mercury;

WGPUDevice gDevice = nullptr;
WGPUAdapter gAdapter = nullptr;
WGPUInstance gInstance = nullptr;
WGPUQueue gQueue = nullptr;
WGPUSurface gSurface = nullptr;
WGPUSwapChain  gSwapChain = nullptr;
WGPUTextureFormat gPrefferedBackbufferFormat = WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm;

void RequestDeviceCallback(WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * userdata)
{
    mercury::write_log_message("device requested: %s", message);
    bool succeed = status == WGPURequestDeviceStatus::WGPURequestDeviceStatus_Success;

    mercury::write_log_message("device status: %s ptr:%p", succeed ? "success" : "fail", device);

    if(succeed)
    {
        gDevice = device;
        mercury::write_log_message("getting queue");
        gQueue = wgpuDeviceGetQueue(device);

        mercury::write_log_message("create swapchain");
        WGPUSurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
        canvasDesc.selector = "canvas";
        canvasDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;

        WGPUSurfaceDescriptor surfDesc{};
        surfDesc.nextInChain = &(canvasDesc.chain);
        gSurface = wgpuInstanceCreateSurface(gInstance,&surfDesc);
        mercury::write_log_message("surface created: %p", gSurface);

        gPrefferedBackbufferFormat = wgpuSurfaceGetPreferredFormat(gSurface,gAdapter);
        mercury::write_log_message("gPrefferedBackbufferFormat: %d", gPrefferedBackbufferFormat);

        WGPUSwapChainDescriptor scDesc{};
        scDesc.usage = WGPUTextureUsage::WGPUTextureUsage_RenderAttachment;
        scDesc.format = gPrefferedBackbufferFormat;
        scDesc.width = 1280;
        scDesc.height = 720;
        scDesc.presentMode = WGPUPresentMode::WGPUPresentMode_Fifo;

        gSwapChain = wgpuDeviceCreateSwapChain(device,gSurface, &scDesc);

        mercury::write_log_message("swapchain created: %p", gSwapChain);

        imgui::initialize();
    }
}

void RequestAdapterCallback(WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * userdata)
{
    mercury::write_log_message("adapter requested: %s", message);
    bool succeed = status == WGPURequestAdapterStatus::WGPURequestAdapterStatus_Success;

    mercury::write_log_message("adapter status: %s ptr:%p", succeed ? "success" : "fail", adapter);
    if(succeed)
    {
        gAdapter = adapter;
        WGPUAdapterProperties props = {};
        wgpuAdapterGetProperties(adapter,&props);

        mercury::write_log_message("adapter name: %s",props.name);
        mercury::write_log_message("adapter compatibility : %d",props.compatibilityMode);
        mercury::write_log_message("adapter architecture: %s",props.architecture);
        mercury::write_log_message("adapter description: %s",props.driverDescription);
        mercury::write_log_message("adapter vendor: %s",props.vendorName);
        mercury::write_log_message("adapter backend: %d",props.backendType);
        mercury::write_log_message("adapter type: %d",props.adapterType);

        WGPUSupportedLimits limits;
        wgpuAdapterGetLimits(adapter, &limits);

        mercury::write_log_message("max tex2D: %d",limits.limits.maxTextureDimension2D);

        WGPUDeviceDescriptor desc = {};

        wgpuAdapterRequestDevice(adapter, &desc, RequestDeviceCallback, userdata);
    }
}

bool llri::initialize()
{
    mercury::write_log_message("Initialize WebGPU llri");

    WGPUInstanceDescriptor desc = {};
    gInstance = wgpuCreateInstance(nullptr);

    mercury::write_log_message("instance created: %p", gInstance);

    WGPURequestAdapterOptions options = {};
    options.powerPreference = WGPUPowerPreference_HighPerformance;
    options.compatibilityMode = false;
    options.backendType = WGPUBackendType_WebGPU;
    mercury::write_log_message("before adapter request");
    wgpuInstanceRequestAdapter(gInstance,&options,RequestAdapterCallback,nullptr);
    mercury::write_log_message("after adapter request");

	return true;
}

void llri::shutdown()
{

    //wgpuDeviceCreateCommandEncoder(device,)
}

WGPUColor clearColor = {0.2f, 0.4f, 0.7f, 1};

bool llri::update()
{
    if(gSwapChain != nullptr)
    {
        auto view = wgpuSwapChainGetCurrentTextureView(gSwapChain);

        WGPURenderPassColorAttachment attachment{};
        attachment.view = view;
        attachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
        attachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
        attachment.clearValue = clearColor;
        attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        WGPURenderPassDescriptor renderpass{};
        renderpass.colorAttachmentCount = 1;
        renderpass.colorAttachments = &attachment;


        WGPUCommandBuffer commands;

        {
            WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(gDevice, nullptr);
            {
                WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &renderpass);

                llri::context ctx{ static_cast<void*>(pass) };
                engine::renderCallback(ctx);

                wgpuRenderPassEncoderEnd(pass);
            }
            commands = wgpuCommandEncoderFinish(encoder,nullptr);
        }

        wgpuQueueSubmit(gQueue,1,&commands);
    }

	return true;
}

llri::CommandList llri::BeginOneTimeSubmitCommandList()
{
	void* cmdListPtr = nullptr;

	llri::CommandList result = { cmdListPtr };
	return result;
}

void llri::EndOneTimeSubmitCommandList(llri::CommandList clist)
{

}

bool llri::swapchain::create(void* nativeWindowHandle)
{
    return true;
}

bool llri::swapchain::destroy()
{
    return true;
}

void llri::swapchain::setBackGroundColor(float r, float g, float b, float a)
{
    clearColor.r = r;
    clearColor.g = g;
    clearColor.b = b;
    clearColor.a = a;
}

bool llri::swapchain::update()
{
    llri::update();
    return true;
}

#endif
