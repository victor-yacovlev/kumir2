#import <Cocoa/Cocoa.h>
#include "mac-fixes.h"

void MacFixes::setLionFullscreenButton(void * nsviewptr) {
    NSView * nsview = (NSView*)nsviewptr;
    NSWindow *nswindow = [nsview window];
    [nswindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
}
