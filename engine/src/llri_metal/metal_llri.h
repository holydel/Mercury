#pragma once
#include "../llri.h"

#ifdef MERCURY_GRAPHICS_API_METAL
#include "mercury_log.h"
#include <system_error>
#include "../platform.h"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

extern MTL::Device *gDevice;
extern MTL::CommandQueue *gCommandQueue;
#endif