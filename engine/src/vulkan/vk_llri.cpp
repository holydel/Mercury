#include "vk_llri.h"

#include <cstring>

#include "vk_swapchain.h"

#include "../application.h"
#include <string>

//have extern interface in header
VkInstance gInstance = nullptr;
VkAllocationCallbacks* gGlobalAllocationsCallbacks = nullptr;
VkPhysicalDevice gPhysicalDevice = nullptr;
VkDevice gDevice = nullptr;
VmaAllocator gVMAallocator = nullptr;
VkQueue gMainQueue = nullptr;

//local
static VkDebugUtilsMessengerEXT gVulkanDebugMessenger = nullptr;
using namespace mercury;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	write_log_message(pCallbackData->pMessage);

	return VK_FALSE;
}

static void InitializeInstance()
{
	const auto& renderCfg = gApplication->config.render;

	u32 installedVersion = 0;

	if (vkEnumerateInstanceVersion != nullptr)
	{
		VK_CALL(vkEnumerateInstanceVersion(&installedVersion));

		int major = VK_VERSION_MAJOR(installedVersion);
		int minor = VK_VERSION_MINOR(installedVersion);
		int patch = VK_VERSION_PATCH(installedVersion);

		mercury::write_log_message(u8"Found vulkan instance: %d.%d.%d", major, minor, patch);
	}

	struct InstanceExtender
	{
		std::vector<const char*> enabledLayers;
		std::vector<const char*> enabledExtensions;

		std::vector<VkLayerProperties> all_layers;
		std::vector<VkExtensionProperties> all_extensions;

		InstanceExtender()
		{
			all_layers = EnumerateVulkanObjects(vkEnumerateInstanceLayerProperties);
			all_extensions = EnumerateVulkanObjects(vkEnumerateInstanceExtensionProperties);
		}

		bool TryAddLayer(const char* layer_name)
		{
			bool exists = false;

			for (auto& l : all_layers)
			{
				if (strcmp(l.layerName, layer_name) == 0)
				{
					exists = true;
					break;
				}
			}

			if (exists)
			{
				enabledLayers.push_back(layer_name);
			}

			return exists;
		}

		bool TryAddExtension(const char* extension_name)
		{
			bool exists = false;

			for (auto& e : all_extensions)
			{
				if (strcmp(e.extensionName, extension_name) == 0)
				{
					exists = true;
					break;
				}
			}

			if (exists)
			{
				enabledExtensions.push_back(extension_name);
			}

			return exists;
		}

		u32 NumEnabledLayers() const
		{
			return (u32)enabledLayers.size();
		}

		u32 NumEnabledExtension() const
		{
			return (u32)enabledExtensions.size();
		}

		const char** const EnabledLayers() const
		{
			return (const char** const)enabledLayers.data();
		}

		const char** const EnabledExtensions() const
		{
			return (const char** const)enabledExtensions.data();
		}

	} instance_extender;

	bool isDebugLayers = renderCfg.enableDebugLayers;

	if (isDebugLayers)
	{
		instance_extender.TryAddLayer("VK_LAYER_KHRONOS_validation");
		instance_extender.TryAddExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (gApplication->config.outputMode != OutputMode::Headless)
	{
		instance_extender.TryAddExtension(VK_KHR_SURFACE_EXTENSION_NAME);
	}

	instance_extender.TryAddExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

#ifdef MERCURY_PLATFORM_WINDOWS
	instance_extender.TryAddExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef MERCURY_PLATFORM_ANDROID
	instance_extender.TryAddExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif
#ifdef MERCURY_PLATFORM_LINUX
	//VK_KHR_xcb_surface
	instance_extender.TryAddExtension("VK_KHR_xcb_surface");
#endif
#ifdef MERCURY_PLATFORM_MACOS
    //Found drivers that contain devices which support the portability subset, but the instance does not enumerate portability drivers!
    //Applications that wish to enumerate portability drivers must set the VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR bit in the VkInstanceCreateInfo flags
    //and enable the VK_KHR_portability_enumeration instance extension.
    instance_extender.TryAddExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instance_extender.TryAddExtension("VK_MVK_macos_surface");
#endif

	instance_extender.TryAddExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.flags = 0;
	debugCreateInfo.pNext = nullptr;
	debugCreateInfo.pUserData = nullptr;
	debugCreateInfo.pfnUserCallback = &debugCallback;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;


	VkApplicationInfo vkAppInfo;
	vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pNext = nullptr;
	vkAppInfo.pApplicationName = gApplication->config.appName;
	vkAppInfo.pEngineName = "mercury";
	vkAppInfo.applicationVersion = gApplication->config.version.packed;
	vkAppInfo.apiVersion = installedVersion ? installedVersion : VK_MAKE_API_VERSION(0, 1, 1, 0);

	VkInstanceCreateInfo createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	if (isDebugLayers)
	{
		createInfo.pNext = &debugCreateInfo;
	}

	createInfo.pApplicationInfo = &vkAppInfo;
	createInfo.ppEnabledLayerNames = instance_extender.EnabledLayers();
	createInfo.ppEnabledExtensionNames = instance_extender.EnabledExtensions();
	createInfo.enabledLayerCount = instance_extender.NumEnabledLayers();
	createInfo.enabledExtensionCount = instance_extender.NumEnabledExtension();

	createInfo.flags = 0;
#ifdef MERCURY_PLATFORM_MACOS
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
	VK_CALL(vkCreateInstance(&createInfo, gGlobalAllocationsCallbacks, &gInstance));

	LoadVkInstanceLevelFuncs(gInstance);

	if (vkCreateDebugUtilsMessengerEXT != nullptr)
	{
		vkCreateDebugUtilsMessengerEXT(gInstance, &debugCreateInfo, gGlobalAllocationsCallbacks, &gVulkanDebugMessenger);
	}
}

static i8 ChoosePhysicalDeviceByHeuristics(const std::vector<VkPhysicalDevice>& allDevices)
{
	//TODO: selection heuristics
	return 0;
}

static void ChoosePhysicalDevice()
{
	const auto& renderCfg = gApplication->config.render;
	auto phys_devices = EnumerateVulkanObjects(gInstance, vkEnumeratePhysicalDevices);

	for (int i = 0; i < phys_devices.size(); ++i)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(phys_devices[i], &props);

		write_log_message("Found VK device (%d): %s", i, props.deviceName);
	}

	auto selectedAdapterID = renderCfg.adapterID == -1 ? ChoosePhysicalDeviceByHeuristics(phys_devices) : renderCfg.adapterID;
	
	gPhysicalDevice = phys_devices[selectedAdapterID];

    if(vkGetPhysicalDeviceProperties2)
    {
        VkPhysicalDeviceDriverProperties driverProps = {};
        driverProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

        VkPhysicalDeviceProperties2 props = {};
        props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        props.pNext = &driverProps;

        vkGetPhysicalDeviceProperties2(gPhysicalDevice, &props);

        auto major = VK_API_VERSION_MAJOR(props.properties.apiVersion);
        auto minor = VK_API_VERSION_MINOR(props.properties.apiVersion);
        auto patch = VK_API_VERSION_PATCH(props.properties.apiVersion);

        write_log_message("Selected Physical Device: %s", props.properties.deviceName);
        write_log_message("Vulkan API: %d.%d.%d", major, minor, patch);

        write_log_message("DRIVER: %s %s", driverProps.driverName, driverProps.driverInfo);
    }
    else
    {
        VkPhysicalDeviceProperties props = {};
        vkGetPhysicalDeviceProperties(gPhysicalDevice, &props);

        auto major = VK_API_VERSION_MAJOR(props.apiVersion);
        auto minor = VK_API_VERSION_MINOR(props.apiVersion);
        auto patch = VK_API_VERSION_PATCH(props.apiVersion);

        write_log_message("Selected Physical Device: %s", props.deviceName);
        write_log_message("Vulkan API: %d.%d.%d", major, minor, patch);
    }

}

