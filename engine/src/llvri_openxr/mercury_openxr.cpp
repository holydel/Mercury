#include "mercury_openxr.h"
#ifdef MERCURY_XR_API_OPENXR
#include "mercury_log.h"

using namespace mercury;

static void* gLibHandle = nullptr;

#define XR_LOAD_GLOBAL_FUNC(func) xrGetInstanceProcAddr( nullptr, #func,(PFN_xrVoidFunction*)&func)
#define XR_LOAD_INSTANCE_FUNC(func) xrGetInstanceProcAddr( gXRInstance, #func,(PFN_xrVoidFunction*)&func)

#define XR_DEFINE_FUNCTION( fun ) PFN_##fun fun = nullptr;

////GLOBAL LEVEL
//XR_DEFINE_FUNCTION(xrNegotiateLoaderRuntimeInterface);
//XR_DEFINE_FUNCTION(xrGetInstanceProcAddr);
//XR_DEFINE_FUNCTION(xrInitializeLoaderKHR);
//XR_DEFINE_FUNCTION(xrEnumerateApiLayerProperties);
//XR_DEFINE_FUNCTION(xrEnumerateInstanceExtensionProperties);
//XR_DEFINE_FUNCTION(xrCreateInstance);
//
////INSTANCE LEVEL
//XR_DEFINE_FUNCTION(xrGetInstanceProperties);
//XR_DEFINE_FUNCTION(xrDestroyInstance);
//XR_DEFINE_FUNCTION(xrResultToString);
//XR_DEFINE_FUNCTION(xrGetVulkanGraphicsRequirements2KHR);
//XR_DEFINE_FUNCTION(xrCreateVulkanInstanceKHR);
//XR_DEFINE_FUNCTION(xrGetVulkanGraphicsDevice2KHR);
//XR_DEFINE_FUNCTION(xrCreateVulkanDeviceKHR);
//XR_DEFINE_FUNCTION(xrEnumerateViewConfigurations);
//
//XR_DEFINE_FUNCTION(xrGetSystem);
//XR_DEFINE_FUNCTION(xrGetSystemProperties);
//XR_DEFINE_FUNCTION(xrCreateSession);
//
//XR_DEFINE_FUNCTION(xrGetViewConfigurationProperties);
//XR_DEFINE_FUNCTION(xrEnumerateViewConfigurationViews);
//XR_DEFINE_FUNCTION(xrBeginSession);
//XR_DEFINE_FUNCTION(xrWaitFrame);
//XR_DEFINE_FUNCTION(xrBeginFrame);
//XR_DEFINE_FUNCTION(xrLocateViews);
//XR_DEFINE_FUNCTION(xrEndFrame);
//XR_DEFINE_FUNCTION(xrEndSession);
//XR_DEFINE_FUNCTION(xrPollEvent);
//
//XR_DEFINE_FUNCTION(xrEnumerateSwapchainFormats);
//XR_DEFINE_FUNCTION(xrCreateSwapchain);
//XR_DEFINE_FUNCTION(xrEnumerateSwapchainImages);
//XR_DEFINE_FUNCTION(xrAcquireSwapchainImage);
//XR_DEFINE_FUNCTION(xrWaitSwapchainImage);
//XR_DEFINE_FUNCTION(xrReleaseSwapchainImage);
//
//XR_DEFINE_FUNCTION(xrCreateReferenceSpace);
//
//XR_DEFINE_FUNCTION(xrEnumerateDisplayRefreshRatesFB);
//XR_DEFINE_FUNCTION(xrGetDisplayRefreshRateFB);
//XR_DEFINE_FUNCTION(xrRequestDisplayRefreshRateFB);
//
//XR_DEFINE_FUNCTION(xrSyncActions);
//XR_DEFINE_FUNCTION(xrEnumerateEnvironmentBlendModes);
//XR_DEFINE_FUNCTION(xrCreateActionSpace);
//XR_DEFINE_FUNCTION(xrStringToPath);
//XR_DEFINE_FUNCTION(xrCreateActionSet);
//XR_DEFINE_FUNCTION(xrCreateAction);
//XR_DEFINE_FUNCTION(xrLocateSpace);
//XR_DEFINE_FUNCTION(xrAttachSessionActionSets);
//XR_DEFINE_FUNCTION(xrGetActionStatePose);
//XR_DEFINE_FUNCTION(xrSuggestInteractionProfileBindings);
//XR_DEFINE_FUNCTION(xrGetCurrentInteractionProfile);
//XR_DEFINE_FUNCTION(xrGetActionStateBoolean);
//
//XR_DEFINE_FUNCTION(xrGetVisibilityMaskKHR);
//XR_DEFINE_FUNCTION(xrGetRenderModelPropertiesFB);
//XR_DEFINE_FUNCTION(xrLoadRenderModelFB);
//XR_DEFINE_FUNCTION(xrEnumerateRenderModelPathsFB);

#ifdef HEXCELL_PLATFORM_ANDROID
XR_DEFINE_FUNCTION(xrCreateSwapchainAndroidSurfaceKHR);
#endif
//SYSTEM LEVEL

XrInstance gXRInstance = nullptr;

#define XR_LOAD_GLOBAL_FUNC(func) xrGetInstanceProcAddr( nullptr, #func,(PFN_xrVoidFunction*)&func)
#define XR_LOAD_INSTANCE_FUNC(func) xrGetInstanceProcAddr( gXRInstance, #func,(PFN_xrVoidFunction*)&func)

void LoadOpenXR_Library()
{
	auto runtimeInfo = platform::getOpenXRRuntimeInfo();

	gLibHandle = platform::loadSharedLibrary(runtimeInfo.library_fullpath.c_str());

	//DWORD errCode = GetLastError();
	//void* addr = GetProcAddress((HMODULE)gLibHandle, "xrNegotiateLoaderApiLayerInterface");

	//void* addr2 = GetProcAddress((HMODULE)gLibHandle, "xrNegotiateLoaderRuntimeInterface");
	//errCode = GetLastError();

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
	//XR_LOAD_GLOBAL_FUNC(xrInitializeLoaderKHR);
	//XR_LOAD_GLOBAL_FUNC(xrEnumerateApiLayerProperties);

	//auto all_extensions = EnumerateOpenXRObjects(xrEnumerateInstanceExtensionProperties, XR_TYPE_EXTENSION_PROPERTIES);

	//auto libFullName = platform::getSharedLibraryFullFilename(gLibHandle);
	//write_log_message("Found openxr loader: %s", libFullName);
	auto allApiProps = EnumerateOpenXRObjects(xrEnumerateApiLayerProperties, XR_TYPE_API_LAYER_PROPERTIES);
	auto allExtensions = EnumerateOpenXRObjects(xrEnumerateInstanceExtensionProperties, XR_TYPE_EXTENSION_PROPERTIES);

	static const char* validationLayerName = "XR_APILAYER_LUNARG_core_validation";
	int a = 42;
}

void ShutdownOpenXR_Library()
{
	platform::unloadSharedLibrary(gLibHandle);
	gLibHandle = nullptr;
}

void LoadXRInstanceLevelFuncs(XrInstance instance)
{

}
#endif