#import <Cocoa/Cocoa.h>
#include "mac-fixes.h"

void MacFixes::setLionFullscreenButton(void * nsviewptr) {
    NSView * nsview = (NSView*)nsviewptr;
    NSWindow *nswindow = [nsview window];
#if defined(MAC_OS_X_VERSION_10_7 )
    [nswindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
#endif
}
