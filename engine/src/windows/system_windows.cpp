#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_WINDOWS
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

#define NOMINMAX

#include <Windows.h>
#include <ShlObj.h>
#include <intrin.h>
#include <map>
#include <Psapi.h>

#include "../application.h"
#include "../platform.h"

using namespace mercury;

HWND gMainWindow = nullptr;
HINSTANCE gWinSystemInstance = nullptr;

void platformInitialize()
{
	timeBeginPeriod(1);
	//SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
}

void platformShutdown()
{
	timeEndPeriod(1);
}

//console main
int main()
{
	gWinSystemInstance = GetModuleHandle(nullptr);
	ApplicationRun();
	return 0;
}

//desktop main
int WINAPI WinMain([[maybe_unused]] HINSTANCE hInstance,    // HANDLE TO AN INSTANCE.  This is the "handle" to YOUR PROGRAM ITSELF.
	[[maybe_unused]] HINSTANCE hPrevInstance,// USELESS on modern windows (totally ignore hPrevInstance)
	[[maybe_unused]] LPSTR szCmdLine,        // Command line arguments.  similar to argv in standard C programs
	[[maybe_unused]] int iCmdShow)          // Start window maximized, minimized, etc.
{
	gWinSystemInstance = hInstance;
	ApplicationRun();
	return 0;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

constexpr const wchar_t* winClassName = L"MercuryWindow";

void platformCreateMainWindow()
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

void platformDestroyMainWindow()
{
	::DestroyWindow(gMainWindow);
	::UnregisterClassW(winClassName, gWinSystemInstance);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
	}
	break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProcW(hwnd, message, wParam, lParam);
	}

	return 0;
}

void platformUpdate()
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



int utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, static_cast<int>(strlen(utf8Str)), outBuff, buffLen);
}

int wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return ::WideCharToMultiByte(CP_UTF8, 0, wideStr, static_cast<int>(wcslen(wideStr)), outBuff, buffLen,nullptr, nullptr);
}

#endif
