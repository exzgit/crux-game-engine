#pragma once

#include <string>
#include <memory>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "recs/world.h"
#include "mesh.h"
#include "transform.h"
#include "material.h"
#include "camera.h"

namespace __RUNTIME__ {
class SystemRenderer {
  public:
	SystemRenderer()
	: _program(0), _uMVP(-1)
	{
		// simple shader sources
		const char* vs = R"(
		#version 330 core

		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aNormal;
		layout (location = 2) in vec2 aTexCoord;

		uniform mat4 u_MVP;

		void main()
		{
			gl_Position = u_MVP * vec4(aPos, 1.0);
		}
		)";

		const char* fs = R"(
		#version 330 core

		out vec4 FragColor;

		uniform vec3 u_AlbedoColor;
		uniform float u_Opacity;

		void main()
		{
			// Use material albedo directly (vertex colors not provided by loader)
			FragColor = vec4(u_AlbedoColor, u_Opacity);
		}
		)";

		auto compile = [](GLenum type, const char* src)->GLuint {
			GLuint s = glCreateShader(type);
			glShaderSource(s, 1, &src, nullptr);
			glCompileShader(s);
			GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
			if (!ok) {
			GLint len = 0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
			std::string log(len, '\0');
			glGetShaderInfoLog(s, len, nullptr, &log[0]);
			std::cerr << "Shader compile error: " << log << std::endl;
			}
			return s;
		};

		GLuint vsid = compile(GL_VERTEX_SHADER, vs);
		GLuint fsid = compile(GL_FRAGMENT_SHADER, fs);
		_program = glCreateProgram();
		glAttachShader(_program, vsid);
		glAttachShader(_program, fsid);
		glLinkProgram(_program);
		GLint ok = 0; glGetProgramiv(_program, GL_LINK_STATUS, &ok);
		if (!ok) {
			GLint len = 0; glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &len);
			std::string log(len, '\0');
			glGetProgramInfoLog(_program, len, nullptr, &log[0]);
			std::cerr << "Program link error: " << log << std::endl;
		}
		glDeleteShader(vsid); glDeleteShader(fsid);

		_uMVP = glGetUniformLocation(_program, "u_MVP");

		// default camera
		_proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
		_view = glm::lookAt(glm::vec3(0.0f,0.0f,3.0f), glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));
	}

	~SystemRenderer() {
		if (_program) glDeleteProgram(_program);
	}

	void render_frame(World& _world) {
		glUseProgram(_program);
		_world.query().for_each<Transform, Mesh, MeshRenderer, Material>(
			[&](Transform& transform, Mesh& mesh, MeshRenderer& mesh_renderer, Material& material) {

			glm::mat4 model(1.0f);
			model = glm::translate(model, transform.position);
			model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1,0,0));
			model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0,1,0));
			model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0,0,1));
			model = glm::scale(model, transform.scale);

			glm::mat4 mvp = _proj * _view * model;
			glUniformMatrix4fv(_uMVP, 1, GL_FALSE, glm::value_ptr(mvp));

			// === MATERIAL BINDING ===
			glUniform3fv(
				glGetUniformLocation(_program, "u_AlbedoColor"),
				1,
				glm::value_ptr(material.albedo_color)
			);

			glUniform1f(
				glGetUniformLocation(_program, "u_Opacity"),
				material.opacity
			);

			mesh_renderer.draw();
			}
		);
		glUseProgram(0);
	}

  void update_system(World& world, float /*delta_time*/) {
    world.query().for_each<Transform, Camera>(
      [&](Transform& transform, Camera& camera) {

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
        set_view_projection(view, projection);
      }
    );
  }

  void set_view_projection(const glm::mat4& view, const glm::mat4& projection)
  {
    _view = view;
    _proj = projection;
  }

  void set_projection(const glm::mat4& proj)
  {
    _proj = proj;
  }

  private:
	GLuint _program;
	GLint _uMVP;
	glm::mat4 _view, _proj;
};
};
