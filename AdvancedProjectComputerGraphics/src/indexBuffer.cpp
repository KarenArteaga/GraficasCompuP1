#include "IndexBuffer.h"

#include "Renderer.h"

IndexBuffer::IndexBuffer( const unsigned int* data, unsigned int _count ) : count( _count ) {
	GLCall( glGenBuffers( 1, &rendererId ) );
	GLCall( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, rendererId ) );
	GLCall( glBufferData( GL_ELEMENT_ARRAY_BUFFER, count * sizeof( unsigned int ), data, GL_STATIC_DRAW ) );
}

IndexBuffer::~IndexBuffer() {
	GLCall( glDeleteBuffers( 1, &rendererId ) );
}

void IndexBuffer::bind() const {
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, rendererId ) );
}

void IndexBuffer::unBind() const {
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
}
