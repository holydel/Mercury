#pragma once

#include "mercury_api.h"
#ifdef MERCURY_XR_API_OPENXR
#include "../platform.h"

#define XR_NO_PROTOTYPES 1

#ifdef MERCURY_PLATFORM_WINDOWS
#define XR_USE_PLATFORM_WIN32 1
#include <Windows.h>
#endif
#ifdef MERCURY_PLATFORM_ANDROID
#define XR_USE_PLATFORM_ANDROID 1
#include <android/native_activity.h>
#endif
#ifdef MERCURY_PLATFORM_LINUX
#define XR_USE_PLATFORM_XCB 1
#endif
#ifdef MERCURY_PLATFORM_MACOS
#define VK_USE_PLATFORM_METAL_EXT 1
#endif
#define XR_ENABLE_BETA_EXTENSIONS 1

#ifdef MERCURY_GRAPHICS_API_VULKAN
#define XR_USE_GRAPHICS_API_VULKAN 1
#include <vulkan/vulkan.h>
#endif

#ifdef MERCURY_GRAPHICS_API_D3D12
#define XR_USE_GRAPHICS_API_D3D12 1
#include <directx/d3d12.h>
#endif

#include <openxr/openxr_platform.h>
#include <openxr/openxr_loader_negotiation.h>

void LoadOpenXR_Library();
void LoadXRGlobalLevelFuncs();
void LoadXRInstanceLevelFuncs();
void ShutdownOpenXR_Library();

extern XrSystemId gXRSystemID;
extern XrSession gXRSession;

const char* MercuryXrResultToString(XrResult res);

#define XR_CALL(func) {XrResult res = func; if(XR_FAILED(res)){ mercury::platform::fatalFail(MercuryXrResultToString(res));}}

#define XR_DECLARE_FUNCTION( fun ) extern PFN_##fun fun;

struct OpenXRLayer
{
	PFN_xrGetInstanceProcAddr xrGetInstanceProcAddr;
	PFN_xrNegotiateLoaderApiLayerInterface xrNegotiateLoaderApiLayerInterface;
	PFN_xrCreateApiLayerInstance xrCreateApiLayerInstance;
	mercury::OpenXRLayerInfo info;
	void* libHandle = nullptr;
};

extern std::vector<OpenXRLayer> gImplicitLayers;
extern std::vector<OpenXRLayer> gExplicitLayers;

extern XrInstance gXRInstance;
//
XR_DECLARE_FUNCTION(xrNegotiateLoaderRuntimeInterface);
XR_DECLARE_FUNCTION(xrGetInstanceProcAddr);

XR_DECLARE_FUNCTION(xrInitializeLoaderKHR);
XR_DECLARE_FUNCTION(xrEnumerateApiLayerProperties);
XR_DECLARE_FUNCTION(xrEnumerateInstanceExtensionProperties);
XR_DECLARE_FUNCTION(xrCreateInstance);
//
XR_DECLARE_FUNCTION(xrDestroyInstance);
XR_DECLARE_FUNCTION(xrGetSystem);
XR_DECLARE_FUNCTION(xrGetSystemProperties);
XR_DECLARE_FUNCTION(xrResultToString);
XR_DECLARE_FUNCTION(xrGetInstanceProperties);
XR_DECLARE_FUNCTION(xrEnumerateViewConfigurations);
XR_DECLARE_FUNCTION(xrGetViewConfigurationProperties);
XR_DECLARE_FUNCTION(xrEnumerateViewConfigurationViews);
XR_DECLARE_FUNCTION(xrBeginSession);
XR_DECLARE_FUNCTION(xrWaitFrame);
XR_DECLARE_FUNCTION(xrBeginFrame);
XR_DECLARE_FUNCTION(xrLocateViews);
XR_DECLARE_FUNCTION(xrEndFrame);
XR_DECLARE_FUNCTION(xrEndSession);
XR_DECLARE_FUNCTION(xrCreateSession);
XR_DECLARE_FUNCTION(xrPollEvent);
XR_DECLARE_FUNCTION(xrEnumerateSwapchainFormats);
XR_DECLARE_FUNCTION(xrCreateSwapchain);
XR_DECLARE_FUNCTION(xrEnumerateSwapchainImages);
XR_DECLARE_FUNCTION(xrAcquireSwapchainImage);
XR_DECLARE_FUNCTION(xrWaitSwapchainImage);
XR_DECLARE_FUNCTION(xrReleaseSwapchainImage);
XR_DECLARE_FUNCTION(xrCreateReferenceSpace);
XR_DECLARE_FUNCTION(xrEnumerateEnvironmentBlendModes);
XR_DECLARE_FUNCTION(xrEnumerateDisplayRefreshRatesFB);
XR_DECLARE_FUNCTION(xrGetDisplayRefreshRateFB);
XR_DECLARE_FUNCTION(xrRequestDisplayRefreshRateFB);
XR_DECLARE_FUNCTION(xrCreateActionSpace);
XR_DECLARE_FUNCTION(xrSyncActions);
XR_DECLARE_FUNCTION(xrStringToPath);
XR_DECLARE_FUNCTION(xrCreateActionSet);
XR_DECLARE_FUNCTION(xrCreateAction);
XR_DECLARE_FUNCTION(xrLocateSpace);
XR_DECLARE_FUNCTION(xrAttachSessionActionSets);
XR_DECLARE_FUNCTION(xrGetActionStatePose);
XR_DECLARE_FUNCTION(xrSuggestInteractionProfileBindings);
XR_DECLARE_FUNCTION(xrGetCurrentInteractionProfile);
XR_DECLARE_FUNCTION(xrGetActionStateBoolean);

