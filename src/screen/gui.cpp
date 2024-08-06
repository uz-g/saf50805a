#include "screen/gui.hpp"
#include "globals.h"

namespace screen {

// Button map for LVGL GUI
static const char *btnmMap[] = {"far side", "close side", "skills", ""};

void autonBtnmAction(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    const char *txt = lv_btnmatrix_get_btn_text(obj, lv_btnmatrix_get_selected_btn(obj));
    if (lv_obj_get_user_data(obj) == (void *)100) {
        if (std::string(txt) == "far side") {
            controller.rumble("._");
            selected_auto = AutoMode::FAR_SIDE;
        } else if (std::string(txt) == "close side") {
            controller.rumble(".. _");
            selected_auto = AutoMode::CLOSE_SIDE;
        } else if (std::string(txt) == "skills") {
            controller.rumble("_._");
            selected_auto = AutoMode::SKILLS;
        }
    }
}

void autoSelectorCallback(lv_event_t* e) {
    lv_obj_t* dropdown = lv_event_get_target(e);
    selected_auto = static_cast<AutoMode>(lv_dropdown_get_selected(dropdown));
}

void initializeGUI() {
    // LVGL GUI setup
    lv_theme_t *th = lv_theme_default_init(
        lv_disp_get_default(), lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_theme_set_apply_cb(th, NULL);

    std::cout << pros::millis() << ": creating gui..." << std::endl;

    // Create tabview and add tabs
    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 20);
    lv_obj_t *mainTab = lv_tabview_add_tab(tabview, "Autons");

    // Create button matrix for autonomous selection
    lv_obj_t *mainBtnm = lv_btnmatrix_create(mainTab);
    lv_btnmatrix_set_map(mainBtnm, btnmMap);
    lv_btnmatrix_set_btn_ctrl_all(mainBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(mainBtnm, autonBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(mainBtnm, 450, 50);
    lv_obj_align(mainBtnm, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_user_data(mainBtnm, (void *)100);

    // Create dropdown for autonomous selection
    lv_obj_t* autoSelector = lv_dropdown_create(mainTab);
    lv_dropdown_set_options(autoSelector, "Off\nClose Side\nFar Side\nSkills");
    lv_obj_align(autoSelector, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(autoSelector, autoSelectorCallback, LV_EVENT_VALUE_CHANGED, NULL);
}

void updateGUI() {
    // Add any GUI update logic here if needed
}

} // namespace screen