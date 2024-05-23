// dear imgui: Platform Binding for X11 using xcb
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan..)
// This works as is with Vulkan. For OpenGL using GLX, you need a hybrid
// of XLib and xcb using the X11/Xlib-xcb.h header. Use XLib to create the
// GLX context, then use functions in Xlib-xcb.h to convert the XLib
// structures to xcb, which you can then pass unmodified here.
// Requires libxcb, libxcb-xfixes and libxcb-keysyms1

// Implemented features:
//  [ ] Platform: Clipboard support
//  [ ] Platform: Mouse cursor shape and visibility.
//  [X] Platform: Keyboard arrays indexed using XK symbols, e.g. ImGui::IsKeyPressed(XK_space).
//  [ ] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.

#include "imgui.h"
#include <X11/keysym.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

#include <time.h>
#include <stdio.h>

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
// 2020-07-30 Initial implementation

// X11 Data
static xcb_connection_t    *g_Connection;
static xcb_drawable_t      *g_Win;
static xcb_key_symbols_t   *g_KeySyms;

static timespec             g_LastTime;
static timespec             g_CurrentTime;

// Functions

bool    ImGui_ImplX11_Init(xcb_connection_t *connection, xcb_drawable_t *win)
{
    g_Connection = connection;
    g_Win = win;
    clock_gettime(CLOCK_MONOTONIC_RAW, &g_LastTime);

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_x11";

    // Set initial display size
    xcb_generic_error_t *x_Err = nullptr;
    xcb_get_geometry_reply_t *resp = xcb_get_geometry_reply(g_Connection, xcb_get_geometry(g_Connection, *g_Win), &x_Err);
    if(x_Err)
        return false;
    io.DisplaySize = ImVec2(resp->width, resp->height);

    // Get the current key map
    g_KeySyms = xcb_key_symbols_alloc(connection);

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    // X Keyboard non-latin syms have the top high bits set.
    // ImGui enforces the lookup values between 0..512.
    // Therefore we have to remove the high bits to pass this check.
    // There are some unusual key symbols in the 0xFE00 and 0xFD00 range.
    // If you really want to support those check for that in your own xcb event handler.
    io.KeyMap[ImGuiKey_Tab] = XK_Tab - 0xFF00;
    io.KeyMap[ImGuiKey_LeftArrow] = XK_Left - 0xFF00;
    io.KeyMap[ImGuiKey_RightArrow] = XK_Right - 0xFF00;
    io.KeyMap[ImGuiKey_UpArrow] = XK_Up - 0xFF00;
    io.KeyMap[ImGuiKey_DownArrow] = XK_Down - 0xFF00;
    io.KeyMap[ImGuiKey_PageUp] = XK_Page_Up - 0xFF00;
    io.KeyMap[ImGuiKey_PageDown] = XK_Page_Up - 0xFF00;
    io.KeyMap[ImGuiKey_Home] = XK_Home - 0xFF00;
    io.KeyMap[ImGuiKey_End] = XK_End - 0xFF00;
    io.KeyMap[ImGuiKey_Insert] = XK_Insert - 0xFF00;
    io.KeyMap[ImGuiKey_Delete] = XK_Delete - 0xFF00;
    io.KeyMap[ImGuiKey_Backspace] = XK_BackSpace - 0xFF00;
    io.KeyMap[ImGuiKey_Space] = XK_space;
    io.KeyMap[ImGuiKey_Enter] = XK_Return - 0xFF00;
    io.KeyMap[ImGuiKey_Escape] = XK_Escape - 0xFF00;
    //io.KeyMap[ImGuiKey_KeyPadEnter] = XK_KP_Enter - 0xFF00;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    return true;
}

void    ImGui_ImplX11_Shutdown()
{
    xcb_key_symbols_free(g_KeySyms);
    xcb_flush(g_Connection);
    xcb_disconnect(g_Connection);
}

void    ImGui_ImplX11_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup time step
    clock_gettime(CLOCK_MONOTONIC_RAW, &g_CurrentTime);
    io.DeltaTime = (g_CurrentTime.tv_sec - g_LastTime.tv_sec) +
        ((g_CurrentTime.tv_nsec - g_LastTime.tv_nsec) / 1000000000.0f);
    g_LastTime = g_CurrentTime;
}

// X11 xcb message handler (process X11 mouse/keyboard inputs, etc.)
// Call from your application's message handler. Returns true if
// ImGui processed the event. You can use this for checking if you
// need further processing for the event.
bool ImGui_ImplX11_Event(xcb_generic_event_t *event)
{
    if (ImGui::GetCurrentContext() == NULL)
        return false;

    ImGuiIO& io = ImGui::GetIO();
    switch (event->response_type & ~0x80) {
    case XCB_MOTION_NOTIFY: {
        xcb_motion_notify_event_t *e = (xcb_motion_notify_event_t*)event;
        io.MousePos = ImVec2(e->event_x, e->event_y);
        return true;
    }
    case XCB_KEY_PRESS: {
        xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        xcb_keysym_t k = xcb_key_press_lookup_keysym(g_KeySyms, e, 0);
        if(k < 0xFF) { // latin-1 range
            io.AddInputCharacter(k);
            io.KeysDown[k] = 1;
        }
        else if(k >= 0x1000100 && k <= 0x110ffff) // utf range
            io.AddInputCharacterUTF16(k);
        else
            io.KeysDown[k - 0xFF00] = 1;

        return true;
    }
    case XCB_KEY_RELEASE: {
        xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        xcb_keysym_t k = xcb_key_press_lookup_keysym(g_KeySyms, e, 0);
        if(k < 0xFF)
            io.KeysDown[k] = 0;
        else
            io.KeysDown[k - 0xFF00] = 0;

        return true;
    }
    case XCB_BUTTON_PRESS: {
        xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
        // Get exact coords of the event. It may be separate from the mouse cursor.
        // e.g. touch input
        io.MousePos = ImVec2(e->event_x, e->event_y);
        // X decided button 4 is mwheel up and 5 is mwheel down
        if(e->detail == 4)
            io.MouseWheel += 1.0;
        else if(e->detail == 5)
            io.MouseWheel -= 1.0;
        else
            io.MouseDown[e->detail - 1] = true;
        return true;
    }
    case XCB_BUTTON_RELEASE: {
        xcb_button_release_event_t *e = (xcb_button_release_event_t *)event;
        io.MousePos = ImVec2(e->event_x, e->event_y);
        if(e->detail != 4 || e->detail != 5)
            io.MouseDown[e->detail - 1] = false;
        return true;
    }
    }
    return false;
}