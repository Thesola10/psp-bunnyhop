
#include "controller.h"



static SceCtrlData btnStates;

static unsigned int buttonsPrev;

static bhop_ButtonMap *currentMap;

void bhop_ButtonMap_load(bhop_ButtonMap *map)
{
    currentMap = map;
}

void bhop_scanButtons(void)
{
    unsigned int scanMask;

    sceCtrlReadBufferPositive(&btnStates, 1);

    scanMask = btnStates.Buttons ^ buttonsPrev;

#define _impl_bhop_ButtonMap_callIfExists$(map, btn) { \
        if ( map != (void*) 0 ) { map ( btnStates.Buttons & btn ? 1 : 0 , btnStates ); } \
        else                    { TraceLog(LOG_INFO, "Empty handler: " #map "\n" ); } \
    }

    if (scanMask & PSP_CTRL_SQUARE)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onSquare, PSP_CTRL_SQUARE);
    if (scanMask & PSP_CTRL_TRIANGLE)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onTriangle, PSP_CTRL_TRIANGLE);
    if (scanMask & PSP_CTRL_CIRCLE)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onCircle, PSP_CTRL_CIRCLE);
    if (scanMask & PSP_CTRL_CROSS)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onCross, PSP_CTRL_CROSS);

    if (scanMask & PSP_CTRL_UP)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onUp, PSP_CTRL_UP);
    if (scanMask & PSP_CTRL_DOWN)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onDown, PSP_CTRL_DOWN);
    if (scanMask & PSP_CTRL_LEFT)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onLeft, PSP_CTRL_LEFT);
    if (scanMask & PSP_CTRL_RIGHT)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onRight, PSP_CTRL_RIGHT);

    if (scanMask & PSP_CTRL_LTRIGGER)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onL, PSP_CTRL_LTRIGGER);
    if (scanMask & PSP_CTRL_RTRIGGER)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onR, PSP_CTRL_RTRIGGER);

    if (scanMask & PSP_CTRL_START)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onStart, PSP_CTRL_START);
    if (scanMask & PSP_CTRL_SELECT)
        _impl_bhop_ButtonMap_callIfExists$(currentMap->onSelect, PSP_CTRL_SELECT);

    buttonsPrev = btnStates.Buttons;
}
