#include "llvri_openxr.h"
#ifdef MERCURY_XR_API_OPENXR
#include "mercury_log.h"
#include "mercury_openxr.h"
#include <array>

#ifdef MERCURY_GRAPHICS_API_D3D12
#include "../llri_d3d12/d3d12_llri.h"
#endif

using namespace mercury;

XrInstance gXRInstance = XR_NULL_HANDLE;
XrSystemId gXRSystemID = {};
XrDebugUtilsMessengerEXT gXRDebugMessenger = XR_NULL_HANDLE;
XrSession gXRSession = XR_NULL_HANDLE;
XrViewConfigurationType gXRPreferredConfigType;
u16 gRecommendedWidth = 0;
u16 gRecommendedHeight = 0;
XrSwapchain gXRSwapchain = XR_NULL_HANDLE;
XrEnvironmentBlendMode gCurrentBlendMode = XrEnvironmentBlendMode::XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
XrSpace gLocalSpace = {};
XrSpace gViewSpace = {};
XrSpace gStageSpace = {};
XrCompositionLayerProjection gCurrentProjectionLayer = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
std::array<XrCompositionLayerProjectionView, 2> gCurrentProjectionLayerView;

XrBool32 XrDebugMessageHandler(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageTypes, const XrDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
	if (messageSeverity >= XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		mercury::write_log_message("%s", callbackData->message);
	}
	return false;
}

struct Props
{
	XrInstanceProperties instanceProps = { XR_TYPE_INSTANCE_PROPERTIES, nullptr };
	XrSystemProperties systemProps = { XR_TYPE_SYSTEM_PROPERTIES, nullptr };
};

Props gXRProps;

void CreateXRInstance()
{
	auto allApiProps = EnumerateOpenXRObjects(xrEnumerateApiLayerProperties, XR_TYPE_API_LAYER_PROPERTIES);
	auto allExtensions = EnumerateOpenXRObjects(xrEnumerateInstanceExtensionProperties, XR_TYPE_EXTENSION_PROPERTIES);

	std::vector<const char*> enabledLayers;
	std::vector<const char*> enabledExtensions;

	enabledLayers.push_back("XR_APILAYER_LUNARG_core_validation");
	enabledExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);

#ifdef MERCURY_GRAPHICS_API_D3D12
	enabledExtensions.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
#endif
#ifdef MERCURY_GRAPHICS_API_VULKAN
	enabledExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME); "XR_KHR_vulkan_enable2"
#endif

	XrDebugUtilsMessengerCreateInfoEXT debugMessengerCI{ XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	debugMessengerCI.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	//debugMessengerCI.messageSeverities |= XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	//debugMessengerCI.messageSeverities |= XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

	debugMessengerCI.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT
		| XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debugMessengerCI.userData = nullptr;
	debugMessengerCI.userCallback = &XrDebugMessageHandler;

	XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
	createInfo.enabledExtensionCount = enabledExtensions.size();
	createInfo.enabledExtensionNames = enabledExtensions.data();
	createInfo.enabledApiLayerCount = 0;// enabledLayers.size();
	createInfo.enabledApiLayerNames = enabledLayers.data();
	strcpy_s(createInfo.applicationInfo.engineName, "Mercury");
	strcpy_s(createInfo.applicationInfo.applicationName, "MercuryTest");
	createInfo.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);
	createInfo.applicationInfo.applicationVersion = 1;
	createInfo.applicationInfo.engineVersion = 1;

	//createInfo.next = &debugMessengerCI;

	xrCreateInstance(&createInfo, &gXRInstance);

	LoadXRInstanceLevelFuncs();

	auto& props = gXRProps.instanceProps;
	XR_CALL(xrGetInstanceProperties(gXRInstance, &props));

	mercury::write_log_message("Selected XR Runtime: %s version: %d.%d.%d", props.runtimeName,
		XR_VERSION_MAJOR(props.runtimeVersion), XR_VERSION_MINOR(props.runtimeVersion),
		XR_VERSION_PATCH(props.runtimeVersion));
}

