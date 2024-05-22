#include "mercury_rendering.h"
#include "llri.h"
#include "swapchain.h"

using namespace mercury;


void rendering::SetBackGroundColor(float r, float g, float b, float a)
{
	llri::swapchain::setBackGroundColor(r, g, b, a);
}
