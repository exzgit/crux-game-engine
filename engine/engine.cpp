#include "engine.h"

namespace __ENGINE__ {
  CruxEngine::CruxEngine() {}
  CruxEngine::~CruxEngine() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void CruxEngine::init() {
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
      title.c_str(),
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
    window = WINDOW;

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



    //
    // Add the physics system to the world.
    // This system handles basic rigidbody integration and naive collision detection.
    //

    world.add_system < __RUNTIME__::PhysicsSystem > ();

    //
    // Initialize the rendering system.
    // The SystemRenderer is responsible for drawing all entities that have renderable components.
    //
    renderer = __RUNTIME__::SystemRenderer();
  }

  void CruxEngine::update() {
    // ============================
    // FRAME TIMING
    // ============================
    Time::update_deltatime();

    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ============================
    // PRESENT
    // ============================
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
