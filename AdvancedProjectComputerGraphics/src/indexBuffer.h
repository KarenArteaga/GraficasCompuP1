#pragma once

class IndexBuffer {
private:
	unsigned int rendererId;
	unsigned int count;
public:
	IndexBuffer( const unsigned int* data, unsigned int _count );
	~IndexBuffer();

	void bind() const;
	void unBind() const;

	inline unsigned int  getCount() const { return count; }
};