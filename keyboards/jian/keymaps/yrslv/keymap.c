#include QMK_KEYBOARD_H
#include "print.h"

enum {
    TD_NONE = 0,
    SINGLE_TAP = 1,
    SINGLE_HOLD = 2,
    DOUBLE_TAP = 3,
    DOUBLE_HOLD = 4,
    UNKNOWN_TAPS = 5
};

typedef struct {
    bool is_press_action;
    int state;
} td_tap_t;

// Tap dance enums
enum {
    CTL_SPACE,
    CMD_SHIFT,
    ALT_SHIFT,
    ALT_BACK,
    Q_FORW,
    ENT_SPACE,
    LAYER_SWITCH
};

int cur_dance(qk_tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void ctl_space_finished(qk_tap_dance_state_t *state, void *user_data);
void ctl_space_reset(qk_tap_dance_state_t *state, void *user_data);

void cmd_shift_finished(qk_tap_dance_state_t *state, void *user_data);
void cmd_shift_reset(qk_tap_dance_state_t *state, void *user_data);

void alt_shift_finished(qk_tap_dance_state_t *state, void *user_data);
void alt_shift_reset(qk_tap_dance_state_t *state, void *user_data);

void alt_back_finished(qk_tap_dance_state_t *state, void *user_data);
void alt_back_reset(qk_tap_dance_state_t *state, void *user_data);

void q_forw_finished(qk_tap_dance_state_t *state, void *user_data);
void q_forw_reset(qk_tap_dance_state_t *state, void *user_data);

void ent_space_finished(qk_tap_dance_state_t *state, void *user_data);
void ent_space_reset(qk_tap_dance_state_t *state, void *user_data);

void layer_switch_finished(qk_tap_dance_state_t *state, void *user_data);
void layer_switch_reset(qk_tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = LAYOUT(
  KC_ESC, TD(ALT_BACK),   TD(Q_FORW),    KC_W,    KC_E,    KC_R,  KC_T,          KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,            KC_LBRC,         LAG_T(KC_RBRC),
          LSFT_T(KC_TAB), KC_A,          KC_S,    KC_D,    KC_F,  KC_G,          KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,         KC_QUOT,
          TD(CTL_SPACE),  KC_Z,          KC_X,    KC_C,    KC_V,  KC_B,          KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, RGUI_T(KC_BSLS),
                         TD(ENT_SPACE), LT(1,KC_CAPS), TD(CMD_SHIFT),         RSFT_T(KC_ENT), LT(2,KC_SPC),  LT(1,KC_BSPC)
),
[1] = LAYOUT(
  KC_TRNS, KC_CAPS, KC_PSLS, KC_7, KC_8, KC_9,  KC_MINS,        KC_AMPR,   KC_COLN,   KC_PMNS,   KC_GT,   KC_NO,   KC_NO,  RSG(KC_4),
           KC_TRNS, KC_PAST, KC_4, KC_5, KC_6,  KC_EQL,         KC_EXLM,   KC_PEQL,   KC_LPRN,   KC_RPRN, KC_QUES, KC_NO,
           KC_TRNS, KC_0,    KC_1, KC_2, KC_3,  KC_PDOT,        KC_PIPE,   KC_UNDS,   KC_LCBR,   KC_RCBR, KC_RALT,   KC_RCMD,
                        LT(3,KC_ENT), KC_TRNS,  KC_TRNS,        KC_TRNS,   LT(3, KC_SPC),   KC_TRNS
),
[2] = LAYOUT(
  KC_TRNS, KC_CAPS,  KC_NO,   KC_F7,   KC_F8,   KC_F9,   KC_F10,        KC_KB_VOLUME_UP,   KC_ENT,        RALT(KC_ENT), KC_RGUI, KC_RSFT, KC_PGUP,  RSG(KC_4),
           KC_TRNS,  KC_NO,   KC_F4,   KC_F5,   KC_F6,   KC_F11,        KC_HOME,           KC_LEFT,       KC_UP,        KC_RGHT, KC_END,  KC_PGDN,
           KC_LCMD,  KC_LGUI, KC_F1,   KC_F2,   KC_F3,   KC_F12,        KC_KB_VOLUME_DOWN, KC_RALT,       KC_DOWN,      TD(ALT_SHIFT), KC_RSFT, KC_RCMD,
                        KC_TRNS, LT(3,KC_CAPS), TD(CMD_SHIFT),        KC_TRNS, KC_TRNS,  LT(3,KC_BSPC)
),
[3] = LAYOUT_symmetric(
  RESET,   DEBUG,   KC_ASUP, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,
           XXXXXXX, KC_ASRP, KC_ASTG,  XXXXXXX, XXXXXXX, XXXXXXX,
           EEP_RST, KC_ASDN, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,
                                       _______, XXXXXXX, _______
)
};

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=false;
  debug_matrix=false;
  //debug_keyboard=true;
  //debug_mouse=true;
}

