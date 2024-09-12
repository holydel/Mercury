#include "mercury_openxr.h"
#ifdef MERCURY_XR_API_OPENXR
#include "mercury_log.h"

using namespace mercury;

static void* gLibHandle = nullptr;

#define XR_LOAD_GLOBAL_FUNC(func) xrGetInstanceProcAddr( nullptr, #func,(PFN_xrVoidFunction*)&func)
#define XR_LOAD_INSTANCE_FUNC(func) xrGetInstanceProcAddr( gXRInstance, #func,(PFN_xrVoidFunction*)&func)

#define XR_DEFINE_FUNCTION( fun ) PFN_##fun fun = nullptr;

////GLOBAL LEVEL
XR_DEFINE_FUNCTION(xrNegotiateLoaderRuntimeInterface);
XR_DEFINE_FUNCTION(xrGetInstanceProcAddr);
XR_DEFINE_FUNCTION(xrInitializeLoaderKHR);
XR_DEFINE_FUNCTION(xrEnumerateApiLayerProperties);
XR_DEFINE_FUNCTION(xrEnumerateInstanceExtensionProperties);
XR_DEFINE_FUNCTION(xrCreateInstance);
//
////INSTANCE LEVEL
XR_DEFINE_FUNCTION(xrGetInstanceProperties);
XR_DEFINE_FUNCTION(xrDestroyInstance);
XR_DEFINE_FUNCTION(xrResultToString);
XR_DEFINE_FUNCTION(xrEnumerateViewConfigurations);

XR_DEFINE_FUNCTION(xrGetSystem);
XR_DEFINE_FUNCTION(xrGetSystemProperties);
XR_DEFINE_FUNCTION(xrCreateSession);

XR_DEFINE_FUNCTION(xrGetViewConfigurationProperties);
XR_DEFINE_FUNCTION(xrEnumerateViewConfigurationViews);
XR_DEFINE_FUNCTION(xrBeginSession);
XR_DEFINE_FUNCTION(xrWaitFrame);
XR_DEFINE_FUNCTION(xrBeginFrame);
XR_DEFINE_FUNCTION(xrLocateViews);
XR_DEFINE_FUNCTION(xrEndFrame);
XR_DEFINE_FUNCTION(xrEndSession);
XR_DEFINE_FUNCTION(xrPollEvent);

XR_DEFINE_FUNCTION(xrEnumerateSwapchainFormats);
XR_DEFINE_FUNCTION(xrCreateSwapchain);
XR_DEFINE_FUNCTION(xrEnumerateSwapchainImages);
XR_DEFINE_FUNCTION(xrAcquireSwapchainImage);
XR_DEFINE_FUNCTION(xrWaitSwapchainImage);
XR_DEFINE_FUNCTION(xrReleaseSwapchainImage);

XR_DEFINE_FUNCTION(xrCreateReferenceSpace);

XR_DEFINE_FUNCTION(xrEnumerateDisplayRefreshRatesFB);
XR_DEFINE_FUNCTION(xrGetDisplayRefreshRateFB);
XR_DEFINE_FUNCTION(xrRequestDisplayRefreshRateFB);

XR_DEFINE_FUNCTION(xrSyncActions);
XR_DEFINE_FUNCTION(xrEnumerateEnvironmentBlendModes);
XR_DEFINE_FUNCTION(xrCreateActionSpace);
XR_DEFINE_FUNCTION(xrStringToPath);
XR_DEFINE_FUNCTION(xrCreateActionSet);
XR_DEFINE_FUNCTION(xrCreateAction);
XR_DEFINE_FUNCTION(xrLocateSpace);
XR_DEFINE_FUNCTION(xrAttachSessionActionSets);
XR_DEFINE_FUNCTION(xrGetActionStatePose);
XR_DEFINE_FUNCTION(xrSuggestInteractionProfileBindings);
XR_DEFINE_FUNCTION(xrGetCurrentInteractionProfile);
XR_DEFINE_FUNCTION(xrGetActionStateBoolean);

XR_DEFINE_FUNCTION(xrGetVisibilityMaskKHR);
XR_DEFINE_FUNCTION(xrGetRenderModelPropertiesFB);
XR_DEFINE_FUNCTION(xrLoadRenderModelFB);
XR_DEFINE_FUNCTION(xrEnumerateRenderModelPathsFB);

XR_DEFINE_FUNCTION(xrCreateDebugUtilsMessengerEXT);

#ifdef MERCURY_GRAPHICS_API_VULKAN
XR_DEFINE_FUNCTION(xrGetVulkanGraphicsRequirements2KHR);
XR_DEFINE_FUNCTION(xrCreateVulkanInstanceKHR);
XR_DEFINE_FUNCTION(xrGetVulkanGraphicsDevice2KHR);
XR_DEFINE_FUNCTION(xrCreateVulkanDeviceKHR);
#endif

