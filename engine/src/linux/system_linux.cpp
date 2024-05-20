#include <cstdlib>
#include <cstring>
#include <thread>

#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_LINUX
#include <xcb/xcb.h>

#include "../application.h"
#include "../platform.h"

using namespace mercury;

xcb_connection_t *gConnection = nullptr;
xcb_window_t gWindow;
const xcb_setup_t *gSetup;
xcb_screen_t *gScreen = nullptr;
xcb_atom_t gWmProtocols;
xcb_atom_t gWmDeleteWin;

void platformInitialize()
{
}

void platformShutdown()
{

}

//console main
int main()
{
	ApplicationRun();
	return 0;
}


void platformCreateMainWindow()
{
	auto& config = gApplication->config.output;
	const char* utf8WinCaption = gApplication->config.GetWindowTitle();

	/* Open the connection to the X server */
	gConnection = xcb_connect (NULL, NULL);


	/* Get the first screen */
	gSetup = xcb_get_setup (gConnection);
	xcb_screen_iterator_t   iter   = xcb_setup_roots_iterator (gSetup);
	gScreen = iter.data;

	uint32_t valueList[] = { gScreen->black_pixel, 0 };
	/* Create the window */
	gWindow = xcb_generate_id (gConnection);
	xcb_create_window (gConnection,                    /* Connection          */
					   XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
					   gWindow,                        /* window Id           */
					   gScreen->root,                  /* parent window       */
					   0, 0,                          /* x, y                */
					   750, 150,                      /* width, height       */
					   10,                            /* border_width        */
					   XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
					   gScreen->root_visual,           /* visual              */
					   XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, valueList );                     /* masks, not used yet */


	/* Map the window on the screen */
	xcb_map_window (gConnection, gWindow);

	xcb_change_property(
  gConnection,
  XCB_PROP_MODE_REPLACE,
  gWindow,
  XCB_ATOM_WM_NAME,
  XCB_ATOM_STRING,
  8,
  strlen(utf8WinCaption),
  utf8WinCaption);

	/* Make sure commands are sent before we pause so that the window gets shown */
	xcb_flush (gConnection);


}

void platformDestroyMainWindow()
{
	xcb_destroy_window(gConnection, gWindow);
	xcb_disconnect (gConnection);
}


void platformUpdate()
{
	using namespace std::chrono_literals;
	
	xcb_generic_event_t *event;
	xcb_client_message_event_t *cm;

		event = xcb_wait_for_event(gConnection);

		// switch (event->response_type & ~0x80) {
		// 	case XCB_CLIENT_MESSAGE: {
		// 		cm = (xcb_client_message_event_t *)event;
		//
		// 		if (cm->data.data32[0] == wmDeleteWin)
		// 			running = false;
		//
		// 		break;
		// 	}
		// }

		free(event);

	std::this_thread::sleep_for((1ms));
}

//int xc_platform::UTF8ToWideChar(const char* utf8string, wchar_t* out_buff)
//{
//	int numCharsConverted = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, out_buff, 65536);
//	return numCharsConverted;
//}



int utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

int wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

#endif
