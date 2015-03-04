#ifndef TTYUTIL_MOUSE_H_
#define TTYUTIL_MOUSE_H_

#include "event.h"

// unified mouse event
struct ttyutil_mouse {
    int button;
    int x;
    int y;
    int action;
    int ctrl;
};

// define buttons
#define MOUSE_BUTTON_LEFT (0x0001)
#define MOUSE_BUTTON_LEFT2 (0x0004)
#define MOUSE_BUTTON_LEFT3 (0x0008)
#define MOUSE_BUTTON_LEFT4 (0x0010)
#define MOUSE_BUTTON_RIGHT (0x0002)

// define event types
#define MOUSE_ACTION_UP (0x0000)
#define MOUSE_ACTION_DOWN (0x0001)
#define MOUSE_ACTION_MOVE (0x0002)
#define MOUSE_ACTION_WHEEL (0x0003)
#define MOUSE_ACTION_HWHEEL (0x0004)
// number of different mouse actions
#define MOUSE_ACTION_LENGTH 5

ttyutil_mouse* ttyutil_mouse_destroy(ttyutil_mouse *ptr);
ttyutil_mouse* ttyutil_mouse_create(int button, int x, int y, int action,
        int ctrl);

#endif // TTYUTIL_MOUSE_H_
