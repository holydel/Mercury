#pragma once
//configure engine

//uncomment one for configure graphics API

#define MERCURY_GRAPHICS_API_VULKAN
//#define MERCURY_GRAPHICS_API_D3D12
//#define MERCURY_GRAPHICS_API_METAL
//#define MERCURY_GRAPHICS_API_OPENGL


#ifdef _WIN32
#define MERCURY_PLATFORM_WINDOWS
#endif

#ifdef __ANDROID__
#define MERCURY_PLATFORM_ANDROID
#else
#ifdef __linux__
#define MERCURY_PLATFORM_LINUX
#endif
#endif


#ifdef __APPLE__
#define MERCURY_PLATFORM_MACOS
#endif

#if defined(MERCURY_PLATFORM_ANDROID) || defined(MERCURY_PLATFORM_IOS)
#define MERCURY_MOBILE
#else
#define MERCURY_DESKTOP
#endif