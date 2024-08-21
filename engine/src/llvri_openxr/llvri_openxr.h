#pragma once
#include "../llvri.h"

#ifdef MERCURY_XR_API_OPENXR

bool llvri::initialize();

void llvri::shutdown();

bool llvri::update();


#endif