void CreateDebugListener()
{
	XrDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
	messengerCreateInfo.type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messengerCreateInfo.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	messengerCreateInfo.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	messengerCreateInfo.userCallback = &XrDebugMessageHandler;
	messengerCreateInfo.userData = nullptr;

	xrCreateDebugUtilsMessengerEXT(gXRInstance, &messengerCreateInfo, &gXRDebugMessenger);
}

void GetSystem()
{
	XrSystemGetInfo systemGI{ XR_TYPE_SYSTEM_GET_INFO };
	systemGI.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	XR_CALL(xrGetSystem(gXRInstance, &systemGI, &gXRSystemID));
	
	auto& props = gXRProps.systemProps;
	XR_CALL(xrGetSystemProperties(gXRInstance, gXRSystemID, &props));

	mercury::write_log_message("Selected XR System: %s", props.systemName);

	mercury::write_log_message("XR System layers: %d max width: %d max height: %d", props.graphicsProperties.maxLayerCount
		, props.graphicsProperties.maxSwapchainImageWidth, props.graphicsProperties.maxSwapchainImageHeight);
	
}

bool llvri::initialize()
{
	mercury::write_log_message("LowLevel XR Interface Initialize: OPENXR");
	LoadOpenXR_Library();

	LoadXRGlobalLevelFuncs();

	CreateXRInstance();

	if (gXRInstance == XR_NULL_HANDLE)
	{
		mercury::write_log_message("No XR runtime or No Connected Device!");
		return false;
	}

	CreateDebugListener();

	GetSystem();

	int a = 42;
	//TODO: manual layers/runtimes loading
	// 
	//XrNegotiateLoaderInfo loaderInfo = {};
	//loaderInfo.structType = XR_LOADER_INTERFACE_STRUCT_LOADER_INFO;
	//loaderInfo.structVersion = XR_LOADER_INFO_STRUCT_VERSION;
	//loaderInfo.structSize = sizeof(loaderInfo);
	//loaderInfo.minInterfaceVersion = 1;
	//loaderInfo.maxInterfaceVersion = XR_CURRENT_LOADER_RUNTIME_VERSION;
	//loaderInfo.minApiVersion = XR_MAKE_VERSION(1, 0, 0);
	//loaderInfo.maxApiVersion = XR_MAKE_VERSION(1, 0x3ff, 0xfff);  // Maximum allowed version for this major version

	//XrApiLayerNextInfo nextInfo = {};
	//nextInfo.structType = XR_LOADER_INTERFACE_STRUCT_API_LAYER_NEXT_INFO;
	//nextInfo.structVersion = XR_API_LAYER_NEXT_INFO_STRUCT_VERSION;
	//nextInfo.structSize = sizeof(nextInfo);
	//nextInfo.nextGetInstanceProcAddr = xrGetInstanceProcAddr;

	//XrNegotiateApiLayerRequest layerRequest = {};
	//layerRequest.structType = XR_LOADER_INTERFACE_STRUCT_API_LAYER_REQUEST;
	//layerRequest.structVersion = XR_API_LAYER_INFO_STRUCT_VERSION;
	//layerRequest.structSize = sizeof(layerRequest);

	//auto l = gExplicitLayers[0];
	//l.xrNegotiateLoaderApiLayerInterface(&loaderInfo, l.info.name.c_str(), &layerRequest);
	//
	////auto allApiProps = EnumerateOpenXRObjects(xrEnumerateApiLayerProperties, XR_TYPE_API_LAYER_PROPERTIES);
	////auto allExtensions = EnumerateOpenXRObjects(xrEnumerateInstanceExtensionProperties, XR_TYPE_EXTENSION_PROPERTIES);

	//// Create an OpenXR instance with the debug utils messenger
	//XrInstanceCreateInfo createInfo = {};
	//createInfo.type = XR_TYPE_INSTANCE_CREATE_INFO;
	//createInfo.next = &createInfo;
	//createInfo.enabledExtensionCount = 0;
	//const char* enabledExtensions[] = { XR_EXT_DEBUG_UTILS_EXTENSION_NAME };
	//createInfo.enabledExtensionNames = enabledExtensions;

	//XrApiLayerCreateInfo layerCinfo = { XR_LOADER_INTERFACE_STRUCT_API_LAYER_CREATE_INFO,XR_API_LAYER_CREATE_INFO_STRUCT_VERSION,sizeof(XrApiLayerCreateInfo) };
	//layerCinfo.nextInfo = &nextInfo;

	//layerRequest.createApiLayerInstance(&createInfo, &layerCinfo, &gXRInstance);

	////xrCreateInstance(&createInfo, &gXRInstance);

	//// Set up the debug utils messenger


	return true;
}

