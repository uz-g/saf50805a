#pragma once

#include "main.h"

namespace gui {
    void initializeGUI();
    void updateGUI();
    
    // Declare the callback functions here
    void autonBtnmAction(lv_event_t *e);
    void autoSelectorCallback(lv_event_t* e);
}