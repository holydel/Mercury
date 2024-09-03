#include "vk_llri.h"

#ifdef MERCURY_GRAPHICS_API_VULKAN
#include <cstring>
#include "vk_utils.h"

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
uint32_t gMainQueueIndex = 0;

VkCommandPool gOneTimeSubmitCommandPool;
VkCommandBuffer gCurrentCommandBuffer = nullptr;

std::vector<VkShaderModule> gAllShaderModules;
std::vector<VkPipeline> gAllPSOs;
std::vector<VkPipelineLayout> gAllLayouts;

std::vector<VkBuffer> gAllBuffers;
std::vector<BufferMeta> gAllBuffersMeta;



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
	instance_extender.TryAddExtension(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#endif

	instance_extender.TryAddExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.flags = 0;
	debugCreateInfo.pNext = nullptr;
	debugCreateInfo.pUserData = nullptr;
	debugCreateInfo.pfnUserCallback = &debugCallback;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;


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

	if (vkGetPhysicalDeviceProperties2)
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

	VkPhysicalDeviceMemoryProperties memProps = {};
	vkGetPhysicalDeviceMemoryProperties(gPhysicalDevice, &memProps);

	for (int i = 0; i < memProps.memoryHeapCount; ++i)
	{
		mercury::write_log_message("HEAP(%d) SIZE: %d MB %s", i, (memProps.memoryHeaps[i].size / (1024 * 1024)),
			memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "DEVICE" : "HOST");
	}

	for (int i = 0; i < memProps.memoryTypeCount; ++i)
	{
		std::string desc = "MEM TYPE (" + std::to_string(i) + ") HEAP: " + std::to_string(memProps.memoryTypes[i].heapIndex);

		//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x00000001,
		//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000002,
		//	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00000004,
		//	VK_MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00000008,
		//	VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000010,
		//	VK_MEMORY_PROPERTY_PROTECTED_BIT = 0x00000020,
		//	VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD = 0x00000040,
		//	VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD = 0x00000080,
		//	VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV = 0x00000100,

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			desc += "| DEVICE_LOCAL_BIT";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			desc += "| HOST_VISIBLE_BIT";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			desc += "| HOST_COHERENT_BIT";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
			desc += "| HOST_CACHED_BIT";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
			desc += "| LAZILY_ALLOCATED_BIT";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT)
			desc += "| PROTECTED_BIT";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD)
			desc += "| DEVICE_COHERENT_BIT_AMD";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD)
			desc += "| DEVICE_UNCACHED_BIT_AMD";

		if (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV)
			desc += "| RDMA_CAPABLE_BIT_NV";

		mercury::write_log_message(desc.c_str());
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

	device_extender.TryAddExtension(VK_NV_MEMORY_DECOMPRESSION_EXTENSION_NAME);
	device_extender.TryAddExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	device_extender.TryAddExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
	
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

	VkPhysicalDeviceBufferDeviceAddressFeatures pfeture = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES };
	pfeture.bufferDeviceAddress = true;

	VkPhysicalDeviceFeatures2 enabledFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	enabledFeatures.pNext = &pfeture;

	deviceCreateInfo.pNext = &enabledFeatures;
	VK_CALL(vkCreateDevice(gPhysicalDevice, &deviceCreateInfo, gGlobalAllocationsCallbacks, &gDevice));


	LoadVkDeviceLevelFuncs(gDevice);

	vkGetDeviceQueue(gDevice, 0, 0, &gMainQueue);

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = gPhysicalDevice;
	allocatorInfo.device = gDevice;
	allocatorInfo.instance = gInstance;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

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

	VkCommandPoolCreateInfo poolInfo;
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.queueFamilyIndex = 0;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VK_CALL(vkCreateCommandPool(gDevice, &poolInfo, gGlobalAllocationsCallbacks, &gOneTimeSubmitCommandPool));
}

