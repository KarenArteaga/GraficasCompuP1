#include "Texture.h"

#include "stb_image/stb_image.h"
#include <iostream>

Texture::Texture( const std::string& path ) : filePath( path ), localBuffer( nullptr ), width( 0 ), height( 0 ), BPP( 0 ) {
	stbi_set_flip_vertically_on_load( 1 );
	localBuffer = stbi_load( path.c_str(), &width, &height, &BPP, 4 );

	GLCall( glGenTextures( 1, &rendererId ) );
	GLCall( glBindTexture( GL_TEXTURE_2D, rendererId ) );

	GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
	GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
	GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
	GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );

	if( localBuffer ) {
		GLCall( glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer ) );
		GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
		stbi_image_free( localBuffer );
	} else {
		std::cout << "\nError: Failed to load texture" << std::endl;
		std::cout << stbi_failure_reason() << std::endl;
		__debugbreak();
	}
}

Texture::~Texture() {
	GLCall( glDeleteTextures( 1, &rendererId ) );
}

GLuint Texture::getRendererId() {
	return rendererId;
}

void Texture::bind( unsigned int slot ) const {
	glActiveTexture( GL_TEXTURE0 + slot );
	GLCall( glBindTexture( GL_TEXTURE_2D, rendererId ) );
}

void Texture::unBind() const {
	GLCall( glBindTexture( GL_TEXTURE_2D, 0 ) );
}
