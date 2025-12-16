#pragma once

#include <GLFW/glfw3.h>
#include "error.h"

enum class Cursor {
  Disable = GLFW_CURSOR_DISABLED,
  Normal = GLFW_CURSOR_NORMAL,
  Hidden = GLFW_CURSOR_HIDDEN,
  Captured = GLFW_CURSOR_CAPTURED,
  Unvailable = GLFW_CURSOR_UNAVAILABLE
};

enum class Pointer {
  Hand = GLFW_HAND_CURSOR,
  Arrow = GLFW_ARROW_CURSOR,
  IBeam = GLFW_IBEAM_CURSOR,
  HResize = GLFW_HRESIZE_CURSOR,
  VResize = GLFW_VRESIZE_CURSOR,
  Resize = GLFW_RESIZE_ALL_CURSOR,
  Crosshair = GLFW_CROSSHAIR_CURSOR,
  NotAllowed = GLFW_NOT_ALLOWED_CURSOR,
};

enum class Action {
  Press = GLFW_PRESS,
  Release = GLFW_RELEASE,
  Repeat = GLFW_REPEAT
};

enum class Mouse {
  Left = GLFW_MOUSE_BUTTON_LEFT,
  Right = GLFW_MOUSE_BUTTON_RIGHT,
  Middle = GLFW_MOUSE_BUTTON_MIDDLE
};

enum class Keyboard {
  SPACE = GLFW_KEY_SPACE,
  APOSTROPHE = GLFW_KEY_APOSTROPHE,
  COMMA = GLFW_KEY_COMMA,
  MINUS = GLFW_KEY_MINUS,
  PERIOD = GLFW_KEY_PERIOD,
  SLASH = GLFW_KEY_SLASH,
  NUM_0 = GLFW_KEY_0,
  NUM_1 = GLFW_KEY_1,
  NUM_2 = GLFW_KEY_2,
  NUM_3 = GLFW_KEY_3,
  NUM_4 = GLFW_KEY_4,
  NUM_5 = GLFW_KEY_5,
  NUM_6 = GLFW_KEY_6,
  NUM_7 = GLFW_KEY_7,
  NUM_8 = GLFW_KEY_8,
  NUM_9 = GLFW_KEY_9,
  SEMICOLON = GLFW_KEY_SEMICOLON,
  EQUAL = GLFW_KEY_EQUAL,
  A = GLFW_KEY_A,
  B = GLFW_KEY_B,
  C = GLFW_KEY_C,
  D = GLFW_KEY_D,
  E = GLFW_KEY_E,
  F = GLFW_KEY_F,
  G = GLFW_KEY_G,
  H = GLFW_KEY_H,
  I = GLFW_KEY_I,
  J = GLFW_KEY_J,
  K = GLFW_KEY_K,
  L = GLFW_KEY_L,
  M = GLFW_KEY_M,
  N = GLFW_KEY_N,
  O = GLFW_KEY_O,
  P = GLFW_KEY_P,
  Q = GLFW_KEY_Q,
  R = GLFW_KEY_R,
  S = GLFW_KEY_S,
  T = GLFW_KEY_T,
  U = GLFW_KEY_U,
  V = GLFW_KEY_V,
  W = GLFW_KEY_W,
  X = GLFW_KEY_X,
  Y = GLFW_KEY_Y,
  Z = GLFW_KEY_Z,
  ESCAPE = GLFW_KEY_ESCAPE,
  ENTER = GLFW_KEY_ENTER,
  TAB = GLFW_KEY_TAB,
  BACKSPACE = GLFW_KEY_BACKSPACE,
  INSERT = GLFW_KEY_INSERT,
  DELETE = GLFW_KEY_DELETE,
  RIGHT = GLFW_KEY_RIGHT,
  LEFT = GLFW_KEY_LEFT,
  DOWN = GLFW_KEY_DOWN,
  UP = GLFW_KEY_UP,
  PAGE_UP = GLFW_KEY_PAGE_UP,
  PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
  HOME = GLFW_KEY_HOME,
  END = GLFW_KEY_END,
  CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
  SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
  NUM_LOCK = GLFW_KEY_NUM_LOCK,
  PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
  PAUSE = GLFW_KEY_PAUSE,
  F1 = GLFW_KEY_F1,
  F2 = GLFW_KEY_F2,
  F3 = GLFW_KEY_F3,
  F4 = GLFW_KEY_F4,
  F5 = GLFW_KEY_F5,
  F6 = GLFW_KEY_F6,
  F7 = GLFW_KEY_F7,
  F8 = GLFW_KEY_F8,
  F9 = GLFW_KEY_F9,
  F10 = GLFW_KEY_F10,
  F11 = GLFW_KEY_F11,
  F12 = GLFW_KEY_F12,
  LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
  LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
  LEFT_ALT = GLFW_KEY_LEFT_ALT,
  LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
  RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
  RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
  RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
  RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
  MENU = GLFW_KEY_MENU
};

class InputSystem {
  public:
    double mouse_dx = 0.0;
    double mouse_dy = 0.0;

    InputSystem(GLFWwindow* win);
    ~InputSystem() = default;

    void set_input_mode(Cursor mode);
    void get_input_mode(Cursor& mode) const;
    void get_cursor_position(double& xPos, double& yPos) const;
    void set_cursor_position(double& xPos, double& yPos) const;
    void mouse_callback(double xpos, double ypos);
    void reset_mouse_delta();
    
    bool is_key_pressed(Keyboard key) const;
    bool is_mouse_button_pressed(Mouse button) const;

  private:
    GLFWwindow* _window;
};