static void DestroyDevice()
{
	if (gDevice)
	{
		VK_CALL(vkDeviceWaitIdle(gDevice));
		llri::swapchain::destroy();
		vkDestroyCommandPool(gDevice, gOneTimeSubmitCommandPool, gGlobalAllocationsCallbacks);
		vkDestroyDevice(gDevice, gGlobalAllocationsCallbacks);

		gDevice = nullptr;

	}
}

static void DestroyInstance()
{
	if (gVulkanDebugMessenger)
	{
		vkDestroyDebugUtilsMessengerEXT(gInstance, gVulkanDebugMessenger, gGlobalAllocationsCallbacks);
		gVulkanDebugMessenger = nullptr;
	}

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
	return true;
}


llri::CommandList llri::BeginOneTimeSubmitCommandList()
{
	VkCommandBuffer cbuff = VK_NULL_HANDLE;

	VkCommandBufferAllocateInfo allocCmdBufferForFrame;
	allocCmdBufferForFrame.commandBufferCount = 1;
	allocCmdBufferForFrame.commandPool = gOneTimeSubmitCommandPool;
	allocCmdBufferForFrame.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocCmdBufferForFrame.pNext = nullptr;
	allocCmdBufferForFrame.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VK_CALL(vkAllocateCommandBuffers(gDevice, &allocCmdBufferForFrame, &cbuff));

	VkCommandBufferBeginInfo cbuffBegin;
	cbuffBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cbuffBegin.pNext = nullptr;
	cbuffBegin.pInheritanceInfo = nullptr;
	cbuffBegin.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CALL(vkBeginCommandBuffer(cbuff, &cbuffBegin));

	llri::CommandList clist;
	clist.impl = cbuff;
	return clist;
}

void llri::EndOneTimeSubmitCommandList(llri::CommandList clist)
{
	VkCommandBuffer cbuff = static_cast<VkCommandBuffer>(clist.impl);

	VK_CALL(vkEndCommandBuffer(cbuff));

	VkSubmitInfo sbmtInfo;
	sbmtInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	sbmtInfo.pNext = nullptr;
	sbmtInfo.commandBufferCount = 1;
	sbmtInfo.pCommandBuffers = &cbuff;
	sbmtInfo.pSignalSemaphores = nullptr;
	sbmtInfo.signalSemaphoreCount = 0;
	sbmtInfo.pWaitDstStageMask = 0;
	sbmtInfo.pWaitSemaphores = nullptr;
	sbmtInfo.waitSemaphoreCount = 0;


	VK_CALL(vkQueueSubmit(gMainQueue, 1, &sbmtInfo, nullptr)); //TODO: fence to free cmdBuff
}


mercury::Shader llri::create_shader_from_bytecode(mercury::Shader::ByteCode bc)
{
	VkShaderModuleCreateInfo cinfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	cinfo.codeSize = bc.size;
	cinfo.pCode = reinterpret_cast<u32*>(bc.data);

	VkShaderModule module = nullptr;
	VK_CALL(vkCreateShaderModule(gDevice, &cinfo, gGlobalAllocationsCallbacks, &module));

	gAllShaderModules.emplace_back(module);
	return mercury::Shader{ (u32)gAllShaderModules.size() - 1 };
}

void FillShaderStageIfNeeded(std::vector<VkPipelineShaderStageCreateInfo>& allStages, VkShaderStageFlagBits stageIN, mercury::Shader shader)
{
	const char* entryPoint = "main";

	if (shader.isValid())
	{
		auto& stage = allStages.emplace_back();
		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage.flags = 0;
		stage.pName = entryPoint;
		stage.pSpecializationInfo = nullptr;
		stage.stage = stageIN;
		stage.module = gAllShaderModules[shader.handle];
	}
}

