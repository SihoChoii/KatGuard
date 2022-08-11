#include "displayIcons.h"

struct lockIcons {
    const byte locked[] = {
        B01110,
        B10001,
        B10001,
        B11111,
        B11011,
        B11011,
        B11111,
        B00000
    };
    const byte unlocked[] = {
        B01110,
        B10010,
        B10000,
        B11111,
        B11011,
        B11011,
        B11111,
        B00000
    };
}