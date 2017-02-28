#ifndef SANDBOXWIDGET_FRAME_H
#define SANDBOXWIDGET_FRAME_H

namespace Python3Language {
namespace Sandbox {

static const char *PropFrameType = "PropFrameType";
static const char *PropEditableFrame = "PropEditableFrame";

enum FrameType {
    FrameUnknown,
    FrameInput,
    FrameOutput,
    FrameError,
    FrameHtml,
    FrameImage,
    FramePyInput
};

}
}

#endif // SANDBOXWIDGET_FRAME_H