bool llvri::createSession()
{
	//XrGraphicsRequirementsD3D12KHR 

#ifdef MERCURY_GRAPHICS_API_D3D12
	XrGraphicsRequirementsD3D12KHR req = { XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR };
	xrGetD3D12GraphicsRequirementsKHR(gXRInstance, gXRSystemID, &req);

	XrGraphicsBindingD3D12KHR graphicsBinding = { XR_TYPE_GRAPHICS_BINDING_D3D12_KHR };
	graphicsBinding.device = gDevice;
	graphicsBinding.queue = gCommandQueue;
#endif

	XrSessionCreateInfo info = { XR_TYPE_SESSION_CREATE_INFO };
	info.systemId = gXRSystemID;
	info.createFlags = 0;
	info.next = &graphicsBinding;

	XR_CALL(xrCreateSession(gXRInstance, &info, &gXRSession));

	auto viewConfigs = EnumerateOpenXRObjects(gXRInstance, gXRSystemID, xrEnumerateViewConfigurations);

	for (auto vConfig : viewConfigs)
	{
		gXRPreferredConfigType = vConfig;

		XrViewConfigurationProperties props = {};
		props.type = XR_TYPE_VIEW_CONFIGURATION_PROPERTIES;

		XR_CALL(xrGetViewConfigurationProperties(gXRInstance, gXRSystemID, XrViewConfigurationType::XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, &props));

		auto cfgViews = EnumerateOpenXRObjects(gXRInstance, gXRSystemID, XrViewConfigurationType::XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_TYPE_VIEW_CONFIGURATION_VIEW, xrEnumerateViewConfigurationViews);
		gRecommendedWidth = cfgViews[0].recommendedImageRectWidth;
		gRecommendedHeight = cfgViews[0].recommendedImageRectHeight;
	}

	XrSwapchainCreateInfo swapchainCreateInfo = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
	swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
	swapchainCreateInfo.faceCount = 1;
	swapchainCreateInfo.format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainCreateInfo.height = gRecommendedHeight;
	swapchainCreateInfo.mipCount = 1;
	swapchainCreateInfo.sampleCount = 1;
	swapchainCreateInfo.width = gRecommendedWidth;
	swapchainCreateInfo.arraySize = 2;

	XR_CALL(xrCreateSwapchain(gXRSession, &swapchainCreateInfo, &gXRSwapchain));

	u32 cntImagesInSwapchain = 0;

	XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
	sessionBeginInfo.primaryViewConfigurationType = gXRPreferredConfigType;

	XR_CALL(xrBeginSession(gXRSession, &sessionBeginInfo));
	XR_CALL(xrEnumerateSwapchainImages(gXRSwapchain, 0, &cntImagesInSwapchain, nullptr));

	std::vector<XrSwapchainImageD3D12KHR> sw_images(cntImagesInSwapchain);

	for (auto& swImageInfo : sw_images)
	{
		swImageInfo.next = nullptr;
		swImageInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR;
		swImageInfo.texture = nullptr;
	}

	XR_CALL(xrEnumerateSwapchainImages(gXRSwapchain, cntImagesInSwapchain, &cntImagesInSwapchain, (XrSwapchainImageBaseHeader*)sw_images.data()));

	uint32_t numBlendModes = 0;
	XrEnvironmentBlendMode blendModes[16] = { };

	xrEnumerateEnvironmentBlendModes(gXRInstance, gXRSystemID, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 16, &numBlendModes, blendModes);
	gCurrentBlendMode = blendModes[0];

	XrReferenceSpaceCreateInfo referenceSpaceCI{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };	
	referenceSpaceCI.poseInReferenceSpace = { {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} };

	referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
	XR_CALL(xrCreateReferenceSpace(gXRSession, &referenceSpaceCI, &gLocalSpace));
	referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
	XR_CALL(xrCreateReferenceSpace(gXRSession, &referenceSpaceCI, &gViewSpace));
	referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	XR_CALL(xrCreateReferenceSpace(gXRSession, &referenceSpaceCI, &gStageSpace));

	for (int i = 0; i < 2; ++i)
	{
		gCurrentProjectionLayerView[i].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
	}
	return true;
}

