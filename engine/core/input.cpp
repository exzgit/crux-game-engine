#include "input.h"

InputSystem::InputSystem(GLFWwindow* win) {
  if (!win) {
    error_callback("failed to initialize InputSystem: WINDOW is null.");
    exit(1);
  }
  
  _window = win;

  glfwSetCursorPosCallback(_window,
    [](GLFWwindow* window, double xpos, double ypos) {
      auto* input =
        static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

      input->mouse_callback(xpos, ypos);
    }
  );
}

void InputSystem::get_input_mode(Cursor& mode) const {
  int glfw_mode = glfwGetInputMode(_window, GLFW_CURSOR);
  mode = static_cast<Cursor>(glfw_mode);
}

bool InputSystem::is_mouse_button_pressed(Mouse button) const {
  return glfwGetMouseButton(_window, static_cast<int>(button)) == GLFW_PRESS;
}

bool InputSystem::is_key_pressed(Keyboard key) const {
  return glfwGetKey(_window, static_cast<int>(key)) == GLFW_PRESS;
}

void InputSystem::set_input_mode(Cursor mode) {
  glfwSetInputMode(_window, GLFW_CURSOR, static_cast<int>(mode));
}

void InputSystem::get_cursor_position(double& xPos, double& yPos) const {
  glfwGetCursorPos(_window, &xPos, &yPos);
}

void InputSystem::set_cursor_position(double& xPos, double& yPos) const {
  glfwSetCursorPos(_window, xPos, yPos);
}

void InputSystem::mouse_callback(double xpos, double ypos) {
  static double last_x = xpos;
  static double last_y = ypos;

  mouse_dx += xpos - last_x;
  mouse_dy += last_y - ypos;

  last_x = xpos;
  last_y = ypos;
}

void InputSystem::reset_mouse_delta() {
  mouse_dx = 0.0;
  mouse_dy = 0.0;
}