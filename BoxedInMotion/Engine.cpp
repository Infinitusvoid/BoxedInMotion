#include "Engine.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image/stb_image_write.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vector>
#include <filesystem>

namespace Engine
{
	
	
	glm::vec3 g_background_color = glm::vec3(0.0f, 0.0f, 0.0f);

	void(*f_loop_callback)(Instance_data*);
	void(*f_init_callback)(Instance_data*);
	void(*f_on_finish_render_callback)(void);
	bool g_camera_locked = false;

	int g_frame = 0;

	unsigned int g_bloom_iterations_amount = 10;

	unsigned int g_num_boxes = 0;

	float g_zoom_min = 1.0f;
	float g_zoom_max = 45.0f;

	void(*f_callback_mouse_button_left_click)(void);
	void(*f_callback_mouse_button_left_relese)(void);
	void(*f_callback_mouse_button_right_click)(void);
	void(*f_callback_mouse_button_right_relese)(void);

	namespace File
	{
		bool searchFileByName(const std::string& directory, const std::string& targetFileName)
		{
			std::filesystem::path dirPath(directory);

			if (!std::filesystem::is_directory(dirPath))
			{
				std::cerr << "Error: The specified directory does not exist." << std::endl;
				return false;
			}

			for (const auto& entry : std::filesystem::directory_iterator(dirPath))
			{
				if (std::filesystem::is_regular_file(entry) && entry.path().filename() == targetFileName)
				{
					std::cout << "File found: " << entry.path() << std::endl;
					return true;
				}
			}

			std::cout << "File not found: " << targetFileName << std::endl;
			return false;
		}

		void writeFileIfNotExists(const std::string& filename, const std::string& content)
		{
			std::ifstream file(filename);

			if (file.good()) {
				std::cout << "File '" << filename << "' already exists. Not overwriting." << std::endl;
				return; // File already exists, do not overwrite
			}

			std::ofstream newFile(filename);

			if (!newFile.is_open()) {
				std::cerr << "Error: Unable to create the file." << std::endl;
				return;
			}

			newFile << content;
			newFile.close();
			std::cout << "File '" << filename << "' has been created and written." << std::endl;
		}

		void writeFileIfNotExists(const char* filename, const char* content)
		{
			std::ifstream file(filename);

			if (file.good()) {
				std::cout << "File '" << filename << "' already exists. Not overwriting." << std::endl;
				return; // File already exists, do not overwrite
			}

			std::ofstream newFile(filename);

			if (!newFile.is_open()) {
				std::cerr << "Error: Unable to create the file." << std::endl;
				return;
			}

			newFile << content;
			newFile.close();
			std::cout << "File '" << filename << "' has been created and written." << std::endl;
		}
	}

	namespace Shader_sources_build_in
	{
		namespace Box
		{
			namespace Vertex
			{
				const char* build_in_vertex_shader_path = "build_in_vertex_shader.vs";
				const char* build_in_vertex_shader_source =
					"#version 330 core\n"
					"layout (location = 0) in vec3 aPos;\n"
					"\n"
					"// Define the instance data struct\n"
					"struct Instance_data\n"
					"{\n"
					"	mat4 model;\n"
					"	vec4 color;\n"
					"};\n"
					"\n"
					"layout(location = 3) in Instance_data instanceData;\n"
					"\n"
					"out vec4 Color;\n"
					"out vec3 FragWorldPos; // World space position\n"
					"out vec3 FragObjectPos; // Object space position\n"
					"\n"
					"uniform mat4 projection;\n"
					"uniform mat4 view;\n"
					"\n"
					"uniform vec3 camera_position;\n"
					"uniform vec3 camera_front;\n"
					"uniform vec3 camera_right;\n"
					"uniform vec3 camera_up;\n"
					"uniform float camera_zoom;\n"
					"uniform int frame;\n"
					"uniform float time;\n"
					"\n"
					"void main()\n"
					"{\n"
					"	int id = gl_InstanceID;\n"
					"	FragObjectPos = aPos; // Store the object space position\n"
					"	FragWorldPos = vec3(instanceData.model * vec4(aPos, 1.0)); // Calculate world space position\n"
					"	Color = instanceData.color;\n"
					"	gl_Position = projection * view * instanceData.model * vec4(aPos + sin(time) * 10.0, 1.0f);\n"
					"}\n";
			}

