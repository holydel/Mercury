//configure engine

//uncomment one for configure graphics API

#define MERCURY_GRAPHICS_API_VULKAN
//#define MERCURY_GRAPHICS_API_D3D12
//#define MERCURY_GRAPHICS_API_METAL
//#define MERCURY_GRAPHICS_API_OPENGL


#ifdef _WIN32
#define MERCURY_PLATFORM_WINDOWS
#endif

#ifdef linux
#ifdef __ANDROID__
#define MERCURY_PLATFORM_ANDROID
#else
#define MERCURY_PLATFORM_LINUX
#endif
#endif



#ifdef __APPLE__
#define MERCURY_PLATFORM_MACOS
#endif