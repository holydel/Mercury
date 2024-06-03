#include "mercury_api.h"

#ifdef MERCURY_GRAPHICS_API_VULKAN
#include "vk_swapchain.h"
#include "../platform.h"
#include "../mercury_imgui.h"

using namespace mercury;

static VkSurfaceKHR gSurface = nullptr;
static VkSwapchainKHR gSwapChain = nullptr;
static std::vector<VkSurfaceFormatKHR> support_formats;
static std::vector<VkPresentModeKHR> support_present_modes;
static VkSurfaceCapabilitiesKHR gSurfaceCaps;
static VkExtent2D gNativeExtent = { 0,0 };

VkRenderPass gRenderPass = nullptr;
static u32 gNumberOfSwapChainFrames = 0;
static std::vector<VkImage> gSwapChainImages;

static VkFormat outputFormat = VK_FORMAT_R8G8B8A8_SRGB;

static float backColorR=0.1f, backColorG=0.15, backColorB=0.22f, backColorA=1.0f;

static u32 gCurrentFrame = 0;

struct FrameObject
{
	VkImageView imageView;
	VkFramebuffer frameBuffer;
	VkFence fence;
	VkSemaphore acquireComplete;
	VkSemaphore renderingComplete;
	VkCommandPool cmdPool;
	VkCommandBuffer cmdBuffer;
};

static std::vector<FrameObject>			gFrameObjects;



void _obtainImages()
{
	VK_CALL(vkGetSwapchainImagesKHR(gDevice, gSwapChain, &gNumberOfSwapChainFrames, nullptr));

	gSwapChainImages.resize(gNumberOfSwapChainFrames);
	VK_CALL(vkGetSwapchainImagesKHR(gDevice, gSwapChain, &gNumberOfSwapChainFrames, gSwapChainImages.data()));
}

void _createFramebuffer()
{
	for (size_t i = 0; i < gNumberOfSwapChainFrames; i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = gSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = outputFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VK_CALL(vkCreateImageView(gDevice, &createInfo, nullptr, &gFrameObjects[i].imageView));
	}

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = outputFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = nullptr;
	subpass.pResolveAttachments = nullptr;

	VkAttachmentDescription attachments[] = { colorAttachment};

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency[2] = {};

	dependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency[0].dstSubpass = 0;
	dependency[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency[0].srcAccessMask = VK_ACCESS_NONE_KHR;
	dependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency[0].dependencyFlags = 0;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = dependency;

	VK_CALL(vkCreateRenderPass(gDevice, &renderPassInfo, nullptr, &gRenderPass));


	for (size_t i = 0; i < gNumberOfSwapChainFrames; i++) {

		VkImageView attachments[] = {
			gFrameObjects[i].imageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = gRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = gNativeExtent.width;
		framebufferInfo.height = gNativeExtent.height;
		framebufferInfo.layers = 1;

		VK_CALL(vkCreateFramebuffer(gDevice, &framebufferInfo, nullptr, &gFrameObjects[i].frameBuffer));
	}
}

void _recreateSwapChain()
{
	vkDeviceWaitIdle(gDevice);

	VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gPhysicalDevice, gSurface, &gSurfaceCaps));

	gNativeExtent = gSurfaceCaps.currentExtent;

	if (gSurfaceCaps.currentTransform == VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR
		|| gSurfaceCaps.currentTransform == VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR)
	{
		gNativeExtent.width = gSurfaceCaps.currentExtent.height;
		gNativeExtent.height = gSurfaceCaps.currentExtent.width;
	}

	{
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.surface = gSurface;

		createInfo.minImageCount = 3;
		createInfo.imageFormat = outputFormat;
		createInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR; //
		createInfo.imageExtent = gNativeExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
		createInfo.preTransform = gSurfaceCaps.currentTransform;
		//createInfo.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;// VK_PRESENT_MODE_FIFO_KHR; //VK_PRESENT_MODE_MAILBOX_KHR
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = gSwapChain;

		VK_CALL(vkCreateSwapchainKHR(gDevice, &createInfo, gGlobalAllocationsCallbacks, &gSwapChain));
	}

	_obtainImages();
	gFrameObjects.resize(gNumberOfSwapChainFrames);;

	_createFramebuffer();

	gCurrentFrame = 0;
}

