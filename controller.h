#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#include <raylib.h>
#include <pspctrl.h>

typedef void (*bhop_ButtonEvent)(char pressed, SceCtrlData state);

#define _impl_bhop_$ButtonEventBlock(n, x) \
    ({ void _gen_bhop_ButtonEventHandler_##n (char pressed, SceCtrlData state) x; \
       _gen_bhop_ButtonEventHandler_##n; \
    })

#define _impl_bhop_$ButtonEvent(n, x) \
    _impl_bhop_$ButtonEventBlock(n, x)

#define bhop_$ButtonEvent(x) \
    _impl_bhop_$ButtonEvent(__COUNTER__, x)

/**
 * \brief Mapping from button to action
 */
typedef struct {
    bhop_ButtonEvent onSquare;
    bhop_ButtonEvent onTriangle;
    bhop_ButtonEvent onCircle;
    bhop_ButtonEvent onCross;

    bhop_ButtonEvent onUp;
    bhop_ButtonEvent onDown;
    bhop_ButtonEvent onLeft;
    bhop_ButtonEvent onRight;

    bhop_ButtonEvent onL;
    bhop_ButtonEvent onR;

    bhop_ButtonEvent onStart;
    bhop_ButtonEvent onSelect;
} bhop_ButtonMap;

void bhop_ButtonMap_load(bhop_ButtonMap *);

void bhop_scanButtons(void);

#endif

// vim: ft=c.doxygen