static void CreateDevice()
{
	struct DeviceExtender
	{
		std::vector<const char*> enabledExtensions;
		std::vector<VkExtensionProperties> all_extensions;
		std::string all_extensions_str;

		DeviceExtender()
		{
			all_extensions_str.reserve(65536);

			const char* fakeLayer = nullptr;
			all_extensions = EnumerateVulkanObjects(gPhysicalDevice, fakeLayer, vkEnumerateDeviceExtensionProperties);

			write_log_message("Device extension lists:");

			for (auto& ex : all_extensions)
			{
				write_log_message(ex.extensionName);

				all_extensions_str += ex.extensionName;
				all_extensions_str += "\n";
			}
		}

		bool TryAddExtension(const char* extension_name)
		{
			bool exists = false;

			for (auto& e : all_extensions)
			{
				if (strcmp(e.extensionName, extension_name) == 0)
				{
					exists = true;
					break;
				}
			}

			if (exists)
			{
				enabledExtensions.push_back(extension_name);
			}

			return exists;
		}

		u32 NumEnabledExtension() const
		{
			return (u32)enabledExtensions.size();
		}

		const char** const EnabledExtensions() const
		{
			return (const char** const)enabledExtensions.data();
		}

	} device_extender;

	std::vector<VkQueueFamilyProperties> queueFamilies(32);


	u32 familiesCount = 32;
	vkGetPhysicalDeviceQueueFamilyProperties(gPhysicalDevice, &familiesCount, queueFamilies.data());


	for (u32 i = 0; i < familiesCount; ++i)
	{
		VkQueueFamilyProperties queueFamily = queueFamilies[i];
		bool isGraphicsQueue = queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
		bool isTransferQueue = queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT;
		bool isComputeQueue = queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
		bool isSparseBindingQueue = queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;

		bool isVideoDecodeQueue = queueFamily.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR;
		bool isVideoEncodeQueue = queueFamily.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR;

		bool isOpticalFlowNVQueue = queueFamily.queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV;

#ifdef MERCURY_PLATFORM_WINDOWS
		bool supportPresent = vkGetPhysicalDeviceWin32PresentationSupportKHR != nullptr ? vkGetPhysicalDeviceWin32PresentationSupportKHR(gPhysicalDevice, i) : false;
#else
		bool supportPresent = false;
#endif

		mercury::write_log_message("queueID: %d (%d) image gran (%d x %d x %d) %s%s%s%s%s%s%s",
			i, queueFamily.queueCount,
			queueFamily.minImageTransferGranularity.width,
			queueFamily.minImageTransferGranularity.height,
			queueFamily.minImageTransferGranularity.depth,
			isGraphicsQueue ? "GRAPHICS " : "",
			isTransferQueue ? "TRANSFER " : "",
			isComputeQueue ? "COMPUTE " : "",
			isSparseBindingQueue ? "SPARSE_BINDING " : "",
			isVideoDecodeQueue ? "VIDEO_DECODE " : "",
			isVideoEncodeQueue ? "VIDEO_ENCODE " : "",
			isOpticalFlowNVQueue ? "OPTICAL_FLOW_NV " : "",
			supportPresent ? "PRESENT" : "");

	}

	float highPriors[2] = { 1.0f,1.0f };

	VkDeviceQueueCreateInfo queueCreateInfoGraphics;
	queueCreateInfoGraphics.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfoGraphics.pNext = 0;
	queueCreateInfoGraphics.pQueuePriorities = highPriors;
	queueCreateInfoGraphics.queueCount = 1;
	queueCreateInfoGraphics.queueFamilyIndex = 0;
	queueCreateInfoGraphics.flags = 0;

	std::vector< VkDeviceQueueCreateInfo> requestedQueues;
	requestedQueues.push_back(queueCreateInfoGraphics);

	device_extender.TryAddExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME); //TODO: fill device info with supported features

