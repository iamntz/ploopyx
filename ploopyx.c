/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "quantum.h"
#include "ploopyx.h"
#include "analog.h"

#include "includes/default-definitions.c"

keyboard_config_t keyboard_config;
uint16_t          dpi_array[] = PLOOPY_DPI_OPTIONS;
#define DPI_OPTION_SIZE ARRAY_SIZE(dpi_array)

bool is_scroll_clicked = false;
bool is_volume_on_scroll = false;
bool is_drag_scroll = false;

bool scroll_h_locked = false;
bool scroll_v_locked = false;
bool prevent_scroll_lock = false;

float total_scroll_h = 0;
float total_scroll_v = 0;

float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

bool mouse_moved = false;

void toggle_drag_scroll(void) {
    is_drag_scroll ^= 1;
}

void cycle_dpi(void) {
    keyboard_config.dpi_config = (keyboard_config.dpi_config + 1) % DPI_OPTION_SIZE;
    eeconfig_update_kb(keyboard_config.raw);
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    static uint16_t volume_scroll_timer;

    mouse_moved = mouse_report.h != 0 || mouse_report.v != 0;

    if (!is_drag_scroll && !is_volume_on_scroll) {
        return pointing_device_task_user(mouse_report);
    }

    if (is_volume_on_scroll) {
        if(timer_elapsed(volume_scroll_timer) > PLOOPY_VOLUME_SCROLL_DEBOUNCE) {
            if ((float) mouse_report.y < 0) {
                tap_code(KC_VOLU);
            } else if ((float) mouse_report.y > 0) {
                tap_code(KC_VOLD);
            }

            volume_scroll_timer = timer_read();
        }
            // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;

        return pointing_device_task_user(mouse_report);
    }

    float scroll_delta_h = (float)mouse_report.x / PLOOPY_DRAGSCROLL_DIVISOR_H;
    float scroll_delta_v = (float)mouse_report.y / PLOOPY_DRAGSCROLL_DIVISOR_V;

    total_scroll_h += scroll_delta_h;
    total_scroll_v += scroll_delta_v;

    // prevent_scroll_lock = prevent_scroll_lock || abs(total_scroll_h) > SCROLL_UNLOCK_THRESHOLD || abs(total_scroll_v) > SCROLL_UNLOCK_THRESHOLD;

    // if (prevent_scroll_lock) {
    //     scroll_v_locked = false;
    //     scroll_h_locked = false;
    // }

    if (!prevent_scroll_lock && abs(total_scroll_h) > SCROLL_LOCK_THRESHOLD) {
        scroll_v_locked = true;
        scroll_h_locked = false;
    }

    if (!prevent_scroll_lock && abs(total_scroll_v) > SCROLL_LOCK_THRESHOLD) {
        scroll_h_locked = true;
        scroll_v_locked = false;
    }

    if (!scroll_h_locked) {
        scroll_accumulated_h += scroll_delta_h;
    }

    if (!scroll_v_locked) {
        scroll_accumulated_v += scroll_delta_v;
    }

    int8_t direction_modifier = 1;

    #ifdef PLOOPY_DRAGSCROLL_INVERT
        direction_modifier = -1;
    #endif

    if (is_drag_scroll) {
        mouse_report.v = (int8_t)scroll_accumulated_v * direction_modifier;
        mouse_report.h = (int8_t)scroll_accumulated_h * direction_modifier;
    }

    // Update accumulated scroll values by subtracting the integer parts
    scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
    scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

    // Clear the X and Y values of the mouse report
    mouse_report.x = 0;
    mouse_report.y = 0;

    return pointing_device_task_user(mouse_report);
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    static uint16_t drag_scroll_timer;

    is_volume_on_scroll = false;

    if (!process_record_user(keycode, record)) {
        return false;
    }

    if (keycode == DPI_CONFIG && record->event.pressed) {
        cycle_dpi();
    }

    if (keycode == TOGGLE_DRAG_SCROLL && record->event.pressed) {
        toggle_drag_scroll();
    }

    if (keycode == VOLUME_ON_SCROLL && record->event.pressed) {
        is_volume_on_scroll = true;
        return false; // Prevent further processing of this keycode
    }

    if (keycode == DPI_SLOW_MO) {
        if (record->event.pressed) {
            pointing_device_set_cpi(PLOOPY_SLOW_DPI);
        } else {
            pointing_device_set_cpi(PLOOPY_FAST_DPI);
        }
    }

    if (keycode == SCROLL_OR_CLICK3) {
        is_drag_scroll = record->event.pressed;

        if(record->event.pressed) {
            layer_on(2);
            mouse_moved = false;
            scroll_accumulated_h = scroll_accumulated_v = 0;
            total_scroll_h = total_scroll_v = 0;
            scroll_h_locked = scroll_v_locked = false;
            prevent_scroll_lock = false;
            drag_scroll_timer = timer_read();
        } else {
            layer_off(2);
            if (!mouse_moved && (timer_elapsed(drag_scroll_timer) < TAPPING_TERM)) {
                // if we didn't scroll, it's a middle click
                tap_code(KC_BTN3);
            }
        }
        return false;
    }

    if (keycode == DRAG_SCROLL || keycode == MO(2)) {
        is_drag_scroll = record->event.pressed;
        total_scroll_h = total_scroll_v = 0;
        prevent_scroll_lock = false;
        scroll_h_locked = scroll_v_locked = false;
    }

    return true;
}

// Hardware Setup
void keyboard_pre_init_kb(void) {
    // debug_enable  = true;
    // debug_matrix  = true;
    // debug_mouse   = true;
    // debug_encoder = true;

    /* Ground all output pins connected to ground. This provides additional
     * pathways to ground. If you're messing with this, know this: driving ANY
     * of these pins high will cause a short. On the MCU. Ka-blooey.
     */
#ifdef UNUSABLE_PINS
    const pin_t unused_pins[] = UNUSABLE_PINS;

    for (uint8_t i = 0; i < ARRAY_SIZE(unused_pins); i++) {
        gpio_set_pin_output_push_pull(unused_pins[i]);
        gpio_write_pin_low(unused_pins[i]);
    }
#endif

    // This is the debug LED.
#if defined(DEBUG_LED_PIN)
    gpio_set_pin_output_push_pull(DEBUG_LED_PIN);
    gpio_write_pin(DEBUG_LED_PIN, debug_enable);
#endif

    keyboard_pre_init_user();
}

void pointing_device_init_kb(void) {
    keyboard_config.raw = eeconfig_read_kb();
    if (keyboard_config.dpi_config > DPI_OPTION_SIZE) {
        eeconfig_init_kb();
    }
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
}

void eeconfig_init_kb(void) {
    keyboard_config.dpi_config = PLOOPY_DPI_DEFAULT;
    eeconfig_update_kb(keyboard_config.raw);
    eeconfig_init_user();
}
