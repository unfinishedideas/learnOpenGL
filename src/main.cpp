#include <iostream>

// external libs
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

// declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


// these will be moved eventually!!!!!!!!!
const char* vertexShaderSource = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";


const char* fragmentShaderSource =
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";


int main() 
{
	// create a GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // <== this is for MAC OS. We are Windows only for now

	GLFWwindow* window = glfwCreateWindow(800, 600, "Pete Learns OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// init GLAD (glad manages function pointers for OpenGL)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;	
	}

	// match OpenGL's window to the GLFW one. Could be smaller if we wanted to make superman 64 :P
	glViewport(0, 0, 800, 600);
	// tell GLFW to call the resize-callback when changing the window size so that OpenGL's viewport matches GLFW
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ITS TRIANGLE TIME
		// float vertices[] = {
		//	-0.5f, -0.5f, 0.0f,
		//	 0.5f, -0.5f, 0.0f,
		//	 0.0f,  0.5f, 0.0f
		//};

	// This is more like rectangle time!
	float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
	};
	// we use indices to refer to the unique points to account for overlapping triangles
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	// Create VERTEX BUFFER OBJECT
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// bind our newly created buffer to the GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// CREATING THE SHADERS! ///////////////////////////////////////////////////////////////////
	// compiling our vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// compiling our fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// make sure the vertex shader compiled
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// make sure the fragment shader compiled
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// CREATING THE SHADER PROGRAM //////////////////////////////////////////////////////////////
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// make sure the shaderProgram properly linked the shaders
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::FAILED_TO_LINK\n" << infoLog << std::endl;
	}

	glUseProgram(shaderProgram);

	// clean up the now unused shader objects (as they are loaded into the program)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Linking vertex attributes... telling OpenGL what the data we gave it actually is
	// 1st param from shader (location 0), 2nd: size of vertex attrib (vec3), 3rd: data type, 4th: do we want to normalize data?
	// 5th: the stride. In this case we know that each vertex takes up 3 float values, 6th: uses a void* and specifies the offset of the first component of the first vertex
	// attribute in the array.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Setting up a VERTEX ARRAY OBJECT (VAO)
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// I dont know anymore
	// 1: bind VAO
	glBindVertexArray(VAO);
	// 2: copy our verticies array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3: set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// From the tutorial: And that is it! Everything we did the last few million pages led up to this moment, 
	// a VAO that stores our vertex attribute configuration and which VBO to use. Usually when you have multiple objects you want to draw, 
	// you first generate/configure all the VAOs (and thus the required VBO and attribute pointers) and store those for later use. 
	// The moment we want to draw one of our objects, we take the corresponding VAO, bind it, then draw the object and unbind the VAO again. 

	// For the rectangle, generate the Element Buffer Object (EBO)
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	// bind the ebo to the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//  THE RENDER LOOP
	// |---------------|
	while (!glfwWindowShouldClose(window))
	{
		// check input
		processInput(window);

		// rendering commands here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render the triangle plz
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		// original call for just the Triangle
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		// new call for the rectangle
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clean up
	glfwTerminate();

	return 0;
}


// resizing window callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// handle input
void processInput(GLFWwindow *window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}