bool llri::swapchain::create(void* nativeWindowHandle)
{

#ifdef MERCURY_PLATFORM_WINDOWS
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = static_cast<HWND>(nativeWindowHandle);
	createInfo.hinstance = static_cast<HINSTANCE>(mercury::platform::getAppInstanceHandle());

	VK_CALL(vkCreateWin32SurfaceKHR(gInstance, &createInfo, nullptr, &gSurface));
#endif

#ifdef MERCURY_PLATFORM_ANDROID
	VkAndroidSurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,nullptr };

	createInfo.flags = 0;
	createInfo.window = static_cast<ANativeWindow*>(nativeWindowHandle);

	vkCreateAndroidSurfaceKHR(gInstance, &createInfo, nullptr, &gSurface);
#endif

#ifdef MERCURY_PLATFORM_LINUX
	VkXcbSurfaceCreateInfoKHR createInfo {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
	createInfo.window = *((xcb_window_t*)nativeWindowHandle);
	createInfo.connection = (xcb_connection_t*)mercury::platform::getAppInstanceHandle();

	vkCreateXcbSurfaceKHR(gInstance, &createInfo, gGlobalAllocationsCallbacks, &gSurface);
#endif

#ifdef MERCURY_PLATFORM_MACOS
    VkMetalSurfaceCreateInfoEXT createInfo {VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT};
	createInfo.pLayer = static_cast<CAMetalLayer*>(nativeWindowHandle);

	vkCreateMetalSurfaceEXT(gInstance, &createInfo, gGlobalAllocationsCallbacks, &gSurface);
#endif
	VkBool32 is_supported = false;

	vkGetPhysicalDeviceSurfaceSupportKHR(gPhysicalDevice, 0, gSurface, &is_supported);

	if (!is_supported)
	{
		mercury::write_log_message("Swapchain surface doesn't support!");
		return false;
	}

	support_formats = EnumerateVulkanObjects(gPhysicalDevice, gSurface, vkGetPhysicalDeviceSurfaceFormatsKHR);
	support_present_modes = EnumerateVulkanObjects(gPhysicalDevice, gSurface, vkGetPhysicalDeviceSurfacePresentModesKHR);
	outputFormat = support_formats[0].format; //TODO: selection heuristics

	_recreateSwapChain();

	VkCommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = nullptr;
	cmdPoolInfo.queueFamilyIndex = 0;
	cmdPoolInfo.flags = 0;

	for (int i = 0; i < gNumberOfSwapChainFrames; ++i)
	{
		VkFenceCreateInfo fenceInfo;
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = nullptr;
		fenceInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

		VK_CALL(vkCreateFence(gDevice, &fenceInfo, nullptr, &gFrameObjects[i].fence));

		VkSemaphoreCreateInfo semInfo;
		semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semInfo.pNext = nullptr;
		semInfo.flags = 0;

		VK_CALL(vkCreateSemaphore(gDevice, &semInfo, nullptr, &gFrameObjects[i].acquireComplete));
		VK_CALL(vkCreateSemaphore(gDevice, &semInfo, nullptr, &gFrameObjects[i].renderingComplete));

		vkCreateCommandPool(gDevice, &cmdPoolInfo, gGlobalAllocationsCallbacks, &gFrameObjects[i].cmdPool);

		VkCommandBufferAllocateInfo allocCmdBufferForFrame;
		allocCmdBufferForFrame.commandBufferCount = 1;
		allocCmdBufferForFrame.commandPool = gFrameObjects[i].cmdPool;
		allocCmdBufferForFrame.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocCmdBufferForFrame.pNext = nullptr;
		allocCmdBufferForFrame.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		vkAllocateCommandBuffers(gDevice, &allocCmdBufferForFrame, &gFrameObjects[i].cmdBuffer);
	}

	imgui::initialize();
	return true;
}