mercury::Material llri::create_material(mercury::Material::Desc desc)
{
	VkPipelineLayout layout;

	VkGraphicsPipelineCreateInfo cinfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

	{
		VkPipelineLayoutCreateInfo cinfo{};
		cinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		VkPushConstantRange range;
		range.size = 128;
		range.offset = 0;
		range.stageFlags = VK_SHADER_STAGE_ALL;

		cinfo.pushConstantRangeCount = 1;
		cinfo.pPushConstantRanges = &range;

		vkCreatePipelineLayout(gDevice, &cinfo, gGlobalAllocationsCallbacks, &layout);
	}

	cinfo.layout = layout;
	VkPipelineVertexInputStateCreateInfo pipVertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	VkPipelineInputAssemblyStateCreateInfo pipInputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	VkPipelineTessellationStateCreateInfo pipTessellationState = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
	VkPipelineViewportStateCreateInfo pipViewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	VkPipelineRasterizationStateCreateInfo pipRasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	VkPipelineMultisampleStateCreateInfo pipMultisamplingState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	VkPipelineDepthStencilStateCreateInfo pipDepthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	VkPipelineColorBlendStateCreateInfo pipColorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	VkPipelineDynamicStateCreateInfo pipDynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };

	VkPipelineColorBlendAttachmentState bstate = {};

	std::vector<VkVertexInputBindingDescription> vertexInputBindDescription;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttribDescription;

	int offsetsPerBinding[8] = { 0 };

	for (auto& vi : desc.vertexInput.inputs)
	{
		u32 vertexSize = 0;

		VkVertexInputAttributeDescription attrib;

		attrib.location = vi.location;
		attrib.binding = vi.bindingPoint;


		attrib.format = vk_utils::GetFormatFromMercuryFormat(vi.fmt);
		attrib.offset = offsetsPerBinding[attrib.binding];
		int attribSize = mercury::utils::GetFormatSizeBits(vi.fmt) / 8;

		offsetsPerBinding[attrib.binding] += attribSize;
		vertexInputAttribDescription.push_back(attrib);
	}

	for (int i = 0; i < 8; ++i)
	{
		if (offsetsPerBinding[i] > 0)
		{
			VkVertexInputBindingDescription vBinding = {};
			vBinding.binding = i;
			vBinding.stride = offsetsPerBinding[i];
			vBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			vertexInputBindDescription.push_back(vBinding);
		}
	}

	pipVertexInputState.pVertexBindingDescriptions = vertexInputBindDescription.data();
	pipVertexInputState.pVertexAttributeDescriptions = vertexInputAttribDescription.data();
	pipVertexInputState.vertexBindingDescriptionCount = (u32)vertexInputBindDescription.size();
	pipVertexInputState.vertexAttributeDescriptionCount = (u32)vertexInputAttribDescription.size();

	bstate.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
	bstate.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
	bstate.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
	bstate.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;

	bstate.colorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
	bstate.alphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
	bstate.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;

	bstate.blendEnable = false;
	bstate.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
	bstate.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
	bstate.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
	bstate.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;

	pipInputAssemblyState.topology = vk_utils::GetTopologyFromMercuryTopology(desc.topology);

	pipColorBlendState.attachmentCount = 1;
	pipColorBlendState.pAttachments = &bstate;
	pipColorBlendState.logicOp = VK_LOGIC_OP_COPY;

	std::vector<VkPipelineShaderStageCreateInfo> allStages;
	FillShaderStageIfNeeded(allStages, VK_SHADER_STAGE_VERTEX_BIT, desc.vertexShader);
	FillShaderStageIfNeeded(allStages, VK_SHADER_STAGE_FRAGMENT_BIT, desc.fragmentShader);

	pipRasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	pipRasterizationState.cullMode = VK_CULL_MODE_BACK_BIT; //TODO: temporal fix
	pipRasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipRasterizationState.lineWidth = 1.0f;

	VkRect2D viewrect;
	viewrect.extent.width = 1280;
	viewrect.extent.height = 720;
	viewrect.offset = { 0,0 };

	VkViewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)1280;
	viewport.height = (float)720;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	pipViewportState.viewportCount = 1;
	pipViewportState.pViewports = &viewport;
	pipViewportState.scissorCount = 1;
	pipViewportState.pScissors = &viewrect;

	pipMultisamplingState.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

	std::vector<VkDynamicState> all_dinamic_states;

	all_dinamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	all_dinamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);

	pipDynamicState.pDynamicStates = all_dinamic_states.data();
	pipDynamicState.dynamicStateCount = (u32)all_dinamic_states.size();

	cinfo.pStages = allStages.data();
	cinfo.stageCount = (u32)allStages.size();

	cinfo.pRasterizationState = &pipRasterizationState;
	cinfo.pColorBlendState = &pipColorBlendState;
	cinfo.pViewportState = &pipViewportState;
	cinfo.pDepthStencilState = &pipDepthStencilState;
	cinfo.pMultisampleState = &pipMultisamplingState;
	cinfo.pVertexInputState = &pipVertexInputState;
	cinfo.pInputAssemblyState = &pipInputAssemblyState;
	cinfo.pDynamicState = &pipDynamicState;

	cinfo.renderPass = gRenderPass;

	VkPipeline pipeline = nullptr;
	VK_CALL(vkCreateGraphicsPipelines(gDevice, nullptr /*TODO: PipelineCache*/, 1, &cinfo, gGlobalAllocationsCallbacks, &pipeline));


	gAllPSOs.emplace_back(pipeline);
	gAllLayouts.emplace_back(layout);

	return mercury::Material{ (u32)gAllPSOs.size() - 1 };
}

