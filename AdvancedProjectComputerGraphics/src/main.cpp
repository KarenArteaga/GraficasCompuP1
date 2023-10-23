#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <iostream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

static void error_callback( int error, const char* description ) {
	fprintf( stderr, "Error: %s\n", description );
}

static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );
}

int SCR_WIDTH = 640;
int SCR_HEIGHT = 960;

struct figure 
{ 
	float refX = 0.0f; 
	float refY = 0.0f; 
	figure() {}
	figure(float X, float Y): refX(X), refY(Y) {}
};

int main( void ) {
	GLFWwindow* window;

	glfwSetErrorCallback( error_callback );

	if( !glfwInit() )
		exit( EXIT_FAILURE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );

	window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "Project 1: KA & MH", NULL, NULL );
	if( !window ) {
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback( window, key_callback );

	glfwMakeContextCurrent( window );
	gladLoadGL();
	glfwSwapInterval( 1 );

	// How OpenGL will work with the format of textures
	GLCall(glEnable(GL_BLEND));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	Shader mainShader("res/shaders/template.vs", "res/shaders/template.fs");
	Renderer renderer;

	std::vector<float> fondo = {
		// posicion                               // color
		21.3f, 938.7f, 0.13f, 1.0f,        0.0f, 0.0f, 0.0f, 1.0f,   // top left
		21.3f, 21.3f, 0.13f, 1.0f,         0.0f, 0.0f, 0.0f, 1.0f,   // bottom left
		618.7f, 938.7f, 0.13f, 1.0f,       0.0f, 0.0f, 0.0f, 1.0f,   // top right
		618.7f, 21.3f, 0.13f, 1.0f,        0.0f, 0.0f, 0.0f, 1.0f    // bottom right
	};
	// 0.9569f, 0.9412f, 0.8980f, 1.0f
	unsigned int indices[] = {
		0, 2, 1,
		1, 2, 3
	};

	figure back1;
	back1.refX = -500.0f; back1.refY = 63.9f;
	//back1.refX = 21.3f; back1.refY = 63.9f;
	figure back2;
	//back2.refX = 437.6f; back2.refY = 170.4;
	back2.refX = -83.7f; back2.refY = 170.4;

	std::vector<float> fondo2;
	renderer.drawEllipse(back1.refX, back1.refY, 340.0f, 681.6f, 0.11f, fondo2, 0.9725f, 0.8509f, 0.7255f, 0.0f, 3.14159 * 0.5);
	renderer.drawEllipse(back2.refX, back2.refY, 149.1f, 511.2f, 0.11f, fondo2, 0.8941f, 0.5804f, 0.3137f, 0.0f, 3.14159 * 0.5);

	std::vector<float> miniCirculos;
	figure mCir1(-100.0f, 45.0f), mCir2(-174.55f, 45.0f);
	renderer.drawEllipse(mCir1.refX, mCir1.refY, 32.0f, 32.0f, 0.12f, miniCirculos, 0.9098f, 0.5490f, 0.2706f, 0.0f, 2 * 3.14159);
	renderer.drawEllipse(mCir2.refX, mCir2.refY, 32.0f, 32.0f, 0.12f, miniCirculos, 0.9098f, 0.5490f, 0.2706f, 0.0f, 2 * 3.14159);

	VertexArray va1, va2, va3;
	VertexBufferLayout layout1, layout2, layout3;

	VertexBuffer vb1(fondo.data(), sizeof(float) * fondo.size());
	IndexBuffer ib1(indices, sizeof(indices));

	VertexBuffer vb2(fondo2.data(), sizeof(float) * fondo2.size());
	VertexBuffer vb3(miniCirculos.data(), sizeof(float) * miniCirculos.size());

	// Add a push per every layer that you add to VertexBuffer, the param is de number of
	// positions that use your layer
	// Each layer represents a level, can be a coordinate, color or texture
	layout1.push<float>(4);
	layout1.push<float>(4);
	layout2.push<float>(4);
	layout2.push<float>(4);
	layout3.push<float>(4);
	layout3.push<float>(4);

	va1.addBuffer(vb1, layout1);
	va2.addBuffer(vb2, layout2);
	va3.addBuffer(vb3, layout3);

	glPolygonMode( GL_FRONT, GL_FILL );

	glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, -1.0f, 1.0f);
	glm::mat4 transform1 = glm::mat4(1.0f);
	glm::mat4 transform2 = glm::mat4(1.0f);
	glm::mat4 transform3 = glm::mat4(1.0f);

	time_t seconds;
	seconds = time(NULL);
	int passed_seconds;

	while( !glfwWindowShouldClose( window ) ) {
		glfwGetFramebufferSize( window, &SCR_WIDTH, &SCR_HEIGHT );
		glViewport( 0, 0, SCR_WIDTH, SCR_HEIGHT);

		renderer.clear();
		mainShader.bind();

		mainShader.SetuniformsMat4f("u_Projection", projection);
		mainShader.SetuniformsMat4f("u_Transformation", transform1);

		renderer.draw(va1, ib1, mainShader);

		passed_seconds = time(NULL) - seconds;

		mainShader.SetuniformsMat4f("u_Transformation", transform2);
		if (back1.refX < 21.3)
		{
			transform2 = glm::translate(transform2, glm::vec3(5.0f, 0.0f, 0.0f));
			back1.refX += 5.0f;
			mainShader.SetuniformsMat4f("u_Transformation", transform2);
			renderer.draw(va2, mainShader, fondo2.size(), 0);
		}
		renderer.draw(va2, mainShader, fondo2.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform3);
		if (passed_seconds % 80 > 3)
		{
			transform3 = glm::translate(transform3, glm::vec3(5.0f, 0.0f, 0.0f));
			mCir1.refX += 5.0f;
			mCir2.refX += 5.0f;
			renderer.draw(va3, mainShader, miniCirculos.size(), 0);
		}

		renderer.draw(va3, mainShader, miniCirculos.size(), 0);

		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	glfwDestroyWindow( window );

	glfwTerminate();
	exit( EXIT_SUCCESS );
}
