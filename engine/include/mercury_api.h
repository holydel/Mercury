#pragma once
//configure engine

//uncomment one for configure graphics API

//#define MERCURY_GRAPHICS_API_VULKAN
#define MERCURY_GRAPHICS_API_D3D12
//#define MERCURY_GRAPHICS_API_METAL


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
}