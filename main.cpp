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

static void error_callback( int error, const char* description ) 
{
	fprintf( stderr, "Error: %s\n", description );
}

static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ) 
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );
}

std::vector<float> getVelComponentsFreeFall(float posX, float posY, float vx0, float vy0, float time, bool &signChange, float depthLimit)
{
	std::vector<float> velocity;
	float gravity = 0.81f; 
	float damping = 0.35f; 

	velocity.push_back(vx0);

	float vY = vy0 - (gravity * time);

	// Check for velocity change
	if ((vy0 < 0.0f && vY >= 0.0f)) {
		signChange = true;
	}
	else {
		signChange = false;
	}

	// Apply bounce (with damping) and threshold
	if (posY <= depthLimit && vY < 0.0f) 
	{
		vY = -vY * damping;
		signChange = true;

		// Apply a threshold to stop the bounce when it becomes too small
		if (std::abs(vY) < 1.0f) {
			vY = 0.0f;
		}
	}
	velocity.push_back(vY);

	return velocity;
}

struct figure 
{ 
	float refX = 0.0f; 
	float refY = 0.0f; 
	float velX = 0.0f;
	float velY = 0.0f;

	figure() {}
	figure(float X, float Y): refX(X), refY(Y) {}

	void updateValues(float X, float Y)
	{
		refX += X; refY += Y; velX = X; velY = Y;
	}

	void updateVels(float X, float Y)
	{
		velX = X; velY = Y;
	}

	void updatePos(float X, float Y)
	{
		refX += X; refY += Y;
	}
};

int SCR_WIDTH = 700;
int SCR_HEIGHT = 975;