			namespace Fragment
			{
				const char* build_in_fragment_shader_path = "build_in_fragment_shader.fs";
				const char* build_in_fragment_shader_source =
					"#version 330 core\n"
					"layout (location = 0) out vec4 FragColor;\n"
					"layout (location = 1) out vec4 BrightColor;"
					"\n"
					"in vec4 Color;\n"
					"in vec3 FragWorldPos; // Input world space position\n"
					"in vec3 FragObjectPos; // Input object space position\n"
					"\n"
					"\n"
					"uniform vec3 camera_position;\n"
					"uniform vec3 camera_front;\n"
					"uniform vec3 camera_right;\n"
					"uniform vec3 camera_up;\n"
					"uniform float camera_zoom;\n"
					"uniform int frame;\n"
					"uniform float time;\n"
					"\n"
					"void main()\n"
					"{\n"
					"   vec4 result = Color + sin(time + FragWorldPos.x * 10.0) * 0.7 + vec4(vec3(sin(FragObjectPos.x * 100), 0.0, 0.0), 1.0);\n"
					"	FragColor = result * 0.7;\n"
					"   float factor_brightnes = dot(vec3(result), vec3(0.2126, 0.7152, 0.0722));\n"
					"   if(factor_brightnes > 1.0) // transhold usually set at 1.0\n"
					"   {\n"
					"     BrightColor = vec4(result.x * 10.0, result.y * 10.0, result.z * 10.0, 1.0);\n"
					"   }\n"
					"   else\n"
					"   {\n"
					"     BrightColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
					"   }\n"
					"\n"
					"}\n";
			}
		}
		
		
		
		void write_them_if_if_they_dont_exist()
		{
			File::writeFileIfNotExists(Box::Vertex::build_in_vertex_shader_path, Box::Vertex::build_in_vertex_shader_source);
			File::writeFileIfNotExists(Box::Fragment::build_in_fragment_shader_path, Box::Fragment::build_in_fragment_shader_source);
		}


	}

	class Shader
	{
	public:
		unsigned int ID;

		Shader(const ShaderSourceCode* source)
		{
			
			// 2. compile shaders
			unsigned int vertex, fragment;
			// vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &source->vertex, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");
			// fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &source->fragment, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");
			// if geometry shader is given, compile geometry shader
			unsigned int geometry;
			if (source->geometry != nullptr)
			{
				geometry = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometry, 1, &source->geometry, NULL);
				glCompileShader(geometry);
				checkCompileErrors(geometry, "GEOMETRY");
			}
			// shader Program
			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			if (source->geometry != nullptr)
			{
				glAttachShader(ID, geometry);
			}
				
			glLinkProgram(ID);
			checkCompileErrors(ID, "PROGRAM");
			// delete the shaders as they're linked into our program now and no longer necessary
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			if (source->geometry != nullptr)
			{
				glDeleteShader(geometry);
			}
				
		}

