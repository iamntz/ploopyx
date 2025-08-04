/* Copyright 2023 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
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
#include QMK_KEYBOARD_H

#include "quantum.h"
#include "ploopyx.h"

#define PREV_DESKTOP LGUI(RCTL(KC_LEFT))
#define NEXT_DESKTOP LGUI(RCTL(KC_RIGHT))
#define ALL_WINDOWS LGUI(KC_TAB)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( LT(1, KC_BTN3), LT(4, KC_BTN4), LT(5, KC_BTN5), KC_BTN2, KC_BTN1, SCROLL_OR_CLICK3 ),
    [1] = LAYOUT( KC_NO, PREV_DESKTOP, NEXT_DESKTOP, ALL_WINDOWS, KC_NO, KC_NO ),
    [2] = LAYOUT( KC_HOME, KC_PGUP, KC_PGDN, KC_END, KC_LCTL, KC_NO),
    [3] = LAYOUT( KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),
    [4] = LAYOUT( KC_NO, KC_NO, KC_MUTE, KC_VOLU, KC_VOLD, KC_NO),
    [5] = LAYOUT( KC_MPRV, KC_MPLY, KC_NO, KC_MNXT, KC_NO, KC_NO)
};
