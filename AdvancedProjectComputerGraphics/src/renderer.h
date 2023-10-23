#pragma once

#include <Glad/glad.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if((!(x))) __debugbreak();
#define GLCall(x) GLClearError(); \
		x; \
		ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall( const char* fucntion, const char* file, int line );

class Renderer {

public:
	void clear()const;
	void draw( const VertexArray& va, const IndexBuffer& ib, const Shader& shader )const;
	void draw(const VertexArray& va, const Shader& shader, int size, int offset) const;
	void drawEllipse(float x, float y, float a, float b, float depth, std::vector<float>& vertices, float R, float G, float B, float min, float max);
};