XR_DECLARE_FUNCTION(xrGetVisibilityMaskKHR);
XR_DECLARE_FUNCTION(xrGetRenderModelPropertiesFB);
XR_DECLARE_FUNCTION(xrLoadRenderModelFB);
XR_DECLARE_FUNCTION(xrEnumerateRenderModelPathsFB);

XR_DECLARE_FUNCTION(xrResultToString);
XR_DECLARE_FUNCTION(xrCreateDebugUtilsMessengerEXT);
#ifdef HEXCELL_PLATFORM_ANDROID
XR_DECLARE_FUNCTION(xrCreateSwapchainAndroidSurfaceKHR);
#endif

#ifdef MERCURY_GRAPHICS_API_VULKAN
XR_DECLARE_FUNCTION(xrGetVulkanGraphicsRequirements2KHR);
XR_DECLARE_FUNCTION(xrCreateVulkanInstanceKHR);
XR_DECLARE_FUNCTION(xrGetVulkanGraphicsDevice2KHR);
XR_DECLARE_FUNCTION(xrCreateVulkanDeviceKHR);
XR_DECLARE_FUNCTION(xrGetVulkanGraphicsRequirements2KHR);
#endif

#ifdef MERCURY_GRAPHICS_API_D3D12
XR_DECLARE_FUNCTION(xrGetD3D12GraphicsRequirementsKHR);
#endif
template <typename T>
std::vector<T> EnumerateOpenXRObjects(XrResult(XRAPI_CALL* pfunc) (uint32_t initialCnt, uint32_t* cnt, T* objs), XrStructureType stype)
{
	uint32_t count = 0;
	pfunc(0, &count, nullptr);
	std::vector<T> result(count);
	for (T& val : result)
	{
		val.type = stype;
	}
	pfunc(count, &count, result.data());

	return result;
}

template <typename T>
std::vector<T> EnumerateOpenXRObjects(XrResult(XRAPI_CALL* pfunc) (const char* layerName, uint32_t initialCnt, uint32_t* cnt, T* objs), XrStructureType stype)
{
	uint32_t count = 0;
	pfunc(nullptr, 0, &count, nullptr);
	std::vector<T> result(count);
	for (T& val : result)
	{
		val.type = stype;
	}
	XrResult fres = pfunc(nullptr, count, &count, result.data());

	return result;
}

template <typename T, typename U, typename W>
std::vector<T> EnumerateOpenXRObjects(U instance, W system, XrResult(XRAPI_CALL* pfunc) (U instance, W device, uint32_t initialCnt, uint32_t* cnt, T* objs))
{
	uint32_t count = 0;
	pfunc(instance, system, 0, &count, nullptr);
	std::vector<T> result(count);
	pfunc(instance, system, count, &count, result.data());

	return result;
}

template <typename T, typename U>
std::vector<T> EnumerateOpenXRObjects(U instance, XrResult(XRAPI_CALL* pfunc) (U instance, uint32_t initialCnt, uint32_t* cnt, T* objs))
{
	uint32_t count = 0;
	pfunc(instance, 0, &count, nullptr);
	std::vector<T> result(count);
	pfunc(instance, count, &count, result.data());

	return result;
}

template <typename T, typename U, typename W, typename X>
std::vector<T> EnumerateOpenXRObjects(U instance, W system, X type, XrStructureType stype, XrResult(XRAPI_CALL* pfunc) (U instance, W device, X type, uint32_t initialCnt, uint32_t* cnt, T* objs))
{
	uint32_t count = 0;
	pfunc(instance, system, type, 0, &count, nullptr);
	std::vector<T> result(count);
	for (T& v : result)
	{
		v.type = stype;
	}
	pfunc(instance, system, type, count, &count, result.data());

	return result;
}
#endif