int main(void) {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project 1: KA & MH", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSwapInterval(1);

	// How OpenGL will work with the format of textures
	GLCall(glEnable(GL_BLEND));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	Shader mainShader("res/shaders/template.vs", "res/shaders/template.fs");

	Renderer renderer;

	std::vector<float> fondo = {
		// position                        // color
		25.0f, 950.0f, 0.09f, 1.0f,        0.9569f, 0.9412f, 0.8980f, 1.0f,   // top left
		25.0f, 25.0f, 0.09f, 1.0f,         0.9569f, 0.9412f, 0.8980f, 1.0f,   // bottom left
		675.0f, 950.0f, 0.09f, 1.0f,       0.9569f, 0.9412f, 0.8980f, 1.0f,   // top right

		675.0f, 950.0f, 0.09f, 1.0f,       0.9569f, 0.9412f, 0.8980f, 1.0f,   // top right
		675.0f, 25.0f, 0.09f, 1.0f,        0.9569f, 0.9412f, 0.8980f, 1.0f,   // bottom right
		25.0f, 25.0f, 0.09f, 1.0f,         0.9569f, 0.9412f, 0.8980f, 1.0f,   // bottom left
	};

	std::vector<float> techo = {
		// position                       // color
		0.0f, 975.0f, 0.14f, 1.0f,        0.1f, 0.1f, 0.1f, 1.0f,   // top left
		0.0f, 950.0f, 0.14f, 1.0f,        0.1f, 0.1f, 0.1f, 1.0f,   // bottom left
		700.0f, 975.0f, 0.14f, 1.0f,      0.1f, 0.1f, 0.1f, 1.0f,   // top right

		700.0f, 975.0f, 0.14f, 1.0f,      0.1f, 0.1f, 0.1f, 1.0f,   // top right
		700.0f, 950.0f, 0.14f, 1.0f,      0.1f, 0.1f, 0.1f, 1.0f,   // bottom right
		0.0f, 950.0f, 0.14f, 1.0f,        0.1f, 0.1f, 0.1f, 1.0f,   // bottom left
	};

	unsigned int indices[] = {
		0, 2, 1,
		1, 2, 3
	};
	unsigned int indices2[] = {
		0, 2, 1,
		1, 2, 3,
	};

	std::vector<float> fondo2;
	figure back1(-500.f, 70.0f), back2(-50.0f, 195.0f);

	renderer.drawEllipse(back1.refX, back1.refY, 425.0f, 825.0f, 0.11f, fondo2, 0.9725f, 0.8509f, 0.7255f, 0.0f, 3.14159 * 0.5);
	renderer.drawEllipse(back2.refX, back2.refY, 200.0f, 605.0f, 0.11f, fondo2, 0.8941f, 0.5804f, 0.3137f, 0.0f, 3.14159 * 0.5);

	std::vector<float> bases;
	figure base1(487.5f, 1200.0f), base2(162.5f, 1246.3f), base2Circle(162.5f, 1150.0f);

	renderer.drawEllipse(base1.refX, base1.refY, 186.5f, 173.3f, 0.12f, bases, 0.718f, 0.235f, 0.216f, 3.14159, 2*3.14159);
	renderer.drawEllipse(base2.refX, base2.refY, 120.0f, 212.74f, 0.12f, bases, 0.867f, 0.247f, 0.275f, 3.14159, 2*3.14159);
	renderer.drawEllipse(base2Circle.refX, base2Circle.refY, 75.0f, 75.0f, 0.13f, bases, 0.8627f, 0.6196f, 0.4196f, 0.0f, 2 * 3.14159);

	std::vector<float> miniCirculos;
	figure mCir1(100.0f, -20.0f), mCir2(180.0f, -20.0f);
	mCir1.velX = 3.0f;
	mCir1.velY = 30.0f;
	mCir2.velX = -3.0f;
	mCir2.velY = 25.0f;

	renderer.drawEllipse(mCir1.refX, mCir1.refY, 18.75f, 18.75f, 0.14f, miniCirculos, 0.9098f, 0.5490f, 0.2706f, 0.0f, 2 * 3.14159);
	renderer.drawEllipse(mCir2.refX, mCir2.refY, 18.75f, 18.75f, 0.14f, miniCirculos, 0.9098f, 0.5490f, 0.2706f, 0.0f, 2 * 3.14159);

	std::vector<float> leftTower;
	// blue supports
	figure rect1(75.0f, 1100.0f);
	renderer.drawEllipse(100.0f, 1100.0f, 25.0, 25.0, 0.15f, leftTower, 0.549, 0.620, 0.604, 0.0f, 3.14159);
	renderer.insertRectangle(rect1.refX, rect1.refY, 97.0f, 50.0, leftTower, 0.549, 0.620, 0.604, 0.15f);
	renderer.drawEllipse(180.0f, 1100.0f, 25.0, 25.0, 0.15f, leftTower, 0.549, 0.620, 0.604, 0.0f, 3.14159);
	renderer.insertRectangle(155.0f, 1100.0f, 97.0f, 50.0, leftTower, 0.549, 0.620, 0.604, 0.15f);
	// orange supports
	renderer.drawEllipse(100.0, 1145.0, 20.0, 20.0, 0.15f, leftTower, 0.859, 0.455, 0.298, 3.14159, 2 * 3.14159);
	renderer.insertRectangle(80.0f, 1220.0, 75.0f, 40.0f, leftTower, 0.859, 0.455, 0.298, 0.15f);
	renderer.drawEllipse(180.0, 1145.0, 20.0, 20.0, 0.15f, leftTower, 0.859, 0.455, 0.298, 3.14159, 2 * 3.14159);
	renderer.insertRectangle(160.0f, 1220.0, 75.0f, 40.0f, leftTower, 0.859, 0.455, 0.298, 0.15f);
	// pot rectangle
	renderer.insertRectangle(50.0, 1240.0, 20.0, 210.0, leftTower, 0.718, 0.235, 0.216, 0.15f);
	// pot
	renderer.insertRectangle(125.0, 1315.0, 75.0, 50, leftTower, 0.608, 0.388, 0.208, 0.15);

	std::vector<float> middleTower;
	figure middleBase(252.0, 1200.0); // 385.0  1200
	renderer.drawEllipse(middleBase.refX, middleBase.refY, 232.5, 140.5, 0.16, middleTower, 0.867, 0.247, 0.275, 1.55 * 3.14159, 2 * 3.14159);
	renderer.drawEllipse(385.0, 1305.0, 75.0, 105.0, 0.16, middleTower, 0.859, 0.455, 0.298, 3.09, 1.98 * 3.14159);
	renderer.drawEllipse(385.0, 1325.0, 72.0, 90.0, 0.15, middleTower, 0.4, 0.533, 0.549, 3.0, 1.95 * 3.14159);
	renderer.drawEllipse(385.0, 1355.0, 60.0, 70.0, 0.14, middleTower, 0.608, 0.388, 0.208, 3.14159, 2 * 3.14159);
	renderer.drawEllipse(385.0, 1392.5, 37.5, 37.5, 0.16, middleTower, 0.867, 0.247, 0.275, 0.0, 2 * 3.14159);
	renderer.drawEllipse(415.0, 1474.0, 55.0, 50.0, 0.16, middleTower, 0.914, 0.486, 0.176, 0.5 * 3.14159, 1.5 * 3.14159);
	renderer.drawEllipse(410.0, 1573.0, 55.0, 50.0, 0.16, middleTower, 0.9725f, 0.8509f, 0.7255f, 0.0, 0.5 * 3.14159);
	renderer.drawEllipse(410.0, 1573.0, 55.0, 50.0, 0.16, middleTower, 0.9725f, 0.8509f, 0.7255f, 1.5 * 3.14159, 2 * 3.14159);

	std::vector<float> leftBase2;
	figure leftEllipse(850.0, 210.0);
	renderer.drawEllipse(leftEllipse.refX, leftEllipse.refY, 75.0, 150.0, 0.16, leftBase2, 0.0, 0.0, 0.0, 0.5 * 3.14159, 3.14159);
	renderer.insertRectangle(850.0, 360.0, 150.0, 125.0, leftBase2, 0.0, 0.0, 0.0, 0.16);
	renderer.drawEllipse(975.0, 210.0, 60.0, 150.0, 0.16, leftBase2, 0.0, 0.0, 0.0, 0, 0.5 * 3.14159);
	renderer.drawEllipse(905.5, 210.0, 60.0, 60.0, 0.17, leftBase2, 0.9098f, 0.5490f, 0.2706f, 0, 2 * 3.14159);

	std::vector<float> rightTower;
	figure rightBase(525.0, 1200.0);
	renderer.drawEllipse(rightBase.refX, rightBase.refY, 75.0, 75.0, 0.18, rightTower, 0.718, 0.235, 0.216, 0, 2 * 3.14159);
	renderer.insertRectangle(425.0, 1307.0, 32.0, 200.0, rightTower, 0.227, 0.482, 0.439, 0.18);
	renderer.drawEllipse(525.0, 1357.0, 100.0, 50.0, 0.18, rightTower, 0.898, 0.749, 0.196, 3.14159, 2 * 3.14159);
	renderer.drawEllipse(525.0, 1377.0, 100.0, 50.0, 0.19, rightTower, 0.780, 0.560, 0.253, 3.14159, 2 * 3.14159);
	renderer.drawEllipse(525.0, 1397.0, 100.0, 50.0, 0.20, rightTower, 0.227, 0.482, 0.439, 3.14159, 2 * 3.14159);
	// block last ellipse // 642
	renderer.drawEllipse(475.0, 1407.0, 40.0f, 35.0f, 0.21f, rightTower, 0.9569f, 0.9412f, 0.8980f, 3.14159f, 3.14159 * 1.5);
	renderer.insertRectangle(475.0, 1412.0, 40.0, 40.0, rightTower, 0.8941f, 0.5804f, 0.3137f, 0.21);
	renderer.drawEllipse(475.0, 1400.0, 135.0f, 30.0f, 0.21f, rightTower, 0.8941f, 0.5804f, 0.3137f, 1.5 * 3.14159f, 3.14159 * 2);
	renderer.drawEllipse(525.0, 1400.0, 78.0f, 30.0f, 0.21f, rightTower, 0.8941f, 0.5804f, 0.3137f, 1.5 * 3.14159f, 3.14159 * 2);

	renderer.drawEllipse(525.0, 1412.0, 45.0, 45.0, 0.22, rightTower, 0.867, 0.247, 0.275, 0.0, 2 * 3.14159);
	renderer.drawEllipse(525.0, 1432.0, 130.0, 130.0, 0.22, rightTower, 0.4, 0.533, 0.549, 0.0, 3.14159);
	renderer.drawEllipse(525.0, 1612.0, 50.0, 50.0, 0.22, rightTower, 0.9725f, 0.8509f, 0.7255f, 3.14159, 2 * 3.14159);
	renderer.drawEllipse(525.0, 1637.0, 25.0, 25.0, 0.22, rightTower, 0.835, 0.525, 0.282, 0, 2 * 3.14159);

	std::vector<float> drops;
	int dropCount = 90;
	float height = 3250.0;
	float xpos = 140.0;
	for (int i = 0; i < dropCount; i++)
	{
		renderer.drawEllipse(xpos, height, 10.0, 10.0, 0.23, drops, 0.529, 0.808, 0.922, 0, 2 * 3.14159);
		height -= 25.0;
		if (i % 2 == 0)
		{
			xpos += 25.0;
		}
		else
		{
			xpos -= 25.0;
		}
	}

	std::vector<float> windowBlack = {
		// posicion                               // color
		0.0f, 975.0f, 0.24f, 1.0f,        0.1f, 0.1f, 0.1f, 0.7f,   // top left
		0.0f, 0.0f, 0.24f, 1.0f,         0.1f, 0.1f, 0.1f, 0.7f,   // bottom left
		700.0f, 975.0f, 0.24f, 1.0f,       0.1f, 0.1f, 0.1f, 0.7f,   // top right

		700.0f, 975.0f, 0.24f, 1.0f,       0.1f, 0.1f, 0.1f, 0.7f,   // top right
		700.0f, 0.0f, 0.24f, 1.0f,        0.1f, 0.1f, 0.1f, 0.7f,    // bottom right
		0.0f, 0.0f, 0.24f, 1.0f,         0.1f, 0.1f, 0.1f, 0.7f,   // bottom left
	};

	VertexArray va1, va2, va3, va4, va5, va6, va7, va8, va9, va10, va11;
	VertexBufferLayout layout1, layout2, layout3, layout4, layout5, layout6, layout7, layout8, layout9, layout10, layout11;

	VertexBuffer vb1(fondo.data(), sizeof(float) * fondo.size());
	IndexBuffer ib1(indices, sizeof(indices));

	VertexBuffer vb2(fondo2.data(), sizeof(float) * fondo2.size());
	VertexBuffer vb3(bases.data(), sizeof(float) * bases.size());
	VertexBuffer vb4(miniCirculos.data(), sizeof(float) * miniCirculos.size());

	VertexBuffer vb5(techo.data(), sizeof(float) * techo.size());
	IndexBuffer ib2(indices2, sizeof(indices2));

	VertexBuffer vb6(leftTower.data(), sizeof(float) * leftTower.size());
	VertexBuffer vb7(middleTower.data(), sizeof(float)* middleTower.size());
	VertexBuffer vb8(leftBase2.data(), sizeof(float)* leftBase2.size());
	VertexBuffer vb9(rightTower.data(), sizeof(float)* rightTower.size());
	VertexBuffer vb10(drops.data(), sizeof(float)* drops.size());
	VertexBuffer vb11(windowBlack.data(), sizeof(float) * windowBlack.size());

	// Add a push per every layer that you add to VertexBuffer, the param is de number of
	// positions that use your layer
	// Each layer represents a level, can be a coordinate, color or texture
	layout1.push<float>(4);
	layout1.push<float>(4);
	layout2.push<float>(4);
	layout2.push<float>(4);
	layout3.push<float>(4);
	layout3.push<float>(4);
	layout4.push<float>(4);
	layout4.push<float>(4);
	layout5.push<float>(4);
	layout5.push<float>(4);
	layout6.push<float>(4);
	layout6.push<float>(4);
	layout7.push<float>(4);
	layout7.push<float>(4);
	layout8.push<float>(4);
	layout8.push<float>(4);
	layout9.push<float>(4);
	layout9.push<float>(4);
	layout10.push<float>(4);
	layout10.push<float>(4);
	layout11.push<float>(4);
	layout11.push<float>(4);

	va1.addBuffer(vb1, layout1);
	va2.addBuffer(vb2, layout2);
	va3.addBuffer(vb3, layout3);
	va4.addBuffer(vb4, layout4);
	va5.addBuffer(vb5, layout5);
	va6.addBuffer(vb6, layout6);
	va7.addBuffer(vb7, layout7);
	va8.addBuffer(vb8, layout8);
	va9.addBuffer(vb9, layout9);
	va10.addBuffer(vb10, layout10);
	va11.addBuffer(vb11, layout11);

	Shader shaderText("res/shaders/template_text.vs", "res/shaders/template_text.fs");
	shaderText.bind();

	Texture texture1("res/textures/planta.jpg");
	texture1.bind(0);

	std::vector<float> plant = {
		// pos                      // color          // texture
		-0.7143, 0.3308, -0.14, 1.0,    0.0, 0.0, 0.0,    0.0, 1.0,
		-0.7143, -0.12, -0.14, 1.0,    0.0, 0.0, 0.0,    0.0, 0.0,
		-0.40, 0.3308, -0.14, 1.0,    0.0, 0.0, 0.0,    1.0, 1.0,

		-0.40, 0.3308, -0.14, 1.0,    0.0, 0.0, 0.0,    1.0, 1.0,
		-0.40, -0.12, -0.14, 1.0,    0.0, 0.0, 0.0,    1.0, 0.0,
		-0.7143, -0.12, -0.14, 1.0,    0.0, 0.0, 0.0,    0.0, 0.0
	};
	std::vector<float> centroid = { 0.55715, -0.2108 };

	VertexArray vaPlant;
	VertexBufferLayout layoutPlant;
	VertexBuffer vbPlant(plant.data(), sizeof(float)* plant.size());
	layoutPlant.push<float>(4);
	layoutPlant.push<float>(3);
	layoutPlant.push<float>(2);
	//layoutPlant.push<float>(2);
	vaPlant.addBuffer(vbPlant, layoutPlant);

	glPolygonMode( GL_FRONT, GL_FILL );

	glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, -1.0f, 1.0f);
	glm::mat4 transform1 = glm::mat4(1.0f);
	glm::mat4 transform2 = glm::mat4(1.0f);
	glm::mat4 transform3 = glm::mat4(1.0f);
	glm::mat4 transform4 = glm::mat4(1.0f);
	glm::mat4 transform5 = glm::mat4(1.0f);
	glm::mat4 transform6 = glm::mat4(1.0f);
	glm::mat4 transform7 = glm::mat4(1.0f);
	glm::mat4 transform8 = glm::mat4(1.0f);
	glm::mat4 transform9 = glm::mat4(1.0f);
	glm::mat4 transform10 = glm::mat4(1.0f);
	glm::mat4 transform11 = glm::mat4(1.0f);
	glm::mat4 transformP = glm::mat4(1.0f);
	glm::mat4 toCenter, Scale, toPos;

	float scale = 0.1f;

	time_t seconds;
	seconds = time(NULL);
	int passed_seconds;
	int totalAnimationTime = 80, cut = 1;

	float seconds_call = 0.0f, jumpTime = 0.0f, seconds_call2 = 0.0f, seconds_call3 = 0.0f, seconds_call4 = 0.0f;
	bool signChange = false;
	float rotateDegree = 0.0;
	int offsetDrops = 0.0;

	while( !glfwWindowShouldClose( window ) ) {
		glfwGetFramebufferSize( window, &SCR_WIDTH, &SCR_HEIGHT );
		glViewport( 0, 0, SCR_WIDTH, SCR_HEIGHT);

		renderer.clear();
		shaderText.bind();

		shaderText.SetuniformsMat4f("u_Transformation", transformP);

		mainShader.bind();

		float brightness = glm::sin(glfwGetTime());

		mainShader.SetuniformsMat4f("u_Projection", projection);
		mainShader.SetuniformsMat4f("u_Transformation", transform1);
		mainShader.setUniform1f("brightness", 1.0);

		renderer.draw(va1, mainShader, fondo.size(), 0);

		passed_seconds = time(NULL) - seconds;

		mainShader.SetuniformsMat4f("u_Transformation", transform2);
		if (back1.refX < 25.0)
		{
			transform2 = glm::translate(transform2, glm::vec3(5.0f, 0.0f, 0.0f));
			back1.refX += 5.0f;
			mainShader.SetuniformsMat4f("u_Transformation", transform2);
			renderer.draw(va2, mainShader, fondo2.size(), 0);
		}
		renderer.draw(va2, mainShader, fondo2.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform3);
		if (passed_seconds % totalAnimationTime > 3 && passed_seconds % totalAnimationTime < 9)
		{
			if (signChange)
			{
				seconds_call = 0.0f;
				signChange = false;
			}
			seconds_call += 0.1;
			std::vector vels = getVelComponentsFreeFall(base1.refX, base1.refY, base1.velX, base1.velY, seconds_call, signChange, 200.0f);			
			base1.updateValues(vels[0], vels[1]);

			transform3 = glm::translate(transform3, glm::vec3(vels[0], vels[1], 0.0f));
			renderer.draw(va3, mainShader, bases.size(), 0);
		}
		renderer.draw(va3, mainShader, bases.size(), 0);
		
		mainShader.SetuniformsMat4f("u_Transformation", transform4);
		if (passed_seconds % totalAnimationTime > 9 && passed_seconds % totalAnimationTime < 15)
		{
			if (passed_seconds % totalAnimationTime < 12)
			{
				if (signChange)
				{
					jumpTime = 0.0f;
					signChange = false;
				}
				jumpTime += 0.1;
				if (mCir1.refX >= 435.f)
				{
					mCir1.velX = 0.0f;
				}
				std::vector vels = getVelComponentsFreeFall(mCir1.refX, mCir1.refY, mCir1.velX, mCir1.velY, jumpTime, signChange, 234.7f);
				transform4 = glm::translate(transform4, glm::vec3(vels[0], vels[1], 0.0f));
				mCir1.updateValues(vels[0], vels[1]);
				mCir2.updatePos(vels[0], vels[1]);
				renderer.draw(va4, mainShader, miniCirculos.size(), 0);
			}
			if (passed_seconds % totalAnimationTime >= 12)
			{
				if (signChange)
				{
					jumpTime = 0.0f;
					signChange = false;
				}
				jumpTime += 0.1;
				if (mCir2.refX <= 180.0f)
				{
					mCir2.velX = 0.0f;
				}
				std::vector vels = getVelComponentsFreeFall(mCir2.refX, mCir2.refY, mCir2.velX, mCir2.velY, jumpTime, signChange, 262.0f);
				transform4 = glm::translate(transform4, glm::vec3(vels[0], vels[1], 0.0f));
				mCir2.updateValues(vels[0], vels[1]);
				renderer.draw(va4, mainShader, miniCirculos.size(), 0);
			}
		}

		renderer.draw(va4, mainShader, miniCirculos.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform5);
		if (passed_seconds % totalAnimationTime > 15 && passed_seconds % totalAnimationTime < 17)
		{
			transform5 = glm::translate(transform5, glm::vec3(5.0f, 0.0f, 0.0f));
			renderer.draw(va5, mainShader, techo.size(), 0);
		}
		
		renderer.draw(va5, mainShader, techo.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform6);

		if (passed_seconds % totalAnimationTime >= 17 && passed_seconds % totalAnimationTime < 22)
		{
			if (signChange)
			{
				seconds_call2 = 0.0f;
				signChange = false;
			}
			seconds_call2 += 0.1;
			std::vector vels = getVelComponentsFreeFall(rect1.refX, rect1.refY, rect1.velX, rect1.velY, seconds_call2, signChange, 379.75f);
			rect1.updateValues(vels[0], vels[1]);

			transform6 = glm::translate(transform6, glm::vec3(vels[0], vels[1], 0.0f));
			renderer.draw(va6, mainShader, leftTower.size(), 0);
		}

		renderer.draw(va6, mainShader, leftTower.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform5);
		if (passed_seconds % totalAnimationTime >= 22 && passed_seconds % totalAnimationTime < 23)
		{
			transform5 = glm::translate(transform5, glm::vec3(4.0f, 0.0f, 0.0f));
			renderer.draw(va5, mainShader, techo.size(), 0);
		}

		renderer.draw(va5, mainShader, techo.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform7);
		if (passed_seconds % totalAnimationTime >= 23 && passed_seconds % totalAnimationTime < 28)
		{
			if (signChange)
			{
				seconds_call3 = 0.0f;
				signChange = false;
			}
			seconds_call3 += 0.1;
			std::vector vels = getVelComponentsFreeFall(middleBase.refX, middleBase.refY, middleBase.velX, middleBase.velY, seconds_call3, signChange, 375.0f);
			middleBase.updateValues(vels[0], vels[1]);

			transform7 = glm::translate(transform7, glm::vec3(vels[0], vels[1], 0.0f));
			renderer.draw(va7, mainShader, middleTower.size(), 0);
		}

		renderer.draw(va7, mainShader, middleTower.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform8);
		if (passed_seconds % totalAnimationTime >= 28 && passed_seconds % totalAnimationTime < 33)
		{
			if (leftEllipse.refX >= 517.5f)
			{
				transform8 = glm::translate(transform8, glm::vec3(-5.0f, 0.0f, 0.0f));
				leftEllipse.refX -= 5.0f;
				renderer.draw(va8, mainShader, leftBase2.size(), 0);
			}
			else if (rotateDegree <= 2.26)
			{
				mainShader.SetuniformsMat4f("u_Transformation", transform7);
				transform7 = glm::rotate(transform7, (float)glm::radians(0.01), glm::vec3(0.0f, 0.0f, 1.0f));
				renderer.draw(va7, mainShader, middleTower.size(), 0);
				mainShader.SetuniformsMat4f("u_Transformation", transform8);
				transform8 = glm::translate(transform8, glm::vec3(-0.23f, 0.0f, 0.0f));
				leftEllipse.refX -= 5.0f;
				renderer.draw(va8, mainShader, leftBase2.size(), 0);
				rotateDegree += 0.01;
			}
		}

		mainShader.SetuniformsMat4f("u_Transformation", transform7);
		renderer.draw(va7, mainShader, middleTower.size(), 0);
		mainShader.SetuniformsMat4f("u_Transformation", transform8);
		renderer.draw(va8, mainShader, leftBase2.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform5);
		if (passed_seconds % totalAnimationTime >= 33 && passed_seconds % totalAnimationTime < 34)
		{
			transform5 = glm::translate(transform5, glm::vec3(4.0f, 0.0f, 0.0f));
			renderer.draw(va5, mainShader, techo.size(), 0);
		}

		renderer.draw(va5, mainShader, techo.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform9); 
		if (passed_seconds % totalAnimationTime >= 34 && passed_seconds % totalAnimationTime < 39)
		{
			if (signChange)
			{
				seconds_call4 = 0.0f;
				signChange = false;
			}
			seconds_call4 += 0.1;
			std::vector vels = getVelComponentsFreeFall(rightBase.refX, rightBase.refY, rightBase.velX, rightBase.velY, seconds_call4, signChange, 435.0f);
			rightBase.updateValues(vels[0], vels[1]);

			transform9 = glm::translate(transform9, glm::vec3(vels[0], vels[1], 0.0f));
			renderer.draw(va9, mainShader, rightTower.size(), 0);
		}

		renderer.draw(va9, mainShader, rightTower.size(), 0);

		mainShader.SetuniformsMat4f("u_Transformation", transform10);
		if (passed_seconds % totalAnimationTime >= 39 && passed_seconds % totalAnimationTime < 49)
		{
			offsetDrops += 5;
			transform10 = glm::translate(transform10, glm::vec3(0.0f, -5.0f, 0.0f));

			renderer.draw(va10, mainShader, drops.size(), 0);

			mainShader.SetuniformsMat4f("u_Transformation", transform1);
			mainShader.setUniform1f("brightness", brightness);
			renderer.draw(va11, mainShader, windowBlack.size(), 0);
			mainShader.setUniform1f("brightness", 1.0);
		}

		shaderText.SetuniformsMat4f("u_Transformation", transformP); 
		if (passed_seconds % totalAnimationTime >= 49 && passed_seconds % totalAnimationTime < 54)
		{
			toCenter = glm::translate(glm::mat4(1.0f), glm::vec3(centroid[0], centroid[1], 0.0f));
			Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0));
			toPos = glm::translate(glm::mat4(1.0f), -glm::vec3(centroid[0], centroid[1], 0.0f));
			transformP = toPos * Scale * toCenter;
			scale += 0.0020;
			transformP = glm::translate(transformP, glm::vec3(0.0f, scale * 0.43, 0.0f));
			renderer.draw(vaPlant, shaderText, plant.size(), 0);
		}

		if (passed_seconds % totalAnimationTime >= 44)
		{
			renderer.draw(vaPlant, shaderText, plant.size(), 0);
		}
		

		if (passed_seconds % totalAnimationTime == 0)
		{
			transform1 = glm::mat4(1.0f);
			transform2 = glm::mat4(1.0f);
			transform3 = glm::mat4(1.0f);
			transform4 = glm::mat4(1.0f);
			transform5 = glm::mat4(1.0f);
			transform6 = glm::mat4(1.0f);
			transform7 = glm::mat4(1.0f);
			transform8 = glm::mat4(1.0f);
			transform9 = glm::mat4(1.0f);
			transform10 = glm::mat4(1.0f);
			transform11 = glm::mat4(1.0f);
			transformP = glm::mat4(1.0f);

			seconds_call = 0.0f, jumpTime = 0.0f, seconds_call2 = 0.0f, seconds_call3 = 0.0f, seconds_call4 = 0.0f;
			signChange = false;
			rotateDegree = 0.0;
			scale = 0.1f;
			offsetDrops = 0.0;

			centroid = { 0.55715, -0.2108 };
			toCenter = glm::translate(glm::mat4(1.0f), glm::vec3(centroid[0], centroid[1], 0.0f));
			Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0));
			toPos = glm::translate(glm::mat4(1.0f), -glm::vec3(centroid[0], centroid[1], 0.0f));
			transformP = toPos * Scale * toCenter;

			back1.refX = -500.0; back1.refY = 70.0;
			base1.refX = 487.5; base1.refY = 1200.0;

			mCir1.refX = 100.0; mCir1.refY = -20.0;
			mCir1.updateVels(3.0, 30.0);
			mCir1.refX = 180.0; mCir2.refY = -20.0;
			mCir2.updateVels(-3.0, 25.0);

			rect1.refX = 75.0; rect1.refY = 1100.0;
			rect1.updateVels(0.0, 0.0);

			middleBase.refX = 252.0; middleBase.refY = 1200.0;
			leftEllipse.refX = 850.0; leftEllipse.refY = 210.0;
			rightBase.refX = 525.0; rightBase.refY = 1200.0;

			float height = 1750.0;
			float xpos = 140.0;
		}

		std::cout << passed_seconds << std::endl;

		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	glfwDestroyWindow( window );

	glfwTerminate();
	exit( EXIT_SUCCESS );
}