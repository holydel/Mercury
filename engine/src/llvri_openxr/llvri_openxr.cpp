#include "llvri_openxr.h"
#ifdef MERCURY_XR_API_OPENXR
#include "mercury_log.h"
#include "mercury_openxr.h"

#ifdef MERCURY_GRAPHICS_API_D3D12
#include "../llri_d3d12/d3d12_llri.h"
#endif

using namespace mercury;

XrInstance gXRInstance = XR_NULL_HANDLE;
XrSystemId gXRSystemID = {};
XrDebugUtilsMessengerEXT gXRDebugMessenger = XR_NULL_HANDLE;
XrSession gXRSession = XR_NULL_HANDLE;


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
	return true;
}

void llvri::shutdown()
{

	ShutdownOpenXR_Library();
	mercury::write_log_message("LowLevel XR Interface Shutdown: OPENXR");
}

bool llvri::update()
{

	return true;
}
#endif