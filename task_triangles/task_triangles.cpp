// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Triangles Task", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID_1 = LoadShaders("SimpleVertex.vertexshader", "SimpleFragment1.fragmentshader");
	GLuint programID_2 = LoadShaders("SimpleVertex.vertexshader", "SimpleFragment2.fragmentshader");

	GLuint MatrixID_1 = glGetUniformLocation(programID_1, "MVP");
	GLuint MatrixID_2 = glGetUniformLocation(programID_2, "MVP");


	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,
		 0.5f,  0.0f, 0.0f,
		 0.5f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glm::mat4 ModelMtx = glm::mat4(1.0f);
	glm::mat4 ViewMtx = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 ProjectionMtx = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 MVP = ProjectionMtx * ViewMtx * ModelMtx;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float cur_x = 1.9;
	float cur_y = 3;
	float cur_z = 1.9;
	int n_iterations = 0;

	do{
		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		glm::mat4 ViewMtx = glm::lookAt(
			glm::vec3(cur_x, cur_y, cur_z),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		glm::mat4 MVP = ProjectionMtx * ViewMtx * ModelMtx;

        if (n_iterations < 200) {
			cur_x -= 0.02;
		}
		else if (n_iterations < 400) {
			cur_z -= 0.02;
		}
		else if (n_iterations < 600) {
			cur_x += 0.02;
		}
		else if (n_iterations < 800) {
			cur_z += 0.02;
		}
		else {
			n_iterations = 0;
		}

		// 1st triangle
		glUseProgram(programID_1);
		glUniformMatrix4fv(MatrixID_1, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 3); 

		// 2nd triangle
		glUseProgram(programID_2);
		glUniformMatrix4fv(MatrixID_2, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 3, 3);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		n_iterations += 1;
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID_1);
	glDeleteProgram(programID_2);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

