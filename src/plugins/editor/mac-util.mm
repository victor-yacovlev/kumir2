#include "mac-util.h"
#import <Cocoa/Cocoa.h>

bool EditorMacUtil::isRussianLayout()
{
    NSTextInputContext * context = [NSTextInputContext currentInputContext];

    if(context != nil)
        return [[context selectedKeyboardInputSource] containsString:@"Russian"];

    return false;
}

bool EditorMacUtil::isCapsLock()
{
    NSUInteger flags = [NSEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask;
    return flags & NSAlphaShiftKeyMask;
}
