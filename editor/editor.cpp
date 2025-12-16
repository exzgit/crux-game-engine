#include "editor.h"

namespace __EDITOR__ {
  void update_camera_vectors(Camera& cam) {
      glm::vec3 direction;
      direction.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
      direction.y = sin(glm::radians(cam.pitch));
      direction.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));

      cam.forward = glm::normalize(direction);
      cam.right   = glm::normalize(glm::cross(cam.forward, {0.0f, 1.0f, 0.0f}));
      cam.up      = glm::normalize(glm::cross(cam.right, cam.forward));
  }

  CruxEditor::CruxEditor() {
    //
    // Initialize GLFW
    //
    if (!glfwInit()) {
      std::cerr << "Failed to initialize GLFW\n";
      std::exit(1);
    }

    //
    // Set OpenGL context version to 3.3 Core profile
    //
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //
    // If window is nullptr, create a windowed mode window and OpenGL context.
    //
    window = glfwCreateWindow(1280, 720, "Crux Editor", nullptr, nullptr);

    if (!window) {
      std::cerr << "Failed to create GLFWwindow\n";
      glfwTerminate();
      std::exit(1);
    }

    glfwMakeContextCurrent(window);

    //
    // Register framebuffer resize callback
    //
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //
    // Initialize GLAD (OpenGL loader)
    //
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cerr << "Failed to initialize GLAD\n";
      std::exit(1);
    }

    //
    // Setup dear ImGUI context.
    //
    IMGUI_CHECKVERSION();                     // Verify ImGui version compatibility
    ImGui::CreateContext();                   // Create global ImGui context
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // keyboard nav
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // docking support
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // multi-viewports
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    //
    // Apply dark theme colors (can be customized further)
    //
    ImGui::StyleColorsDark();

    //
    // Initialize platform/renderer bindings
    // GLFW for input handling, OpenGL3 for rendering
    //
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");     // GLSL version
  }

  CruxEditor::~CruxEditor() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
  }

  void CruxEditor::camera_editor(Transform& transform, Camera& camera, __RUNTIME__::SystemRenderer& renderer) {
    // ============================
    // CLAMP PITCH (ANTI GIMBAL LOCK)
    // ============================
    camera.pitch = glm::clamp(camera.pitch, -89.0f, 89.0f);

    // ============================
    // BUILD CAMERA DIRECTION
    // ============================
    const float yaw   = glm::radians(camera.yaw);
    const float pitch = glm::radians(camera.pitch);

    glm::vec3 forward;
    forward.x = cosf(pitch) * cosf(yaw);
    forward.y = sinf(pitch);
    forward.z = cosf(pitch) * sinf(yaw);
    camera.forward = glm::normalize(forward);

    // ============================
    // ORTHONORMAL BASIS
    // ============================
    const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    camera.right = glm::normalize(glm::cross(camera.forward, worldUp));
    camera.up    = glm::normalize(glm::cross(camera.right, camera.forward));

    // ============================
    // VIEW MATRIX (LOOK-AT)
    // ============================
    const glm::vec3 eye    = transform.position;
    const glm::vec3 center = eye + camera.forward;

    glm::mat4 view = glm::lookAt(eye, center, camera.up);

    // ============================
    // PROJECTION MATRIX
    // ============================
    glm::mat4 projection;
    if (camera.type == CameraPerspective::Perspective) {
      projection = glm::perspective(
        glm::radians(camera.fov),
        camera.aspect_ratio,
        camera.near_plane,
        camera.far_plane
      );
    } else {
      float ortho = 10.0f;
      projection = glm::ortho(
        -ortho * camera.aspect_ratio,
        ortho * camera.aspect_ratio,
        -ortho,
        ortho,
        camera.near_plane,
        camera.far_plane
      );
    }

    // ============================
    // SUBMIT TO RENDERER
    // ============================
    renderer.set_view_projection(view, projection);
  }

  void CruxEditor::update() {
    // Make ECS
    World world;
    // InputSystem input = InputSystem(window);

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
    renderer.is_editor_view = true;

    auto cube_entity = __TOOLS__::create_entities_from_obj(world, "./engine/assets/Mesh.obj");
    assert(world.alive(cube_entity));
    world.add<Transform>(cube_entity);
    
    world.add<Identity>(cube_entity);
    world.get<Identity>(cube_entity).name = "Cube";
    
    auto cam = Camera();
    auto camera_transform = Transform();
    camera_transform.position.z += 10;

    static bool was_right_mouse_button_clicked = false;
    static float camera_editor_speed = 2.0f;
    static float camera_editor_sensitivity = 0.1f;

    static double last_mouse_x = 0.0;
    static double last_mouse_y = 0.0;

    EditorState state;

    //
    // Make application loop
    //
    while (!glfwWindowShouldClose(window)) {
      // Call update deltatime
      Time::update_deltatime();

      // Poll and handle user input events (keyword, mouse)
      glfwPollEvents();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      // draw_editor_dockspace();
      // draw_main_menu_bar();
      // draw_viewport(state);
      // draw_hierarchy(world, nullptr);
      ImGui::Render();

      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);

      glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      renderer.render_frame(world);
      renderer.update_system(world, Time::delta_time);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  
      bool right_mouse_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
      if (right_mouse_pressed) {
        if (!was_right_mouse_button_clicked) {
          was_right_mouse_button_clicked = true;
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
          double x, y;
          glfwGetCursorPos(window, &x, &y);
          last_mouse_x = x;
          last_mouse_y = y;
        } else {
          double x, y;
          glfwGetCursorPos(window, &x, &y);
          
          double dx = x - last_mouse_x;
          double dy = last_mouse_y - y;

          last_mouse_x = x;
          last_mouse_y = y;

          cam.yaw   += static_cast<float>(dx) * camera_editor_sensitivity;
          cam.pitch += static_cast<float>(dy) * camera_editor_sensitivity;

          cam.pitch = glm::clamp(cam.pitch, -89.0f, 89.0f);

          update_camera_vectors(cam);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
          camera_transform.position += cam.forward * camera_editor_speed * Time::delta_time;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
          camera_transform.position -= cam.forward * camera_editor_speed * Time::delta_time;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
          camera_transform.position -= cam.right * camera_editor_speed * Time::delta_time;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
          camera_transform.position += cam.right * camera_editor_speed * Time::delta_time;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
          camera_transform.position += cam.up * camera_editor_speed * Time::delta_time;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
          camera_transform.position -= cam.up * camera_editor_speed * Time::delta_time;
        
      } else {
        was_right_mouse_button_clicked = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }

      auto& cube_transform = world.get<Transform>(cube_entity);
      cube_transform.rotation.x += 1.0f;

      camera_editor(camera_transform, cam, renderer);
      float aspect = static_cast<float>(display_w) / static_cast<float>(display_h);
      glm::mat4 projection = glm::perspective(glm::radians(cam.fov), aspect, cam.near_plane, cam.far_plane);
      renderer.set_projection(projection);

      glfwSwapBuffers(window);
    }
  }

  void CruxEditor::draw_editor_dockspace() {
    static bool dockspace_open = true;

    ImGuiWindowFlags window_flags = 
      ImGuiWindowFlags_MenuBar                |
      ImGuiWindowFlags_NoDocking              |
      ImGuiWindowFlags_NoTitleBar             |
      ImGuiWindowFlags_NoCollapse             |
      ImGuiWindowFlags_NoResize               |
      ImGuiWindowFlags_NoMove                 |
      ImGuiWindowFlags_NoBringToFrontOnFocus  |
      ImGuiWindowFlags_NoNavFocus             |
      ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("EditorDockspace", &dockspace_open, window_flags);

    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("EditorDockspaceID");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

    ImGui::End();
  }

  void CruxEditor::draw_main_menu_bar() {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        ImGui::MenuItem("New Scene");
        ImGui::MenuItem("Open Scene");
        ImGui::MenuItem("Save Scene");
        ImGui::Separator();
        ImGui::MenuItem("Exit");
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Edit")) {
        ImGui::MenuItem("Undo", "Ctrl+Z");
        ImGui::MenuItem("Redo", "Ctrl+Y");
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Hierarchy");
        ImGui::MenuItem("Inspector");
        ImGui::MenuItem("Viewport");
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }
  }

  void CruxEditor::draw_viewport(EditorState& state) {
    ImGui::Begin("Viewport");

    state.viewport_focused = ImGui::IsWindowFocused();
    state.viewport_hovered = ImGui::IsWindowHovered();

    ImVec2 size = ImGui::GetContentRegionAvail();

    state.viewport_size = glm::vec2(size.x, size.y);

    ImGui::Image(
      (ImTextureID)state.viewport_texture,
      size,
      ImVec2(0, 1),
      ImVec2(1, 0)
    );

    ImGui::End();
  }

  void CruxEditor::draw_hierarchy(World& world, Entity* selected) {
    ImGui::Begin("Hierarchy");

    world.query().for_each_entity<Identity>(
    [&](Entity e, Identity& identity)
    {
      bool is_selected = (selected == &e);

      if (ImGui::Selectable(identity.name.c_str(), is_selected))
        selected = &e;

      if (ImGui::BeginPopupContextItem())
      {
        if (ImGui::MenuItem("Delete"))
          world.destroy_entity(e);
        
        ImGui::EndPopup();
      }
    });

    ImGui::End();
  }
}
