#pragma once

#include "Renderer.h"

class Texture {
private:
	GLuint rendererId;
	std::string filePath;
	unsigned char* localBuffer;
	int width, height, BPP;

public:
	Texture( const std::string& path );
	~Texture();

	GLuint getRendererId();

	void bind( unsigned int slot = 0 ) const;
	void unBind() const;

	inline int getWidth() const { return width; }
	inline int getHeight() const { return height; }
};