bool llri::swapchain::destroy()
{
	if (gRenderPass != nullptr)
	{
		vkDestroyRenderPass(gDevice, gRenderPass, gGlobalAllocationsCallbacks);
		gRenderPass = nullptr;
	}

	imgui::shutdown();
	if (gSwapChain != nullptr)
	{
		vkDestroySwapchainKHR(gDevice, gSwapChain, gGlobalAllocationsCallbacks);
		gSwapChain = nullptr;
	}
	if (gSurface != nullptr)
	{
		vkDestroySurfaceKHR(gInstance, gSurface, gGlobalAllocationsCallbacks);
		gSurface = nullptr;
	}
	return true;
}

void llri::swapchain::setBackGroundColor(float r, float g, float b, float a)
{
	backColorR = r;
	backColorG = g;
	backColorB = b;
	backColorA = a;
}


bool llri::swapchain::update()
{
	u32 frameIndex = gCurrentFrame % 3;
	u32 presentedImageIndex = -1;
	const auto& frame = gFrameObjects[frameIndex];

	VkResult result = vkAcquireNextImageKHR(gDevice, gSwapChain, UINT64_MAX, frame.acquireComplete, VK_NULL_HANDLE, &presentedImageIndex);
	if (result != VK_SUCCESS)
	{
		_recreateSwapChain();
		return false;
	}

	vkWaitForFences(gDevice, 1, &frame.fence, VK_TRUE, UINT64_MAX); //UINT64_MAX
	vkResetFences(gDevice, 1, &frame.fence);

	//mercury::write_log_message("frameID: %d imageID: %d", frameIndex, presentedImageIndex);
	vkResetCommandPool(gDevice,frame.cmdPool,0);

	VkCommandBufferBeginInfo cbuffBegin;
	cbuffBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cbuffBegin.pNext = nullptr;
	cbuffBegin.pInheritanceInfo = nullptr;
	cbuffBegin.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(frame.cmdBuffer, &cbuffBegin);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = gRenderPass;
	renderPassInfo.framebuffer = frame.frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = gNativeExtent;

	VkClearValue clearColor[1];
	clearColor[0].color.float32[0] = backColorR;
	clearColor[0].color.float32[1] = backColorG;
	clearColor[0].color.float32[2] = backColorB;
	clearColor[0].color.float32[3] = backColorA;

	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = clearColor;

	vkCmdBeginRenderPass(frame.cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkRect2D scissorRect = {};
	scissorRect.extent = gNativeExtent;
	VkViewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)scissorRect.extent.width;
	viewport.height = (float)scissorRect.extent.height;
	viewport.minDepth = 0.0f; //TODO: revZ
	viewport.maxDepth = 1.0f;

	vkCmdSetScissor(frame.cmdBuffer, 0, 1, &scissorRect);
	vkCmdSetViewport(frame.cmdBuffer, 0, 1, &viewport);


	mercury::imgui::render(frame.cmdBuffer);

	vkCmdEndRenderPass(frame.cmdBuffer);

	vkEndCommandBuffer(frame.cmdBuffer);

	VkSubmitInfo sbmtInfo;
	sbmtInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	sbmtInfo.pNext = nullptr;
	sbmtInfo.commandBufferCount = 1;
	sbmtInfo.pCommandBuffers = &frame.cmdBuffer;


	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	sbmtInfo.pSignalSemaphores = &frame.renderingComplete;
	sbmtInfo.signalSemaphoreCount = 1;
	sbmtInfo.pWaitDstStageMask = &waitStage;
	sbmtInfo.pWaitSemaphores = &frame.acquireComplete;
	sbmtInfo.waitSemaphoreCount = 1;

	vkQueueSubmit(gMainQueue, 1, &sbmtInfo, frame.fence);

	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.pWaitSemaphores = &frame.renderingComplete;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.swapchainCount = 1;
	presentInfo.pImageIndices = &presentedImageIndex;
	presentInfo.pSwapchains = &gSwapChain;
	presentInfo.pNext = nullptr;

	auto res = vkQueuePresentKHR(gMainQueue, &presentInfo);
	if (res != VK_SUCCESS)
	{
		_recreateSwapChain();
		return false;
	}

	gCurrentFrame++;
	return true;
}
#endif