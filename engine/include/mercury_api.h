#pragma once
//configure engine

//uncomment one for configure graphics API

//

#define MERCURY_SOUND_API_MINIAUDIO
//#define MERCURY_SOUND_API_FMOD
//#define MERCURY_SOUND_API_WWISE

#define MERCURY_PHYSICS_API_BULLET
//#define MERCURY_PHYSICS_API_PHYSX

//#define MERCURY_XR_API_OPENXR
//#define MERCURY_XR_API_APPLE_VISION

#if defined(MERCURY_XR_API_OPENXR) || defined(MERCURY_XR_API_APPLE_VISION)
#define MERCURY_USE_XR
#endif

#ifdef _WIN32
#define MERCURY_PLATFORM_WINDOWS
//#define MERCURY_GRAPHICS_API_D3D12
#define MERCURY_GRAPHICS_API_VULKAN
#endif

#ifdef __EMSCRIPTEN__
#define MERCURY_PLATFORM_EMSCRIPTEN
#define MERCURY_GRAPHICS_API_WEBGPU
#endif

#ifdef __ANDROID__
#define MERCURY_PLATFORM_ANDROID
#define MERCURY_GRAPHICS_API_VULKAN
#else
#ifdef __linux__
#define MERCURY_PLATFORM_LINUX
#define MERCURY_GRAPHICS_API_VULKAN
#endif
#endif


#ifdef __APPLE__
#define MERCURY_PLATFORM_MACOS
#define MERCURY_GRAPHICS_API_METAL
//#define MERCURY_GRAPHICS_API_VULKAN
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

//Profilers
//Enable IO profiler (also enabled sound IO profilers if possible)
//#define MERCURY_IO_PROFILER
//Enable CPU profiler (also enabled sound CPU profilers if possible)
//#define MERCURY_CPU_PROFILER
//Enable GPU profiler (also enabled tools profilers)
//#define MERCURY_GPU_PROFILER
//Enable CPU profiler (also enabled sound MEMORY profilers if possible)
//#define MERCURY_MEMORY_PROFILER

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
		static constexpr u32 InvalidValue = 0xFFFFFFFFu;

		u32 handle = InvalidValue;

		bool isValid() const
		{
			return handle != InvalidValue;
		}
		u32 Value()
		{
			return handle;
		}
	};

	struct Handle64
	{
		static constexpr u64 InvalidValue = 0xFFFFFFFFFFFFFFFFu;

		u64 handle = InvalidValue;

		bool isValid() const
		{
			return handle != InvalidValue;
		}

		u64 Value()
		{
			return handle;
		}
	};

	struct Handle16
	{
		static constexpr u16 InvalidValue = 0xFFFFu;

		u16 handle = InvalidValue;

		bool isValid() const
		{
			return handle != InvalidValue;
		}

		u16 Value()
		{
			return handle;
		}
	};

	struct Handle8
	{
		static constexpr u8 InvalidValue = 0xFFu;

		u8 handle = InvalidValue;

		bool isValid() const
		{
			return handle != InvalidValue;
		}

		u8 Value()
		{
			return handle;
		}
	};
}

