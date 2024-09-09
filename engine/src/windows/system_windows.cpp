#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_WINDOWS
#include "mercury_log.h"

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <ShlObj.h>
#include <intrin.h>
#include <map>
#include <Psapi.h>
#include <cstdio>

#include "../application.h"
#include "../platform.h"
#include <thread>
#include <KnownFolders.h>
#include <shlobj.h>
#include "../swapchain.h"

#include <filesystem>
#include <json.hpp>
#include <fstream>

using namespace mercury;

HWND gMainWindow = nullptr;
HINSTANCE gWinSystemInstance = nullptr;

void platform::initialize()
{
	mercury::write_log_message("platform WINDOWS initialize");

	timeBeginPeriod(1);
	SetProcessDPIAware();
	//SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
}

void platform::shutdown()
{
	mercury::write_log_message("platform WINDOWS shutdown");

	timeEndPeriod(1);
}

std::thread* mainThread;

//console main
int main()
{
	gWinSystemInstance = GetModuleHandle(nullptr);
	mainThread = new std::thread(ApplicationRun);
	mainThread->join();
	return 0;
}

//desktop main
int WINAPI WinMain([[maybe_unused]] HINSTANCE hInstance,    // HANDLE TO AN INSTANCE.  This is the "handle" to YOUR PROGRAM ITSELF.
	[[maybe_unused]] HINSTANCE hPrevInstance,// USELESS on modern windows (totally ignore hPrevInstance)
	[[maybe_unused]] LPSTR szCmdLine,        // Command line arguments.  similar to argv in standard C programs
	[[maybe_unused]] int iCmdShow)          // Start window maximized, minimized, etc.
{
	gWinSystemInstance = hInstance;
	mainThread = new std::thread(ApplicationRun);
	mainThread->join();
	return 0;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

constexpr const wchar_t* winClassName = L"MercuryWindow";

void platform::createMainWindow()
{
	auto& config = gApplication->config.output;
	const char* utf8WinCaption = gApplication->config.GetWindowTitle();
	bool is_maximized = config.mode != WindowConfig::WindowMode::Windowed;

	WNDCLASSEXW windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = gWinSystemInstance;
	windowClass.hIcon = ::LoadIcon(gWinSystemInstance, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = winClassName;
	windowClass.hIconSm = ::LoadIcon(gWinSystemInstance, IDI_APPLICATION);

	static ATOM atom = ::RegisterClassExW(&windowClass);

	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect = { 0, 0, static_cast<LONG>(config.width), static_cast<LONG>(config.height) };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
	int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

	wchar_t winTitleBuff[256] = { 0 };
	utf8ToWide(utf8WinCaption, winTitleBuff,256);

	HWND hWnd = ::CreateWindowExW(
		0,
		winClassName,
		winTitleBuff,
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		gWinSystemInstance,
		nullptr
	);

	HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monitorInfo = {};
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	::GetMonitorInfo(hMonitor, &monitorInfo);

	if (is_maximized)
	{
		::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	else
	{
		::ShowWindow(hWnd, SW_SHOW);
	}

	gMainWindow = hWnd;
}

void platform::destroyMainWindow()
{
	gMainWindow = nullptr;
	::DestroyWindow(gMainWindow);
	::UnregisterClassW(winClassName, gWinSystemInstance);
}

void* platform::getMainWindowHandle()
{
	return static_cast<void*>(gMainWindow);
}

void* platform::getAppInstanceHandle()
{
	return static_cast<void*>(gWinSystemInstance);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_PAINT:
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

		switch (wParam)
		{
		case VK_ESCAPE:
			gApplication->OnApplicationClose();
			::PostQuitMessage(0);
			break;
        default:
			break;

		}
	}
	break;

	case WM_SYSCHAR:
		break;
		break;
	case WM_SIZE:
	{
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);

		llri::swapchain::resize(width, height);
	}
	break;
	case WM_DESTROY:
		gMainWindow = nullptr;
		gApplication->OnApplicationClose();
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProcW(hwnd, message, wParam, lParam);
	}

	return 0;
}