/* Return an integer that corresponds to what kind of tap dance should be executed.
 *
 * How to figure out tap dance state: interrupted and pressed.
 *
 * Interrupted: If the state of a dance dance is "interrupted", that means that another key has been hit
 *  under the tapping term. This is typically indicitive that you are trying to "tap" the key.
 *
 * Pressed: Whether or not the key is still being pressed. If this value is true, that means the tapping term
 *  has ended, but the key is still being pressed down. This generally means the key is being "held".
 *
 * One thing that is currenlty not possible with qmk software in regards to tap dance is to mimic the "permissive hold"
 *  feature. In general, advanced tap dances do not work well if they are used with commonly typed letters.
 *  For example "A". Tap dances are best used on non-letter keys that are not hit while typing letters.
 *
 * Good places to put an advanced tap dance:
 *  z,q,x,j,k,v,b, any function key, home/end, comma, semi-colon
 *
 * Criteria for "good placement" of a tap dance key:
 *  Not a key that is hit frequently in a sentence
 *  Not a key that is used frequently to double tap, for example 'tab' is often double tapped in a terminal, or
 *    in a web form. So 'tab' would be a poor choice for a tap dance.
 *  Letters used in common words as a double. For example 'p' in 'pepper'. If a tap dance function existed on the
 *    letter 'p', the word 'pepper' would be quite frustating to type.
 *
 * For the third point, there does exist the 'TD_DOUBLE_SINGLE_TAP', however this is not fully tested
 *
 */
int cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        // If count = 1, and it has been interrupted - it doesn't matter if it is pressed or not: Send SINGLE_TAP
        if (state->interrupted) {
            // dprint("interrupted SINGLE_HOLD");
            return SINGLE_HOLD;
            // If the interrupting key is released before the tap-dance key, then it is a single HOLD
            // However, if the tap-dance key is released first, then it is a single TAP
            // But how to get access to the state of the interrupting key????
        } else {
            if (!state->pressed) {
                // dprint("!pressed SINGLE_TAP");
                return SINGLE_TAP;
            }
            else {
                // dprint("pressed SINGLE_HOLD");
                return SINGLE_HOLD;
            }
        }
    } else if (state->count == 2) {
        if (state->pressed)
            return DOUBLE_HOLD;
        else
            return DOUBLE_TAP;
    } else
        return UNKNOWN_TAPS;
}

// Create an instance of 'td_tap_t' for the 'x' tap dance.
static td_tap_t ctl_space_tap_state = {.is_press_action = true, .state = TD_NONE};

void ctl_space_finished(qk_tap_dance_state_t *state, void *user_data) {
    ctl_space_tap_state.state = cur_dance(state);
    switch (ctl_space_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_LCTL);
            break;
        case SINGLE_HOLD:
            register_code(KC_LCTL);
            break;
        case DOUBLE_TAP:
            register_code(KC_LCTL);
            tap_code(KC_SPC);
            break;
    }
}

void ctl_space_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (ctl_space_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_LCTL);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_LCTL);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_SPC);
            unregister_code(KC_LCTL);
            break;
    }
    ctl_space_tap_state.state = TD_NONE;
}

static td_tap_t cmd_shift_tap_state = {.is_press_action = true, .state = TD_NONE};

void cmd_shift_finished(qk_tap_dance_state_t *state, void *user_data) {
    cmd_shift_tap_state.state = cur_dance(state);
    switch (cmd_shift_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_RCMD);
            break;
        case SINGLE_HOLD:
            register_code(KC_RCMD);
            break;
        case DOUBLE_TAP:
            tap_code(KC_RSFT);
            register_code(KC_RSFT);
            break;
        case DOUBLE_HOLD:
            register_code(KC_RCMD);
            register_code(KC_RSFT);
            break;
    }
}

void cmd_shift_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (cmd_shift_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_RCMD);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_RCMD);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_RSFT);
            break;
        case DOUBLE_HOLD:
            unregister_code(KC_RCMD);
            unregister_code(KC_RSFT);
            break;
    }
    cmd_shift_tap_state.state = TD_NONE;
}

static td_tap_t alt_shift_tap_state = {.is_press_action = true, .state = TD_NONE};

void alt_shift_finished(qk_tap_dance_state_t *state, void *user_data) {
    alt_shift_tap_state.state = cur_dance(state);
    switch (alt_shift_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_RALT);
            break;
        case SINGLE_HOLD:
            register_code(KC_RALT);
            break;
        case DOUBLE_HOLD:
            register_code(KC_RALT);
            register_code(KC_RSFT);
            break;
    }
}

void alt_shift_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (alt_shift_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_RALT);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_RALT);
            break;
        case DOUBLE_HOLD:
            unregister_code(KC_RALT);
            unregister_code(KC_RSFT);
            break;
    }
    alt_shift_tap_state.state = TD_NONE;
}