#ifdef MERCURY_GRAPHICS_API_D3D12
XR_DEFINE_FUNCTION(xrGetD3D12GraphicsRequirementsKHR);
#endif

#ifdef HEXCELL_PLATFORM_ANDROID
XR_DEFINE_FUNCTION(xrCreateSwapchainAndroidSurfaceKHR);
#endif
//SYSTEM LEVEL

#define XR_LOAD_GLOBAL_FUNC(func) xrGetInstanceProcAddr( nullptr, #func,(PFN_xrVoidFunction*)&func)
#define XR_LOAD_INSTANCE_FUNC(func) xrGetInstanceProcAddr( gXRInstance, #func,(PFN_xrVoidFunction*)&func)

std::vector<OpenXRLayer> gImplicitLayers;
std::vector<OpenXRLayer> gExplicitLayers;

void LoadLayer(std::vector<OpenXRLayer>& vec, const OpenXRLayerInfo& info)
{
	OpenXRLayer& layer = vec.emplace_back();

	layer.libHandle = platform::loadSharedLibrary(info.library_fullpath.c_str());
	layer.info = info;
	LOAD_FUNC_BY_NAME(layer.libHandle, layer.xrNegotiateLoaderApiLayerInterface, info.xrNegotiateLoaderApiLayerInterfaceFuncName.c_str());
	LOAD_FUNC_BY_NAME(layer.libHandle, layer.xrGetInstanceProcAddr, info.xrGetInstanceProcAddrFuncName.c_str());
	LOAD_FUNC_BY_NAME(layer.libHandle, layer.xrCreateApiLayerInstance, "xrCreateApiLayerInstance");
}

void LoadOpenXR_Library()
{
	//
	//D:\\UnrealEngine-release\\Engine\\Binaries\\ThirdParty\\OpenXR\\win64\\

	gLibHandle = platform::loadSharedLibrary("C:\\Users\\holyd\\Downloads\\openxr_loader_windows-1.1.40\\x64\\bin\\openxr_loader.dll");


	int a = 42;
	//manual runtime/layers loading. temprorary disabled

	//auto runtimeInfo = platform::getOpenXRRuntimeInfo();

	//gLibHandle = platform::loadSharedLibrary(runtimeInfo.library_fullpath.c_str());

	//LOAD_FUNC_PTR(gLibHandle, xrNegotiateLoaderRuntimeInterface);

	//XrNegotiateLoaderInfo loader_info = {};
	//loader_info.structType = XR_LOADER_INTERFACE_STRUCT_LOADER_INFO;
	//loader_info.structVersion = XR_LOADER_INFO_STRUCT_VERSION;
	//loader_info.structSize = sizeof(XrNegotiateLoaderInfo);
	//loader_info.minInterfaceVersion = 1;
	//loader_info.maxInterfaceVersion = XR_CURRENT_LOADER_RUNTIME_VERSION;
	//loader_info.minApiVersion = XR_MAKE_VERSION(1, 0, 0);
	//loader_info.maxApiVersion = XR_MAKE_VERSION(1, 0x3ff, 0xfff);  // Maximum allowed version for this major version

	//XrNegotiateRuntimeRequest request = { XR_LOADER_INTERFACE_STRUCT_RUNTIME_REQUEST ,XR_LOADER_INFO_STRUCT_VERSION ,sizeof(XrNegotiateRuntimeRequest)};

	//auto res = xrNegotiateLoaderRuntimeInterface(&loader_info, &request);

	//xrGetInstanceProcAddr = request.getInstanceProcAddr;


	//XR_LOAD_GLOBAL_FUNC(xrEnumerateInstanceExtensionProperties);
	//XR_LOAD_GLOBAL_FUNC(xrCreateInstance);

	//auto layersInfo = platform::getOpenXRLayersInfo();


	//for (const auto& l : layersInfo.implicitLayers)
	//	LoadLayer(gImplicitLayers,l);

	//for (const auto& l : layersInfo.explicitLayers)
	//	LoadLayer(gExplicitLayers,l);
}

const char* MercuryXrResultToString(XrResult res)
{
	static char buff[XR_MAX_RESULT_STRING_SIZE] = {};
	xrResultToString(gXRInstance, res, buff);
	return buff;
}

void ShutdownOpenXR_Library()
{
	platform::unloadSharedLibrary(gLibHandle);
	gLibHandle = nullptr;
}

