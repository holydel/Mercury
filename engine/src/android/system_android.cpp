#include "mercury_api.h"
#ifdef MERCURY_PLATFORM_ANDROID

#include "../application.h"

#include <stdio.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <string>
#include <thread>
#include <atomic>
using namespace std::chrono_literals;

std::atomic_bool is_running = false;
std::atomic_bool need_to_stop = false;



void platformInitialize()
{

}

void platformShutdown()
{

}

void platformUpdate()
{
	std::this_thread::sleep_for(1ms);
}

void MainLoop()
{
	is_running = true;

	gApplication->Initialize();

	while (gApplication->Update() && !need_to_stop)
	{
		platformUpdate();
	}

	gApplication->Shutdown();

	is_running = false;
}

std::thread* mainThread = nullptr;

/**
	 * NativeActivity has started.  See Java documentation for Activity.onStart()
	 * for more information.
	 */
void onStart(ANativeActivity* activity)
{
	mercury::log("onStart");
	//ANativeActivity_setWindowFlags(activity, ana, 0);

	if (mainThread == nullptr)
	{
		mainThread = new std::thread(MainLoop);
	}	
}

/**
 * NativeActivity has resumed.  See Java documentation for Activity.onResume()
 * for more information.
 */
void onResume(ANativeActivity* activity)
{
	mercury::log("onResume");
}

/**
 * Framework is asking NativeActivity to save its current instance state.
 * See Java documentation for Activity.onSaveInstanceState() for more
 * information.  The returned pointer needs to be created with malloc();
 * the framework will call free() on it for you.  You also must fill in
 * outSize with the number of bytes in the allocation.  Note that the
 * saved state will be persisted, so it can not contain any active
 * entities (pointers to memory, file descriptors, etc).
 */
void* onSaveInstanceState(ANativeActivity* activity, size_t* outSize)
{
	mercury::log("onSaveInstanceState");

	void* savedData = malloc(64);
	*outSize = 64;
	return savedData;
}

/**
 * NativeActivity has paused.  See Java documentation for Activity.onPause()
 * for more information.
 */
void onPause(ANativeActivity* activity)
{
	mercury::log("onPause");
}

/**
 * NativeActivity has stopped.  See Java documentation for Activity.onStop()
 * for more information.
 */
void onStop(ANativeActivity* activity)
{
	mercury::log("onStop");
}

/**
 * NativeActivity is being destroyed.  See Java documentation for Activity.onDestroy()
 * for more information.
 */
void onDestroy(ANativeActivity* activity)
{

	need_to_stop = true;

	while (is_running)
	{
		std::this_thread::sleep_for(16ms);
	}

	delete mainThread;

	mercury::log("onDestroy");
}

/**
 * Focus has changed in this NativeActivity's window.  This is often used,
 * for example, to pause a game when it loses input focus.
 */
void onWindowFocusChanged(ANativeActivity* activity, int hasFocus)
{
	mercury::log("onWindowFocusChanged");
}

/**
 * The drawing window for this native activity has been created.  You
 * can use the given native window object to start drawing.
 */
void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window)
{	
	mercury::log("onNativeWindowCreated");
}

/**
 * The drawing window for this native activity has been resized.  You should
 * retrieve the new size from the window and ensure that your rendering in
 * it now matches.
 */
void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window)
{
	mercury::log("onNativeWindowResized");
}

/**
 * The drawing window for this native activity needs to be redrawn.  To avoid
 * transient artifacts during screen changes (such resizing after rotation),
 * applications should not return from this function until they have finished
 * drawing their window in its current state.
 */
void onNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window)
{
	mercury::log("onNativeWindowRedrawNeeded");
}

/**
 * The drawing window for this native activity is going to be destroyed.
 * You MUST ensure that you do not touch the window object after returning
 * from this function: in the common case of drawing to the window from
 * another thread, that means the implementation of this callback must
 * properly synchronize with the other thread to stop its drawing before
 * returning from here.
 */
void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window)
{
	mercury::log("onNativeWindowDestroyed");
}
/**
 * The input queue for this native activity's window has been created.
 * You can use the given input queue to start retrieving input events.
 */
void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue)
{
	mercury::log("onInputQueueCreated");
}

/**
 * The input queue for this native activity's window is being destroyed.
 * You should no longer try to reference this object upon returning from this
 * function.
 */
void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue)
{
	mercury::log("onInputQueueDestroyed");
}

/**
 * The rectangle in the window in which content should be placed has changed.
 */
void onContentRectChanged(ANativeActivity* activity, const ARect* rect)
{
	mercury::log("onContentRectChanged");
}

/**
 * The current device AConfiguration has changed.  The new configuration can
 * be retrieved from assetManager.
 */
void onConfigurationChanged(ANativeActivity* activity)
{
	mercury::log("onConfigurationChanged");
}

/**
 * The system is running low on memory.  Use this callback to release
 * resources you do not need, to help the system avoid killing more
 * important processes.
 */
void onLowMemory(ANativeActivity* activity)
{
	mercury::log("onLowMemory");
}

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity * activity,
	void* savedState, size_t savedStateSize)
{
	activity->callbacks->onConfigurationChanged = onConfigurationChanged;
	activity->callbacks->onContentRectChanged = onContentRectChanged;
	activity->callbacks->onDestroy = onDestroy;
	activity->callbacks->onInputQueueCreated = onInputQueueCreated;
	activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
	activity->callbacks->onLowMemory = onLowMemory;
	activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
	activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
	activity->callbacks->onNativeWindowRedrawNeeded = onNativeWindowRedrawNeeded;
	activity->callbacks->onNativeWindowResized = onNativeWindowResized;
	activity->callbacks->onPause = onPause;
	activity->callbacks->onResume = onResume;
	activity->callbacks->onSaveInstanceState = onSaveInstanceState;
	activity->callbacks->onStart = onStart;
	activity->callbacks->onStop = onStop;
	activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;

	mercury::log("ANativeActivity_onCreate");
}


#endif