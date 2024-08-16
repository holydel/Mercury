#pragma once
//configure engine

//uncomment one for configure graphics API

#define MERCURY_GRAPHICS_API_VULKAN
//#define MERCURY_GRAPHICS_API_D3D12
//#define MERCURY_GRAPHICS_API_METAL
//#define MERCURY_GRAPHICS_API_WEBGPU

#define MERCURY_XR_API_OPENXR
//#define MERCURY_XR_API_APPLE_VISION

#ifdef _WIN32
#define MERCURY_PLATFORM_WINDOWS
#endif

#ifdef __EMSCRIPTEN__
#define MERCURY_PLATFORM_EMSCRIPTEN
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

#ifdef MERCURY_PLATFORM_WINDOWS
#include <xmmintrin.h>
#endif

#ifdef MERCURY_PLATFORM_ANDROID
#include <arm_neon.h>
#endif

namespace mercury
{
	typedef unsigned	long long	u64;
	typedef signed		long long	i64;
	typedef unsigned	int			u32;
	typedef	signed		int			i32;
	typedef unsigned	short		u16;
	typedef	signed		short		i16;
	typedef unsigned	char		u8;
	typedef	signed		char		i8;

	typedef float					f32;
	typedef double					f64;

	//TODO: if supported f16
	typedef	unsigned short			f16;

#ifdef MERCURY_PLATFORM_WINDOWS
	typedef __m128					f32v4;
#endif

#ifdef MERCURY_PLATFORM_ANDROID
	typedef float32x4_t				f32v4;
#endif

	unsigned long long operator ""_KB(unsigned long long v);

	unsigned long long operator ""_MB(unsigned long long v);

	unsigned long long operator ""_GB(unsigned long long v);

	struct Handle32
	{
		u32 handle = 0xFFFFFFFFu;
		operator bool() const
		{
			return handle != 0xFFFFFFFFu;
		}
	};

	struct Handle64
	{
		u64 handle = 0xFFFFFFFFFFFFFFFFu;
		operator bool() const
		{
			return handle != 0xFFFFFFFFFFFFFFFFu;
		}
	};

	struct Handle16
	{
		u64 handle = 0xFFFFu;
		operator bool() const
		{
			return handle != 0xFFFFu;
		}
	};

	struct Handle8
	{
		u8 handle = 0xFFu;
		operator bool() const
		{
			return handle != 0xFFu;
		}
	};
}