void LoadXRGlobalLevelFuncs()
{
	LOAD_FUNC_PTR(gLibHandle, xrGetInstanceProcAddr);
	//LOAD_FUNC_PTR(gLibHandle, xrCreateInstance);
	//LOAD_FUNC_PTR(gLibHandle, xrEnumerateApiLayerProperties);
	//LOAD_FUNC_PTR(gLibHandle, xrEnumerateInstanceExtensionProperties);

	XR_LOAD_GLOBAL_FUNC(xrEnumerateApiLayerProperties);
	XR_LOAD_GLOBAL_FUNC(xrEnumerateInstanceExtensionProperties);
	XR_LOAD_GLOBAL_FUNC(xrCreateInstance);
}

void LoadXRInstanceLevelFuncs()
{
	//
	////INSTANCE LEVEL
	XR_LOAD_INSTANCE_FUNC(xrGetInstanceProperties);
	XR_LOAD_INSTANCE_FUNC(xrDestroyInstance);
	XR_LOAD_INSTANCE_FUNC(xrEnumerateViewConfigurations);
	XR_LOAD_INSTANCE_FUNC(xrResultToString);

	XR_LOAD_INSTANCE_FUNC(xrGetSystem);
	XR_LOAD_INSTANCE_FUNC(xrGetSystemProperties);
	XR_LOAD_INSTANCE_FUNC(xrCreateSession);

	XR_LOAD_INSTANCE_FUNC(xrGetViewConfigurationProperties);
	XR_LOAD_INSTANCE_FUNC(xrEnumerateViewConfigurationViews);
	XR_LOAD_INSTANCE_FUNC(xrBeginSession);
	XR_LOAD_INSTANCE_FUNC(xrWaitFrame);
	XR_LOAD_INSTANCE_FUNC(xrBeginFrame);
	XR_LOAD_INSTANCE_FUNC(xrLocateViews);
	XR_LOAD_INSTANCE_FUNC(xrEndFrame);
	XR_LOAD_INSTANCE_FUNC(xrEndSession);
	XR_LOAD_INSTANCE_FUNC(xrPollEvent);

	XR_LOAD_INSTANCE_FUNC(xrEnumerateSwapchainFormats);
	XR_LOAD_INSTANCE_FUNC(xrCreateSwapchain);
	XR_LOAD_INSTANCE_FUNC(xrEnumerateSwapchainImages);
	XR_LOAD_INSTANCE_FUNC(xrAcquireSwapchainImage);
	XR_LOAD_INSTANCE_FUNC(xrWaitSwapchainImage);
	XR_LOAD_INSTANCE_FUNC(xrReleaseSwapchainImage);

	XR_LOAD_INSTANCE_FUNC(xrCreateReferenceSpace);

	XR_LOAD_INSTANCE_FUNC(xrEnumerateDisplayRefreshRatesFB);
	XR_LOAD_INSTANCE_FUNC(xrGetDisplayRefreshRateFB);
	XR_LOAD_INSTANCE_FUNC(xrRequestDisplayRefreshRateFB);

	XR_LOAD_INSTANCE_FUNC(xrSyncActions);
	XR_LOAD_INSTANCE_FUNC(xrEnumerateEnvironmentBlendModes);
	XR_LOAD_INSTANCE_FUNC(xrCreateActionSpace);
	XR_LOAD_INSTANCE_FUNC(xrStringToPath);
	XR_LOAD_INSTANCE_FUNC(xrCreateActionSet);
	XR_LOAD_INSTANCE_FUNC(xrCreateAction);
	XR_LOAD_INSTANCE_FUNC(xrLocateSpace);
	XR_LOAD_INSTANCE_FUNC(xrAttachSessionActionSets);
	XR_LOAD_INSTANCE_FUNC(xrGetActionStatePose);
	XR_LOAD_INSTANCE_FUNC(xrSuggestInteractionProfileBindings);
	XR_LOAD_INSTANCE_FUNC(xrGetCurrentInteractionProfile);
	XR_LOAD_INSTANCE_FUNC(xrGetActionStateBoolean);

	XR_LOAD_INSTANCE_FUNC(xrGetVisibilityMaskKHR);
	XR_LOAD_INSTANCE_FUNC(xrGetRenderModelPropertiesFB);
	XR_LOAD_INSTANCE_FUNC(xrLoadRenderModelFB);
	XR_LOAD_INSTANCE_FUNC(xrEnumerateRenderModelPathsFB);

	XR_LOAD_INSTANCE_FUNC(xrCreateDebugUtilsMessengerEXT);

#ifdef MERCURY_GRAPHICS_API_D3D12
	XR_LOAD_INSTANCE_FUNC(xrGetD3D12GraphicsRequirementsKHR);
#endif
}
#endif