		// constructor generates the shader on the fly
		// ------------------------------------------------------------------------
		Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
		{
			std::cout << "reading vertexPath : " << vertexPath << "\n";
			std::cout << "reading fragmentPath : " << fragmentPath << "\n";
			// 1. retrieve the vertex/fragment source code from filePath
			std::string vertexCode;
			std::string fragmentCode;
			std::string geometryCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			std::ifstream gShaderFile;
			// ensure ifstream objects can throw exceptions:
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				// open files
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;
				// read file's buffer contents into streams
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				// close file handlers
				vShaderFile.close();
				fShaderFile.close();
				// convert stream into string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
				// if geometry shader path is present, also load a geometry shader
				if (geometryPath != nullptr)
				{
					gShaderFile.open(geometryPath);
					std::stringstream gShaderStream;
					gShaderStream << gShaderFile.rdbuf();
					gShaderFile.close();
					geometryCode = gShaderStream.str();
				}
			}
			catch (std::ifstream::failure& e)
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
			}
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();
			// 2. compile shaders
			unsigned int vertex, fragment;
			// vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");
			// fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");
			// if geometry shader is given, compile geometry shader
			unsigned int geometry;
			if (geometryPath != nullptr)
			{
				const char* gShaderCode = geometryCode.c_str();
				geometry = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometry, 1, &gShaderCode, NULL);
				glCompileShader(geometry);
				checkCompileErrors(geometry, "GEOMETRY");
			}
			// shader Program
			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			if (geometryPath != nullptr)
				glAttachShader(ID, geometry);
			glLinkProgram(ID);
			checkCompileErrors(ID, "PROGRAM");
			// delete the shaders as they're linked into our program now and no longer necessary
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			if (geometryPath != nullptr)
				glDeleteShader(geometry);

		}
		// activate the shader
		// ------------------------------------------------------------------------
		void use()
		{
			glUseProgram(ID);
		}
		// utility uniform functions
		// ------------------------------------------------------------------------
		void setBool(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
		}
		// ------------------------------------------------------------------------
		void setInt(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}
		// ------------------------------------------------------------------------
		void setFloat(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}
		// ------------------------------------------------------------------------
		void setVec2(const std::string& name, const glm::vec2& value) const
		{
			glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec2(const std::string& name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
		}
		// ------------------------------------------------------------------------
		void setVec3(const std::string& name, const glm::vec3& value) const
		{
			glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec3(const std::string& name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
		}
		// ------------------------------------------------------------------------
		void setVec4(const std::string& name, const glm::vec4& value) const
		{
			glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec4(const std::string& name, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
		}
		// ------------------------------------------------------------------------
		void setMat2(const std::string& name, const glm::mat2& mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat3(const std::string& name, const glm::mat3& mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat4(const std::string& name, const glm::mat4& mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

	private:
		// utility function for checking shader compilation/linking errors.
		// ------------------------------------------------------------------------
		void checkCompileErrors(GLuint shader, std::string type)
		{
			GLint success;
			GLchar infoLog[1024];
			if (type != "PROGRAM")
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
		}
	};

	// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	namespace Constants
	{
		// Default camera values
		const float YAW = -90.0f;
		const float PITCH = 0.0f;
		const float SPEED = 2.5f;
		const float SENSITIVITY = 0.1f;
		const float ZOOM = 45.0f;
	}

	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera
	{
	public:
		// camera Attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		// euler Angles
		float Yaw;
		float Pitch;
		// camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;
		
		// constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = Constants::YAW, float pitch = Constants::PITCH) :
			Front(glm::vec3(0.0f, 0.0f, -1.0f)),
			MovementSpeed(Constants::SPEED),
			MouseSensitivity(Constants::SENSITIVITY),
			Zoom(Constants::ZOOM)
		{
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}

		void set_camera_parameters(glm::vec3 set_position, float set_speed, float set_sensitivy, float set_zoom)
		{
			Position = set_position;
			WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
			Yaw = Constants::YAW;
			Pitch = Constants::PITCH;

			Front = glm::vec3(0.0f, 0.0f, -1.0f);
			MovementSpeed = set_speed;
			MouseSensitivity = set_sensitivy;
			Zoom = set_zoom;
			updateCameraVectors();
		}

		// constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
			Front(glm::vec3(0.0f, 0.0f, -1.0f)),
			MovementSpeed(Constants::SPEED),
			MouseSensitivity(Constants::SENSITIVITY),
			Zoom(Constants::ZOOM)
		{
			Position = glm::vec3(posX, posY, posZ);
			WorldUp = glm::vec3(upX, upY, upZ);
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}

		// returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix()
		{
			return glm::lookAt(Position, Position + Front, Up);
		}

		// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(Camera_Movement direction, float deltaTime)
		{
			float velocity = MovementSpeed * deltaTime;
			if (direction == FORWARD)
				Position += Front * velocity;
			if (direction == BACKWARD)
				Position -= Front * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
		}

		// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}

			// update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset)
		{
			Zoom -= (float)yoffset;

			if (Zoom < g_zoom_min)
			{
				Zoom = g_zoom_min;
			}

			if (Zoom > g_zoom_max)
			{
				Zoom = g_zoom_max;
			}

		}

	private:
		// calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors()
		{
			// calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
	};

	struct Vertex
	{
		glm::vec3 Position;
	};

	struct Texture
	{
		unsigned int id;

		void bind()
		{
			glBindTexture(GL_TEXTURE_2D, id);
		}
	};

	class Mesh
	{
	public:
		// mesh Data
		std::vector<Vertex>       vertices;
		std::vector<unsigned int> indices;
		unsigned int VAO;

		// constructor
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
		{
			this->vertices = vertices;
			this->indices = indices;

			// now that we have all the required data, set the vertex buffers and its attribute pointers.
			setupMesh();
		}

		// render the mesh
		void Draw(Shader& shader)
		{
			// draw mesh
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

	private:
		// render data 
		unsigned int VBO, EBO;

		// initializes all the buffer objects/arrays
		void setupMesh()
		{
			// create buffers/arrays
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			// load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// set the vertex attribute pointers
			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		}
	};


	class Model
	{
	public:
		// model data 
		//std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		Mesh* mesh;

		// constructor, expects a filepath to a 3D model.
		Model()
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			auto GenerateCubeData = [&]()
			{
				// Define the vertices of the cube
				glm::vec3 cubeVertices[] = {
					// Front face
					glm::vec3(-1.0f, -1.0f,  1.0f),
					glm::vec3(1.0f, -1.0f,  1.0f),
					glm::vec3(1.0f,  1.0f,  1.0f),
					glm::vec3(-1.0f,  1.0f,  1.0f),

					// Back face
					glm::vec3(-1.0f, -1.0f, -1.0f),
					glm::vec3(1.0f, -1.0f, -1.0f),
					glm::vec3(1.0f,  1.0f, -1.0f),
					glm::vec3(-1.0f,  1.0f, -1.0f)
				};

				// Define the indices to create the faces of the cube
				unsigned int cubeIndices[] = {
					// Front face
					0, 1, 2,
					2, 3, 0,

					// Back face
					4, 5, 6,
					6, 7, 4,

					// Left face
					0, 3, 7,
					7, 4, 0,

					// Right face
					1, 5, 6,
					6, 2, 1,

					// Top face
					3, 2, 6,
					6, 7, 3,

					// Bottom face
					0, 1, 5,
					5, 4, 0
				};

				// Fill the vertices vector
				vertices.insert(vertices.end(), cubeVertices, cubeVertices + 8);

				// Fill the indices vector
				indices.insert(indices.end(), cubeIndices, cubeIndices + 36);
			};

			GenerateCubeData();

			mesh = new Mesh(vertices, indices);
		}

		// draws the model, and thus all its meshes
		void Draw(Shader& shader)
		{
			mesh->Draw(shader);
		}

	};

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	//unsigned int loadTexture(const char* path, bool gammaCorrection);
	void renderQuad();
	void renderCube();

	// settings
#define FULL_SCREEN_MODE

#ifdef FULL_SCREEN_MODE
	const unsigned int SCR_WIDTH = 1920 * 2;
	const unsigned int SCR_HEIGHT = 1080 * 2;
#else
	const unsigned int SCR_WIDTH = 1920 * 1;
	const unsigned int SCR_HEIGHT = 1080 * 1;
#endif

	bool bloom = true;
	bool bloomKeyPressed = false;
	float g_exposure = 1.0f;

	// camera
	Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
	float lastX = (float)SCR_WIDTH / 2.0;
	float lastY = (float)SCR_HEIGHT / 2.0;
	bool firstMouse = true;

	struct Window
	{
		struct Timer
		{
			// timing
			float deltaTime = 0.0f;
			float lastFrame = 0.0f;
			float total = 0.0;
		};

		GLFWwindow* window;
		Timer timer;

		glm::vec3 backround;

		
		bool init()
		{
			{
				{
					std::string source = "#version 330 core\n"
						"layout(location = 0) in vec3 aPos;\n"
						"layout(location = 1) in vec2 aTexCoords;\n"
						"\n"
						"out vec2 TexCoords;\n"
						"\n"
						"void main()\n"
						"{\n"
						"TexCoords = aTexCoords;\n"
						"gl_Position = vec4(aPos, 1.0);\n"
						"}\n";
					File::writeFileIfNotExists("bloom_final_vs.txt", source);
				}

				{
					std::string source = "#version 330 core\n"
						"out vec4 FragColor;\n"
						"\n"
						"in vec2 TexCoords;\n"
						"\n"
						"uniform sampler2D scene;\n"
						"uniform sampler2D bloomBlur;\n"
						"uniform bool bloom;\n"
						"uniform float exposure;\n"
						"\n"
						"void main()\n"
						"{\n"
						"const float gamma = 2.2;\n"
						"vec3 hdrColor = texture(scene, TexCoords).rgb;\n"
						"vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;\n"
						"if (bloom)\n"
						"hdrColor += bloomColor; // additive blending\n"
						"// tone mapping\n"
						"vec3 result = vec3(1.0) - exp(-hdrColor * exposure);\n"
						"// also gamma correct while we're at it       \n"
						"result = pow(result, vec3(1.0 / gamma));\n"
						"FragColor = vec4(result, 1.0);\n"
						"}\n";
					File::writeFileIfNotExists("bloom_final_fs.txt", source);
				}

				

				{
					std::string source = "#version 330 core\n"
						"layout(location = 0) in vec3 aPos;\n"
					"layout(location = 1) in vec2 aTexCoords;\n"
						"\n"
					"out vec2 TexCoords;\n"
						"\n"
					"void main()\n"
					"{\n"
						"TexCoords = aTexCoords;\n"
						"gl_Position = vec4(aPos, 1.0);\n"
					"}\n";
					File::writeFileIfNotExists("blur_vs.txt", source);
				}

				{
					std::string source = "#version 330 core\n"
						"out vec4 FragColor;\n"
						"\n"
					"in vec2 TexCoords;\n"
						"\n"
					"uniform sampler2D image;\n"
						"\n"
					"uniform bool horizontal;\n"
					"uniform float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);\n"
						"\n"
					"void main()\n"
					"{\n"
						"vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel\n"
						"vec3 result = texture(image, TexCoords).rgb * weight[0];\n"
						"if (horizontal)\n"
						"{\n"
							"for (int i = 1; i < 5; ++i)\n"
							"{\n"
								"result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
								"result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
							"}\n"
						"}\n"
						"else\n"
						"{\n"
							"for (int i = 1; i < 5; ++i)\n"
							"{\n"
								"result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
								"result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
							"}\n"
						"}\n"
						"FragColor = vec4(result, 1.0);\n"
					"}\n";
					File::writeFileIfNotExists("blur_fs.txt", source);
				}
			}


			// glfw: initialize and configure
			// ------------------------------
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif



#ifdef FULL_SCREEN_MODE
			// Get the primary monitor
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

			// glfw window creation
			// --------------------
			// Use the monitor's resolution for full screen
			window = glfwCreateWindow(mode->width, mode->height, "LearnOpenGL", primaryMonitor, NULL);

			if (window == NULL)
			{
				std::cout << "Failed to create GLFW window" << std::endl;
				glfwTerminate();
				return false;
			}

#else
			// glfw window creation
			// --------------------
			window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
			if (window == NULL)
			{
				std::cout << "Failed to create GLFW window" << std::endl;
				glfwTerminate();
				return false;
			}

#endif

			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetScrollCallback(window, scroll_callback);
			glfwSetMouseButtonCallback(window, mouseButtonCallback);

			// tell GLFW to capture our mouse
#ifdef FULL_SCREEN_MODE
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
			// glad: load all OpenGL function pointers
			// ---------------------------------------
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				std::cout << "Failed to initialize GLAD" << std::endl;
				return -1;
			}

			// configure global opengl state
			// -----------------------------
			glEnable(GL_DEPTH_TEST);

		}

		void swap_buffers_and_poll_IO_events()
		{
			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		bool continue_the_loop()
		{
			return !glfwWindowShouldClose(window);
		}

		void loop_0()
		{
			// per-frame time logic
			// --------------------
			float currentFrame = static_cast<float>(glfwGetTime());
			timer.deltaTime = currentFrame - timer.lastFrame;
			timer.lastFrame = currentFrame;
			timer.total += timer.deltaTime;
		}

		void process_input()
		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, true);
			}
				

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				camera.ProcessKeyboard(FORWARD, timer.deltaTime);
			}
				
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				camera.ProcessKeyboard(BACKWARD, timer.deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				camera.ProcessKeyboard(LEFT, timer.deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				camera.ProcessKeyboard(RIGHT, timer.deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
			{
				bloom = !bloom;
				bloomKeyPressed = true;
			}

			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
			{
				bloomKeyPressed = false;
			}

			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			{
				if (g_exposure > 0.0f)
				{
					g_exposure -= 0.001f;
				}
				else
				{
					g_exposure = 0.0f;
				}
					
			}
			else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			{
				g_exposure += 0.001f;
			}

			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			{
				// camera lock
				g_camera_locked = true;
			}

			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
			{
				// camera unlock
				g_camera_locked = false;
			}

			if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			{
				// relese mouse
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			{
				// lock mouse
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}

		void clear()
		{
			// render
			// ------
			glClearColor(g_background_color.r, g_background_color.g, g_background_color.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

	};

	Window win;

	struct Framebuffer
	{
		unsigned int id;
	};

	namespace Framebuffer_
	{
		void create(Framebuffer& framebuffer)
		{
			glGenFramebuffers(1, &framebuffer.id);
		}

		void bind(Framebuffer& framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
		}

		void unbind(Framebuffer& framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

	}

	// Function to capture and save the framebuffer to a file
	void captureAndSaveFramebuffer(const std::string& filename, int width, int height)
	{
		// Create an array to store the pixel data
		unsigned char* pixels = new unsigned char[3 * width * height];

		// Read the pixel data from the framebuffer
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		// Flip the image vertically (OpenGL stores the image upside down)
		for (int i = 0; i < height / 2; i++)
		{
			for (int j = 0; j < width * 3; j++)
			{
				std::swap(pixels[i * width * 3 + j], pixels[(height - i - 1) * width * 3 + j]);
			}
		}

		// Save the pixel data to an image file using stb_image_write
		if (stbi_write_png(filename.c_str(), width, height, 3, pixels, 0) == 0)
		{
			std::cerr << "Error saving image to file: " << filename << std::endl;
		}
		else
		{
			std::cout << "Framebuffer captured and saved to " << filename << std::endl;
		}

		// Clean up memory
		delete[] pixels;
	}

	glm::vec3 generate_random_glm_vec3(const glm::vec3& min_value, const glm::vec3& max_value)
	{
		glm::vec3 result;
		result.x = min_value.x + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.x - min_value.x);
		result.y = min_value.y + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.y - min_value.y);
		result.z = min_value.z + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.z - min_value.z);
		return result;
	}

	float generate_random_float(const float min_value, const float max_value)
	{
		return min_value + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value - min_value);
	}

	// https://gist.github.com/Infinitusvoid/00d0fb2e7209f26db0a15e563e95f162/revisions

	struct Boxes
	{
		float factor = 0.0f;

		Instance_data* instance_data = nullptr;

		Shader* shader = nullptr;
		Model* model = nullptr;
		ShaderSourceCode* shader_source_code = nullptr;

		unsigned int buffer;
	public:

		void init()
		{
			Shader_sources_build_in::write_them_if_if_they_dont_exist();

			if (shader_source_code == nullptr)
			{
				shader = new Shader(Shader_sources_build_in::Box::Vertex::build_in_vertex_shader_path, Shader_sources_build_in::Box::Fragment::build_in_fragment_shader_path);
			}
			else
			{
				shader = new Shader(shader_source_code);
			}

			
			model = new Model();
			instance_data = new Instance_data[g_num_boxes];
			factor = 1.0f / g_num_boxes;

			f_init_callback(instance_data);

			// configure instanced array
			// -------------------------

			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, g_num_boxes * sizeof(Instance_data), &instance_data[0], GL_STATIC_DRAW);

			{
				unsigned int VAO = model->mesh->VAO;
				glBindVertexArray(VAO);
				// set attribute pointers for matrix (4 times vec4)
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_data), (void*)0);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_data), (void*)(sizeof(glm::vec4)));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_data), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_data), (void*)(3 * sizeof(glm::vec4)));

				glEnableVertexAttribArray(7);
				glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_data), (void*)(4 * sizeof(glm::vec4)));

				glVertexAttribDivisor(3, 1);
				glVertexAttribDivisor(4, 1);
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);

				glVertexAttribDivisor(7, 1);

				glBindVertexArray(0);
			}

		}

		void free()
		{
			delete shader;
			delete model;
		}

		void draw(glm::mat4 projection, glm::mat4 view)
		{
			camera.Position;
			
			shader->use();
			shader->setMat4("projection", projection);
			shader->setMat4("view", view);
			shader->setVec3("camera_position", camera.Position);
			shader->setVec3("camera_front", camera.Front);
			shader->setVec3("camera_right", camera.Right);
			shader->setVec3("camera_up", camera.Up);
			shader->setFloat("camera_zoom", camera.Zoom);
			shader->setInt("frame", g_frame);
			shader->setFloat("time", win.timer.total);

			glActiveTexture(GL_TEXTURE0);
			
			{
				glBindVertexArray(model->mesh->VAO);
				glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(model->mesh->indices.size()), GL_UNSIGNED_INT, 0, g_num_boxes);
				glBindVertexArray(0);
			}

			update();

		}

	private:

		void update()
		{
			f_loop_callback(instance_data);

			// upload
			{
				glBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBufferData(GL_ARRAY_BUFFER, g_num_boxes * sizeof(Instance_data), &instance_data[0], GL_STATIC_DRAW);
			}
		}

	};

	void set_background_color(glm::vec3 background_color)
	{
		g_background_color = background_color;
	}

	void set_exposure(float value)
	{
		g_exposure = value;
	}

	void set_bloom_iteration(int num_iterations)
	{
		g_bloom_iterations_amount = num_iterations;
	}

	float get_dt()
	{
		return win.timer.deltaTime;
	}

	float get_total_time()
	{
		return win.timer.total;
	}

	int get_frame()
	{
		return g_frame;
	}

	glm::vec3 get_camera_position()
	{
		return camera.Position;
	}

	glm::vec3 get_camera_front()
	{
		return camera.Front;
	}

	glm::vec3 get_camera_right()
	{
		return camera.Right;
	}

	glm::vec3 get_camera_up()
	{
		return camera.Up;
	}

	float get_camera_zoom()
	{
		return camera.Zoom;
	}

	void set_camera_parameters(glm::vec3 set_position, float set_speed, float set_sensitivy, float set_zoom)
	{
		camera.set_camera_parameters(set_position, set_speed, set_sensitivy, set_zoom);
	}

	void set_camera_position(glm::vec3 position)
	{
		camera.Position = position;
	}

	void lock_camera()
	{
		g_camera_locked = true;
	}

	void unlock_camera()
	{
		g_camera_locked = false;
	}

	int play(unsigned int number_of_boxes, void(*f_init)(Instance_data*), void(*f_loop)(Instance_data*), float fov, float view_distance, ShaderSourceCode* shader_source_code)
	{
		std::cout << "f_boxes_engine_start\n";

		f_init_callback = f_init;
		f_loop_callback = f_loop;

		assert(f_init != nullptr);
		assert(f_loop != nullptr);

		g_num_boxes = number_of_boxes;
		assert(g_num_boxes > 0);

		g_zoom_max = fov;

		std::cout << "       Instructions       \n";
		std::cout << " -------------------------\n";
		std::cout << " 1     : capture mouse    \n";
		std::cout << " 2     : release mouse    \n";
		std::cout << " l     : lock camera      \n";
		std::cout << " o     : unlock camera    \n";
		std::cout << " space : bloom on / off   \n";
		std::cout << " q     : exposure decrise \n";
		std::cout << " e     : exposure incrise \n";
		std::cout << " space : blook on / off   \n";
		std::cout << "--------------------------\n";
		std::cout << "\n";

		if (!win.init())
		{
			return -1;
		}

		// build and compile shaders
		// -------------------------
		//Shader shaderLight("7.bloom.vs", "7.light_box.fs");
		Shader shaderBlur("blur_vs.txt", "blur_fs.txt");
		
		Shader shaderBloomFinal("bloom_final_vs.txt", "bloom_final_fs.txt");

		// configure (floating point) framebuffers
		// ---------------------------------------
		Framebuffer hdrFBO;
		Framebuffer_::create(hdrFBO);
		Framebuffer_::bind(hdrFBO);
		// create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
		unsigned int colorBuffers[2];
		glGenTextures(2, colorBuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}
		// create and attach depth buffer (renderbuffer)
		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Framebuffer not complete!" << std::endl;
		}
		Framebuffer_::unbind(hdrFBO);
		// ping-pong-framebuffer for blurring
		unsigned int pingpongFBO[2];
		unsigned int pingpongColorbuffers[2];
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongColorbuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}

		// shader configuration
		// --------------------
		shaderBlur.use();
		shaderBlur.setInt("image", 0);
		shaderBloomFinal.use();
		shaderBloomFinal.setInt("scene", 0);
		shaderBloomFinal.setInt("bloomBlur", 1);




		Boxes boxes;
		boxes.shader_source_code = shader_source_code;
		boxes.init();

		// render loop
		// -----------
		while (win.continue_the_loop())
		{
			win.loop_0();
			win.process_input();
			win.clear();

			// 1. render scene into floating point framebuffer
			// -----------------------------------------------
			Framebuffer_::bind(hdrFBO);//glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, view_distance);
			glm::mat4 view = camera.GetViewMatrix();
			//glm::mat4 model = glm::mat4(1.0f);

			

			boxes.draw(projection, view);


			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 2. blur bright fragments with two-pass Gaussian Blur 
			// --------------------------------------------------
			bool horizontal = true, first_iteration = true;
			shaderBlur.use();
			for (unsigned int i = 0; i < g_bloom_iterations_amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				shaderBlur.setInt("horizontal", horizontal);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
				renderQuad();
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
			// --------------------------------------------------------------------------------------------------------------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderBloomFinal.use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
			shaderBloomFinal.setInt("bloom", bloom);
			shaderBloomFinal.setFloat("exposure", g_exposure);
			renderQuad();


			if (f_on_finish_render_callback != nullptr)
			{
				f_on_finish_render_callback();
			}

			if (false)
			{
				captureAndSaveFramebuffer(("C:/Users/Cosmos/Desktop/output/output_" + std::to_string(g_frame) + ".png").c_str(), SCR_WIDTH, SCR_HEIGHT);
			}

			if (false)
			{
				std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << g_exposure << std::endl;
			}


			win.swap_buffers_and_poll_IO_events();
			g_frame++;
		}

		
		glfwTerminate();


		return 0;
	}

	void captureAndSaveFrameBuffer(const char* path)
	{
		captureAndSaveFramebuffer(path, SCR_WIDTH, SCR_HEIGHT);
	}

	void set_callback_on_finish_render_callback(void(*f)(void))
	{
		f_on_finish_render_callback = f;
	}


	void set_callback_mouse_button_left_click(void(*f)(void))
	{
		f_callback_mouse_button_left_click = f;
	}

	void set_callback_mouse_button_left_relese(void(*f)(void))
	{
		f_callback_mouse_button_left_relese = f;
	}

	void set_callback_mouse_button_right_click(void(*f)(void))
	{
		f_callback_mouse_button_right_click = f;
	}

	void set_callback_mouse_button_right_relese(void(*f)(void))
	{
		f_callback_mouse_button_right_relese = f;
	}

	// renderQuad() renders a 1x1 XY quad in NDC
	// -----------------------------------------
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	// glfw: whenever the mouse moves, this callback is called
	// -------------------------------------------------------
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		if (g_camera_locked == false)
		{
			camera.ProcessMouseMovement(xoffset, yoffset);
		}

	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (g_camera_locked == false)
		{
			camera.ProcessMouseScroll(static_cast<float>(yoffset));
		}

	}

	// Callback function to handle mouse button events
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				if (f_callback_mouse_button_left_click != nullptr)
				{
					//std::cout << "Left mouse button clicked." << std::endl;
					f_callback_mouse_button_left_click();
				}

			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				if (f_callback_mouse_button_right_click != nullptr)
				{
					//std::cout << "Right mouse button clicked." << std::endl;
					f_callback_mouse_button_right_click();
				}

			}
		}

		if (action == GLFW_RELEASE)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				if (f_callback_mouse_button_left_relese != nullptr)
				{
					//std::cout << "Left mouse button relesed." << std::endl;
					f_callback_mouse_button_left_relese();
				}

			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				if (f_callback_mouse_button_right_relese != nullptr)
				{
					//std::cout << "Right mouse button relesed." << std::endl;
					f_callback_mouse_button_right_relese();
				}

			}
		}
	}
}