mercury::Buffer llri::create_buffer(mercury::u64 size, mercury::Buffer::HeapType heapType, mercury::Buffer::BufferType btype)
{
	VkBuffer buffer = VK_NULL_HANDLE;

	//| VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
	VkBufferCreateInfo cinfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	cinfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	if (heapType == mercury::Buffer::HeapType::DEFAULT)
	{
		cinfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	}

	if (heapType == mercury::Buffer::HeapType::UPLOAD)
	{
		cinfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	if (btype == mercury::Buffer::BufferType::VERTEX)
	{
		cinfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}

	if (btype == mercury::Buffer::BufferType::INDEX)
	{
		cinfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}

	if (btype == mercury::Buffer::BufferType::UNIFORM)
	{
		cinfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}

	if (btype == mercury::Buffer::BufferType::STORAGE)
	{
		cinfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}
	cinfo.size = size;
	cinfo.pQueueFamilyIndices = &gMainQueueIndex;
	cinfo.queueFamilyIndexCount = 1;

	auto& meta = gAllBuffersMeta.emplace_back();

	if (size < 2'000'000'000)
	{
		VmaAllocationCreateInfo acinfo = {};
		acinfo.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO;

		acinfo.flags = 0;
		
		if (heapType == mercury::Buffer::HeapType::UPLOAD)
		{
			acinfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

			acinfo.memoryTypeBits = 8;
			//acinfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		

		if (heapType != mercury::Buffer::HeapType::DEFAULT)
		{
			acinfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
			
		VK_CALL(vmaCreateBuffer(gVMAallocator, &cinfo, &acinfo, &buffer, &meta.allocation, &meta.allocInfo));

		meta.mappedPointer = meta.allocInfo.pMappedData;
	}
	else
	{
		vkCreateBuffer(gDevice, &cinfo, gGlobalAllocationsCallbacks, &buffer);

		VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocInfo.memoryTypeIndex = 3;

		VkMemoryRequirements memReq = {};

		vkGetBufferMemoryRequirements(gDevice, buffer, &memReq);

		allocInfo.allocationSize = memReq.size;

		auto res = vkAllocateMemory(gDevice, &allocInfo, gGlobalAllocationsCallbacks, &meta.dedicatedMemory);

		int a = 52;
	}

	//VkBufferDeviceAddressInfo info = {VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
	//info.buffer = buffer;
	//
	//meta.deviceAddress = vkGetBufferDeviceAddress(gDevice, &info);

	gAllBuffers.emplace_back(buffer);

	return mercury::Buffer{ (u32)gAllBuffers.size() - 1 };
}

void* llri::buffer_mapped_data(mercury::u32 handle)
{
	return gAllBuffersMeta[handle].mappedPointer;
}
#endif