static td_tap_t alt_back_tap_state = {.is_press_action = true, .state = TD_NONE};

void alt_back_finished(qk_tap_dance_state_t *state, void *user_data) {
    alt_back_tap_state.state = cur_dance(state);
    switch (alt_back_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_GRV);
            break;
        case SINGLE_HOLD:
            register_code(KC_RSFT);
            register_code(KC_GRV);
            break;
        case DOUBLE_TAP:
            register_code(KC_LCMD);
            register_code(KC_LBRC);
            break;
        case DOUBLE_HOLD:
            register_code(KC_RALT);
            break;
    }
}

void alt_back_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (alt_back_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_GRV);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_RSFT);
            unregister_code(KC_GRV);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_LBRC);
            unregister_code(KC_LCMD);
            break;
        case DOUBLE_HOLD:
            unregister_code(KC_RALT);
            break;
    }
    alt_back_tap_state.state = TD_NONE;
}

static td_tap_t q_forw_tap_state = {.is_press_action = true, .state = TD_NONE};

void q_forw_finished(qk_tap_dance_state_t *state, void *user_data) {
    q_forw_tap_state.state = cur_dance(state);
    switch (q_forw_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_Q);
            break;
        case SINGLE_HOLD:
            register_code(KC_RSFT);
            register_code(KC_Q);
            break;
        case DOUBLE_TAP:
            register_code(KC_LCMD);
            register_code(KC_RBRC);
            break;
        case DOUBLE_HOLD:
            register_code(KC_Q);
            register_code(KC_Q);
            break;
    }
}

void q_forw_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (q_forw_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_Q);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_Q);
            unregister_code(KC_RSFT);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_RBRC);
            unregister_code(KC_LCMD);
            break;
        case DOUBLE_HOLD:
            unregister_code(KC_Q);
            unregister_code(KC_Q);
            break;
    }
    q_forw_tap_state.state = TD_NONE;
}

static td_tap_t ent_space_tap_state = {.is_press_action = true, .state = TD_NONE};

void ent_space_finished(qk_tap_dance_state_t *state, void *user_data) {
    ent_space_tap_state.state = cur_dance(state);
    switch (ent_space_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_TAB);
            break;
        case SINGLE_HOLD:
            register_code(KC_RALT);
            break;
        case DOUBLE_TAP:
            register_code(KC_ENT);
            break;
        case DOUBLE_HOLD:
            layer_on(2);
            break;
    }
}

void ent_space_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (ent_space_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_TAB);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_RALT);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_ENT);
            break;
        case DOUBLE_HOLD:
            layer_off(2);
            break;
    }
    ent_space_tap_state.state = TD_NONE;
}

static td_tap_t layer_switch_tap_state = {.is_press_action = true, .state = TD_NONE};

void layer_switch_finished(qk_tap_dance_state_t *state, void *user_data) {
    layer_switch_tap_state.state = cur_dance(state);
    switch (layer_switch_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_CAPS);
            // dprint("register_code(KC_CAPS)");
            break;
        case SINGLE_HOLD:
            layer_on(1);
            // dprint("layer_on(1)");
            break;
        case DOUBLE_TAP:
            register_code(KC_CAPS);
            // dprint("register_code(KC_CAPS)");
            break;
        case DOUBLE_HOLD:
            layer_on(2);
            // dprint("layer_on(2)");
            break;
    }
}

void layer_switch_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (layer_switch_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_CAPS);
            // dprint("unregister_code(KC_CAPS)");
            break;
        case SINGLE_HOLD:
            layer_off(1);
            // dprint("layer_off(1)");
            break;
        case DOUBLE_TAP:
            unregister_code(KC_CAPS);
            // dprint("unregister_code(KC_CAPS)");
            break;
        case DOUBLE_HOLD:
            layer_off(2);
            // dprint("layer_off(2)");
            break;
    }
    layer_switch_tap_state.state = TD_NONE;
}

// bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
//     dprintf("get_permissive_hold %s", keycode);
//     switch (keycode) {
//         case TD(LAYER_SWITCH):
            // dprint("pepm hold true");
//             // Immediately select the hold action when another key is tapped.
//             return true;
//         default:
//             // Do not select the hold action when another key is tapped.
            // dprint("pepm hold false");
//             return false;
//     }
// }

qk_tap_dance_action_t tap_dance_actions[] = {
  [CTL_SPACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ctl_space_finished, ctl_space_reset),
  [CMD_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, cmd_shift_finished, cmd_shift_reset),
  [ALT_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, alt_shift_finished, alt_shift_reset),
  [ALT_BACK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, alt_back_finished, alt_back_reset),
  [Q_FORW] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, q_forw_finished, q_forw_reset),
  [ENT_SPACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ent_space_finished, ent_space_reset),
  [LAYER_SWITCH] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, layer_switch_finished, layer_switch_reset)
  };
