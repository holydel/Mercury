#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_WINDOWS
#include "windows/system_impl.cpp"
#endif

#ifdef MERCURY_PLATFORM_LINUX
#include "linux/system_impl.cpp"
#endif

#ifdef MERCURY_PLATFORM_ANDROID
#include "android/system_impl.cpp"
#endif

#ifdef MERCURY_PLATFORM_MACOS
#include "macos/system_impl.cpp"
#endif

#ifdef MERCURY_PLATFORM_IOS
#include "ios/system_impl.cpp"
#endif