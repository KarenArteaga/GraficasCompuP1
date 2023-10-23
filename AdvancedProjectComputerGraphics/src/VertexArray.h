#pragma once

#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray {
private:
	unsigned int rendererId;
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unBind() const;

	void addBuffer( VertexBuffer& vb, const VertexBufferLayout& layout );
};