#pragma once
#include "../llvri.h"

#ifdef MERCURY_XR_API_OPENXR

bool llvri::initialize()
{
	return true;
}

void llvri::shutdown()
{

}

bool llvri::update()
{
	return true;
}


#endif