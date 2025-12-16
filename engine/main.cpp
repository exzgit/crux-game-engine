#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h> 
#include <GLFW/glfw3.h> 
#include "recs/world.h"
#include "core/mesh.h"
#include "core/transform.h"
#include "core/render.h"
#include "core/camera.h"
#include "core/input.h"
#include "core/physics_system.h"
#include "tools/load_object.h"
#include "core/error.h"
#include "core/time.h"

using namespace std;

#define DEFAULT_SCREEN_WIDTH    800
#define DEFAULT_SCREEN_HEIGHT   600


int main() {
  static int WIDTH                = DEFAULT_SCREEN_WIDTH;
  static int HEIGHT               = DEFAULT_SCREEN_HEIGHT;
 
  //
  // Initialize the GLFW library.
  // This must be called before any other function.
  // If initialization fails, the application cannot continue.
  //
  if (!glfwInit()) {
    error_callback("failed initialize GLFW.");
    exit(1);
  }

  //
  // Configure GLFW window hints for OpenGL context creation.
  // Specify the required OpenGL version and profile.
  //
  // - Request an OpenGL 3.3 context
  // - Use the Core Profile (no deprecated fixed-function pipeline)
  //
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //
  // Create the main application window and OpenGL context.
  // This call allocates the window surface and prepares an OpenGL context,
  // but the context is not active until it is made current.
  //
  GLFWwindow* WINDOW = glfwCreateWindow(
    DEFAULT_SCREEN_WIDTH,
    DEFAULT_SCREEN_HEIGHT,
    "Crux Engine",
    NULL,
    NULL
  );

  //
  // Verify that the GLFW window and OpenGL context were created successfully.
  // If window creation fails, terminate GLFW and abort the application.
  //
  if (!WINDOW) {
    error_callback("failed to create GLFW window context.");
    glfwTerminate();
    exit(1);
  }
 
  //
  // Make the OpenGL context of the window current on the calling thread.
  // This is required before calling any OpenGL or GLAD function.
  //
  glfwMakeContextCurrent(WINDOW);

  //
  // Initialize the InputSystem using the created GLFW window.
  // This system handles keyboard and mouse input during runtime.
  //
  InputSystem input = InputSystem(WINDOW);

  //
  // Associate the InputSystem instance with the GLFW window.
  // This allows GLFW callbacks to retrieve engine-spesific context
  // via glfwGetWindowUserPointer().
  //
  glfwSetWindowUserPointer(WINDOW, &input);

  //
  // Load all OpenGL function pointers using GLAD.
  // This must be done after creating an OpenGL context.
  // If loading fails, terminate GLFW and abort the application.
  //
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    error_callback("failed to initialize gl loader.");
    exit(1);
  }

  //
  // Enable depth testing in OpenGL to ensure correct rendering of 3D objects.
  // The depth function GL_LESS means a fragment will be drawn if its depth
  // value is less than the current depth buffer value.
  //
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

 
  // =========================================
  // MARK: Game Startup Configuration
  // =========================================

  //
  // Create the main ECS world instance.
  // The World manages all entities, components, and systems.
  //
  World world;

  //
  // Add the physics system to the world.
  // This system handles basic rigidbody integration and naive collision detection.
  //
  world.add_system < __RUNTIME__::PhysicsSystem > ();

  // 
  // Initialize the rendering system.
  // The SystemRenderer is responsible for drawing all entities that have renderable components.
  //
  __RUNTIME__::SystemRenderer renderer = __RUNTIME__::SystemRenderer();

  // Load 3D Model.
  auto e1 = __TOOLS__::create_entities_from_obj(
    world,
    "./engine/assets/Mesh.obj"
  );
  assert(world.alive(e1));

  // Create Entity with Camera Components.
  auto cam = world.create_entity(); 
  assert(world.alive(cam));
  world.add<Camera>(cam);
  world.add<Transform>(cam);
  auto & cam_transform = world.get<Transform>(cam);
  cam_transform.position = {
    0,
    0,
    0.0f
  };

  //
  // --------------------------------------------------------------------------------
  // Flag to detect the right mouse button (RMB) state transition
  // --------------------------------------------------------------------------------
  // This boolean indicates whether the "mouse look" mode was active in the
  // previous frame.
  // - true     : mouse look was active last frame.
  // - false    : mouse look was not active or has just been released.
  //
  // Purpose:
  // When the right mouse button is pressed for the first time, the camera should not immediately apply rotation based on the cursor's previous
  // free position. This flag enables edge detection:
  // - First frame of RMB press -> synchronize cursor position
  // - subsequent frames        -> compute mouse delta for camera rotation
  //
  // This prevents sudden camera jumps when entering mouse look mode.
  static bool   was_right_mouse_down = false;

  // ================================================================================
  // GAME UPDATE
  // ================================================================================
  while (!glfwWindowShouldClose(WINDOW)) {

    // ============================
    // FRAME TIMING
    // ============================
    Time::update_deltatime();

    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (input.is_mouse_button_pressed(Mouse::Right)) {

      if (!was_right_mouse_down) {
        input.set_input_mode(Cursor::Disable);
        input.reset_mouse_delta();

        was_right_mouse_down = true;
      }

      const float speed = 5.0f * Time::delta_time;
      auto& camera = world.get<Camera>(cam);

      // Translation
      if (input.is_key_pressed(Keyboard::W))
        cam_transform.position += camera.forward * speed;
      if (input.is_key_pressed(Keyboard::S))
        cam_transform.position -= camera.forward * speed;
      if (input.is_key_pressed(Keyboard::A))
        cam_transform.position -= camera.right * speed;
      if (input.is_key_pressed(Keyboard::D))
        cam_transform.position += camera.right * speed;
      if (input.is_key_pressed(Keyboard::Q))
        cam_transform.position -= camera.up * speed;
      if (input.is_key_pressed(Keyboard::E))
        cam_transform.position += camera.up * speed;


      // Rotation (RELATIVE ONLY)
      const float sensitivity = 0.1f;
      camera.yaw   += static_cast<float>(input.mouse_dx * sensitivity);
      camera.pitch += static_cast<float>(input.mouse_dy * sensitivity);

      camera.pitch = glm::clamp(camera.pitch, -89.0f, 89.0f);
      input.reset_mouse_delta();
     
    } else {
      if (was_right_mouse_down) {
        input.set_input_mode(Cursor::Normal);
        was_right_mouse_down = false;
      }
    }

    // ============================
    // RENDER + SYSTEM UPDATE
    // ============================
    renderer.render_frame(world);
    renderer.update_system(world, Time::delta_time);

    // ============================
    // VIEWPORT & PROJECTION
    // ============================
    glfwGetFramebufferSize(WINDOW, &WIDTH, &HEIGHT);
    glViewport(0, 0, WIDTH, HEIGHT);

    renderer.set_projection(glm::perspective(
      glm::radians(45.0f),
      static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),
      0.1f,
      100.0f
    ));

    // ============================
    // PRESENT
    // ============================
    glfwSwapBuffers(WINDOW);
    glfwPollEvents();

  }

  glfwDestroyWindow(WINDOW);
  glfwTerminate();
  return 0;
}