#ifdef MERCURY_PLATFORM_MACOS
device_extender.TryAddExtension(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif
	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(requestedQueues.size());
	deviceCreateInfo.pQueueCreateInfos = requestedQueues.data();
	deviceCreateInfo.enabledLayerCount = 0; //deprecated
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.pEnabledFeatures = nullptr;
	deviceCreateInfo.ppEnabledExtensionNames = device_extender.EnabledExtensions(); //deprecated
	deviceCreateInfo.enabledExtensionCount = device_extender.NumEnabledExtension();

	VK_CALL(vkCreateDevice(gPhysicalDevice, &deviceCreateInfo, nullptr, &gDevice));


	LoadVkDeviceLevelFuncs(gDevice);

	vkGetDeviceQueue(gDevice, 0, 0, &gMainQueue);

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = gPhysicalDevice;
	allocatorInfo.device = gDevice;
	allocatorInfo.instance = gInstance;

	VmaVulkanFunctions functions;

	functions.vkAllocateMemory = vkAllocateMemory;
	functions.vkBindBufferMemory = vkBindBufferMemory;

	functions.vkBindImageMemory = vkBindImageMemory;

	functions.vkCmdCopyBuffer = vkCmdCopyBuffer;
	functions.vkCreateBuffer = vkCreateBuffer;
	functions.vkCreateImage = vkCreateImage;
	functions.vkDestroyBuffer = vkDestroyBuffer;
	functions.vkDestroyImage = vkDestroyImage;
	functions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
	functions.vkFreeMemory = vkFreeMemory;
	functions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;

	functions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
	functions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;
	functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
	functions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;

	functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	functions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	functions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	functions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
	functions.vkMapMemory = vkMapMemory;
	functions.vkUnmapMemory = vkUnmapMemory;

	functions.vkBindBufferMemory2KHR = vkBindBufferMemory2;
	functions.vkBindImageMemory2KHR = vkBindImageMemory2;
	functions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
	functions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2;
	functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2;

	allocatorInfo.pVulkanFunctions = &functions;

	vmaCreateAllocator(&allocatorInfo, &gVMAallocator);
}

