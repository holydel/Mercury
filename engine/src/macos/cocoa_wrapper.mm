#include "cocoa_wrapper.h"
#import <Cocoa/Cocoa.h>

void createCocoaWindow()
{
    @autoreleasepool {
        NSRect frame = NSMakeRect(0, 0, 200, 200);
        NSWindow* window  = [[[NSWindow alloc] initWithContentRect:frame
                                                         styleMask:NSBorderlessWindowMask
                                                           backing:NSBackingStoreBuffered
                                                             defer:NO] autorelease];
        [window setBackgroundColor:[NSColor blueColor]];
        [window makeKeyAndOrderFront:NSApp];
    }
}