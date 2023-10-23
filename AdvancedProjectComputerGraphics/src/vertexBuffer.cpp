#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer( const void* data, unsigned int size ) {
	GLCall( glGenBuffers( 1, &rendererId ) );
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, rendererId ) );
	GLCall( glBufferData( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW ) );
}

VertexBuffer::~VertexBuffer() {
	GLCall( glDeleteBuffers( 1, &rendererId ) );
}

void VertexBuffer::bind() {
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, rendererId ) );
}

void VertexBuffer::unBind() {
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
}