static void DestroyDevice()
{
	if (gDevice)
	{
		vkDeviceWaitIdle(gDevice);
		llri::swapchain::destroy();
		vkDestroyDevice(gDevice, gGlobalAllocationsCallbacks);

		gDevice = nullptr;

	}	
}

static void DestroyInstance()
{
	vkDestroyDebugUtilsMessengerEXT(gInstance, gVulkanDebugMessenger, gGlobalAllocationsCallbacks);
	vkDestroyInstance(gInstance, gGlobalAllocationsCallbacks);
}

bool llri::initialize()
{
	mercury::write_log_message("LowLevel Rendering Interface Initialize: VULKAN");
    LoadVK_Library();
	InitializeInstance();
	ChoosePhysicalDevice();
	CreateDevice();

	return true;
}

void llri::shutdown()
{

	DestroyDevice();
	DestroyInstance();
    ShutdownVK_Library();
	mercury::write_log_message("LowLevel Rendering Interface Shutdown: VULKAN");
}

bool llri::update()
{
	static void* nativeWindowHandle = nullptr;
	vkDeviceWaitIdle(gDevice);

	if (nativeWindowHandle != platform::getMainWindowHandle())
	{
		nativeWindowHandle = platform::getMainWindowHandle();

		if (nativeWindowHandle == nullptr)
		{
			llri::swapchain::destroy();
		}
		else
		{
			llri::swapchain::create(nativeWindowHandle);
		}		
	}

	if (nativeWindowHandle != nullptr)
	{
		//do render

		llri::swapchain::update();
	}

	return true;
}
