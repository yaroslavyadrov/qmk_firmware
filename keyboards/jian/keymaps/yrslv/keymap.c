#include QMK_KEYBOARD_H

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
    ALT_SHIFT
};

int cur_dance(qk_tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void ctl_space_finished(qk_tap_dance_state_t *state, void *user_data);
void ctl_space_reset(qk_tap_dance_state_t *state, void *user_data);

void cmd_shift_finished(qk_tap_dance_state_t *state, void *user_data);
void cmd_shift_reset(qk_tap_dance_state_t *state, void *user_data);

void alt_shift_finished(qk_tap_dance_state_t *state, void *user_data);
void alt_shift_reset(qk_tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = LAYOUT(
  KC_ESC, LALT_T(KC_GRV), KC_Q,    KC_W,    KC_E,    KC_R,      KC_T,      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,            KC_LBRC,         KC_RBRC,
          LSFT_T(KC_TAB), KC_A,    KC_S,    KC_D,    KC_F,      KC_G,      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,         LAG_T(KC_QUOT),
          TD(CTL_SPACE),  KC_Z,    KC_X,    KC_C,    KC_V,      KC_B,      KC_N,    KC_M,    KC_COMM, KC_DOT,  RALT_T(KC_SLSH), RGUI_T(KC_BSLS),
               LT(1,KC_ESC), TD(CMD_SHIFT), LT(2,KC_CAPS),      LT(1,KC_ENT), RSFT_T(KC_SPC), LT(2,KC_BSPC)
),
[1] = LAYOUT(
  KC_TRNS, KC_CAPS, KC_PSLS, KC_7, KC_8, KC_9,  KC_MINS,        KC_AMPR,   KC_COLN,   KC_PMNS,   KC_GT,   KC_NO,   KC_NO,  RSG(KC_4),
           KC_TRNS, KC_PAST, KC_4, KC_5, KC_6,  KC_EQL,         KC_EXLM,   KC_PEQL,   KC_LPRN,   KC_RPRN, KC_QUES, KC_NO,
           KC_TRNS, KC_0,    KC_1, KC_2, KC_3,  KC_PCMM,        KC_PIPE,   KC_UNDS,   KC_LCBR,   KC_RCBR, KC_RALT,   KC_RCMD,
                        KC_TRNS, KC_TRNS, LT(3,KC_CAPS),        KC_TRNS,   KC_TRNS,   LT(3, KC_BSPC)
),
[2] = LAYOUT(
  KC_TRNS, KC_CAPS,  KC_NO,   KC_F7,   KC_F8,   KC_F9,   KC_F10,      KC_KB_VOLUME_UP,   KC_ENT,        RALT(KC_ENT), KC_RGUI, KC_RSFT, KC_NO,  RSG(KC_4),
           KC_TRNS,  KC_NO,   KC_F4,   KC_F5,   KC_F6,   KC_F11,      KC_HOME,           KC_LEFT,       KC_UP,        KC_RGHT, KC_END,  KC_NO,
           KC_TRNS,  KC_LGUI, KC_F1,   KC_F2,   KC_F4,   KC_F12,      KC_KB_VOLUME_DOWN, TD(ALT_SHIFT), KC_DOWN,      KC_RALT, KC_RSFT, KC_RCMD,
                                  LT(3,KC_ESC), KC_TRNS, KC_TRNS,     LT(3,KC_ENT), KC_TRNS, KC_TRNS
),
[3] = LAYOUT_symmetric(
  RESET,   DEBUG,   KC_ASUP, XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,
           XXXXXXX, KC_ASRP, KC_ASTG,  XXXXXXX, XXXXXXX, XXXXXXX,
           EEP_RST, KC_ASDN, XXXXXXX, XXXXXXX, XXXXXXX,XXXXXXX,
                                      _______, XXXXXXX,   _______
)
};

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
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else
            return SINGLE_HOLD;
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


qk_tap_dance_action_t tap_dance_actions[] = {
    [CTL_SPACE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ctl_space_finished, ctl_space_reset),
    [CMD_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, cmd_shift_finished, cmd_shift_reset),
    [ALT_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, alt_shift_finished, alt_shift_reset)
};