void llvri::shutdown()
{

	ShutdownOpenXR_Library();
	mercury::write_log_message("LowLevel XR Interface Shutdown: OPENXR");
}

bool llvri::update()
{
	if (gXRInstance == XR_NULL_HANDLE)
	{
		return false;
	}

	// Get the XrFrameState for timing and rendering info.
	XrFrameState frameState{ XR_TYPE_FRAME_STATE };
	XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
	XR_CALL(xrWaitFrame(gXRSession, &frameWaitInfo, &frameState));

	// Tell the OpenXR compositor that the application is beginning the frame.
	XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
	XR_CALL(xrBeginFrame(gXRSession, &frameBeginInfo));

	std::array<XrView, 2> views = { { {XR_TYPE_VIEW},{XR_TYPE_VIEW } } };

	XrViewState viewState{ XR_TYPE_VIEW_STATE };  // Will contain information on whether the position and/or orientation is valid and/or tracked.
	XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
	viewLocateInfo.viewConfigurationType = gXRPreferredConfigType;
	viewLocateInfo.displayTime = frameState.predictedDisplayTime;
	viewLocateInfo.space = gLocalSpace;
	uint32_t viewCount = 0;
	XrResult result = xrLocateViews(gXRSession, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());

	XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
	uint32_t colorImageIndex = 0;
	XR_CALL(xrAcquireSwapchainImage(gXRSwapchain, &acquireInfo, &colorImageIndex));

	XrSwapchainImageWaitInfo waitInfo = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
	waitInfo.timeout = XR_INFINITE_DURATION;

	XR_CALL(xrWaitSwapchainImage(gXRSwapchain, &waitInfo));

	//todo render into swapchain


	XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
	XR_CALL(xrReleaseSwapchainImage(gXRSwapchain, &releaseInfo));

	//
	for (int i = 0; i < 2; ++i)
	{
		gCurrentProjectionLayerView[i].fov = views[i].fov;
		gCurrentProjectionLayerView[i].pose = views[i].pose;
		gCurrentProjectionLayerView[i].subImage.swapchain = gXRSwapchain;
		gCurrentProjectionLayerView[i].subImage.imageRect = { 0,0,gRecommendedWidth,gRecommendedHeight };
		gCurrentProjectionLayerView[i].subImage.imageArrayIndex = 0;
	}

	gCurrentProjectionLayer.space = gLocalSpace;
	gCurrentProjectionLayer.viewCount = gCurrentProjectionLayerView.size();
	gCurrentProjectionLayer.views = gCurrentProjectionLayerView.data();
	gCurrentProjectionLayer.layerFlags = XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;

	const auto* projLayerPtr = &gCurrentProjectionLayer;

	// Tell OpenXR that we are finished with this frame; specifying its display time, environment blending and layers.
	XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
	frameEndInfo.displayTime = frameState.predictedDisplayTime;
	frameEndInfo.environmentBlendMode = gCurrentBlendMode;
	frameEndInfo.layerCount = 1;
	frameEndInfo.layers = (const XrCompositionLayerBaseHeader*const*)&projLayerPtr;
	XR_CALL(xrEndFrame(gXRSession, &frameEndInfo));

	return true;
}
#endif