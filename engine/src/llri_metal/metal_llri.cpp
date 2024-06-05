#include "metal_llri.h"
#ifdef MERCURY_GRAPHICS_API_METAL
#include "../application.h"
#include "../platform.h"


#include "../swapchain.h"

MTL::Device *gDevice = nullptr;
MTL::CommandQueue *gCommandQueue = nullptr;

using namespace mercury;


bool llri::initialize()
{
    mercury::write_log_message("METAL LLRI initialize");
    mercury::write_log_message("MetalDevice: %s", gDevice->name()->cString(NS::StringEncoding::UTF8StringEncoding));
	return true;
}

void llri::shutdown()
{

}

bool llri::update()
{

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
#endif