void platform::update()
{
	MSG msg = {};

	if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

//int xc_platform::UTF8ToWideChar(const char* utf8string, wchar_t* out_buff)
//{
//	int numCharsConverted = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, out_buff, 65536);
//	return numCharsConverted;
//}



int platform::utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, static_cast<int>(strlen(utf8Str))+1, outBuff, buffLen);
}

int platform::wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return ::WideCharToMultiByte(CP_UTF8, 0, wideStr, static_cast<int>(wcslen(wideStr))+1, outBuff, buffLen,nullptr, nullptr);
}

long platform::interlockedAdd32(volatile long* value, long add)
{
	return InterlockedAdd(value, add);
}

void* platform::alignedMalloc(int size, int align)
{
	return _aligned_malloc(size, align);
}

void platform::alignedFree(void* ptr)
{
	_aligned_free(ptr);
}

void* platform::loadSharedLibrary(const char* utf8libname)
{
	return LoadLibraryA(utf8libname);
}

bool platform::unloadSharedLibrary(void* library)
{
	return FreeLibrary(static_cast<HMODULE>(library));
}

void* platform::getFuncPtrImpl(void* library, const char* funcName)
{
	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(library), funcName));
}

void platform::fatalFail(const char* reason)
{
	mercury::write_log_message("%s\n", reason);
	DebugBreak();
}

const char* platform::getSharedLibraryFullFilename(void* libHandle)
{
    static char path[MAX_PATH];
    GetModuleFileNameA(reinterpret_cast<HMODULE>(libHandle), path, MAX_PATH);
    return path;
}

void platform::outputToDebugConsole(const char* text)
{
    printf("%s",text);
}

void platform::outputToLogFile(const char* text)
{

}

const char* platform::getApplicationDataPath()
{
	GUID guid;

	static char appPath[MAX_PATH] = {};

	PWSTR path;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

	wideToUtf8(path, appPath, MAX_PATH);

	CoTaskMemFree((LPVOID)path);

	return appPath;
}

void platform::setMainWindowTitle(const char *u8string)
{
	SetWindowTextA(gMainWindow, u8string);
}

#ifdef MERCURY_GRAPHICS_API_VULKAN
const char* platform::getVulkanLibraryPath()
{
	static const char* libName = u8"vulkan-1.dll";
	return libName;
}
#endif

#ifdef MERCURY_XR_API_OPENXR
const OpenXRRuntimeInfo& platform::getOpenXRRuntimeInfo()
{
	using namespace nlohmann;

	static OpenXRRuntimeInfo runtimeInfo;

	char data[MAX_PATH] = {};
	DWORD maxPath = MAX_PATH;

	auto res = RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\OpenXR\\1", "ActiveRuntime", RRF_RT_REG_SZ, nullptr /*type not required*/, data, &maxPath);
	if (res == ERROR_FILE_NOT_FOUND)
	{
		runtimeInfo = {};
		return runtimeInfo;
	}

	runtimeInfo.hasRuntime = true;

	std::filesystem::path jsonPath = data;

	std::ifstream f(jsonPath);
	json dataJson = json::parse(f);

	auto file_format_version = dataJson["file_format_version"];

	auto runtime = dataJson["runtime"];

	auto library_path = runtime["library_path"];
	auto name = runtime["name"];
	auto VALVE_runtime_is_steamvr = runtime["VALVE_runtime_is_steamvr"];

	if (file_format_version.is_string())
	{
		runtimeInfo.file_format_version = file_format_version.template get<std::string>();
	}

	if (library_path.is_string())
	{
		runtimeInfo.library_path = library_path.template get<std::string>();
		auto fullLibraryPath = std::filesystem::weakly_canonical(jsonPath.remove_filename() / std::filesystem::path(runtimeInfo.library_path));

		runtimeInfo.library_fullpath = fullLibraryPath.u8string();
	}


	if (name.is_string())
	{
		runtimeInfo.name = name.template get<std::string>();
	}

	if (VALVE_runtime_is_steamvr.is_boolean())
	{
		runtimeInfo.isValveRuntime = VALVE_runtime_is_steamvr.template get<bool>();
	}

	return runtimeInfo;
}
#endif

#endif
