#include "Renderer.h"
#include <iostream>

void GLClearError() {
	while( glGetError() != GL_NO_ERROR );
}

bool GLLogCall( const char* function, const char* file, int line ) {
	while( GLenum error = glGetError() ) {
		std::cout << "[OpenGl Error] ( " << error << " )" << function << " " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::clear() const {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const {
	shader.bind();
	va.bind();
	ib.bind();
	GLCall( glDrawElements( GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr ) );
}

void Renderer::draw(const VertexArray& va, const Shader& shader, int size, int offset) const {
    shader.bind();
    va.bind();
    GLCall(glDrawArrays(GL_TRIANGLES, offset, size););
}

void Renderer::drawEllipse(float x, float y, float a, float b, float depth, std::vector<float>& vertices, float R, float G, float B, float min, float max) {
    std::vector<glm::vec3> temp;

    for (int i = 0; i < 2001; i++) {
        float angle1 = min + (max - min) * i / 2000;
        float angle2 = min + (max - min) * (i + 1) / 2000;

        float x1 = x + a * cos(angle1);
        float y1 = y + b * sin(angle1);
        float x2 = x + a * cos(angle2);
        float y2 = y + b * sin(angle2);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(depth);
        vertices.push_back(1.0f);
        vertices.push_back(R);
        vertices.push_back(G);
        vertices.push_back(B);
        vertices.push_back(1.0f);

        vertices.push_back(x1);
        vertices.push_back(y1);
        vertices.push_back(depth);
        vertices.push_back(1.0f);
        vertices.push_back(R);
        vertices.push_back(G);
        vertices.push_back(B);
        vertices.push_back(1.0f);

        vertices.push_back(x2);
        vertices.push_back(y2);
        vertices.push_back(depth);
        vertices.push_back(1.0f);
        vertices.push_back(R);
        vertices.push_back(G);
        vertices.push_back(B);
        vertices.push_back(1.0f);
    }
}
