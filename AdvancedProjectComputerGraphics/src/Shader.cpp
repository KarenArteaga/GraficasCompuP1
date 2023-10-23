#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Renderer.h"

Shader::Shader( const std::string& vertexFilePath, const std::string& fragmentFilePath ) : rendererId( 0 ) {
	ShaderProgramSource source = { parseShader( vertexFilePath ), parseShader( fragmentFilePath ), "" };
	rendererId = createShader( source.vertexSource, source.fragmentSource );
}

Shader::Shader( const std::string& vertexFilePath, const std::string& fragmentFilePath, const std::string& geometryFilePath ) : rendererId( 0 ) {
	ShaderProgramSource source = { parseShader( vertexFilePath ), parseShader( fragmentFilePath ), parseShader( geometryFilePath ) };
	rendererId = createShader( source.vertexSource, source.fragmentSource, source.geometrySource );
}

Shader::~Shader() {
	GLCall( glDeleteProgram( rendererId ) );
}

void Shader::bind() const {
	GLCall( glUseProgram( rendererId ) );
}

void Shader::unBind() const {
	GLCall( glUseProgram( 0 ) );
}

/////// Uniforms

void Shader::setUniform1i( const std::string& name, int value ) {
	GLCall( glUniform1i( getUniformLocation( name ), value ) );
}

void Shader::setUniform4f( const std::string& name, glm::vec4  vector ) {
	GLCall( glUniform4fv( getUniformLocation( name ), 1, &vector[ 0 ] ) );
}

void Shader::SetuniformsMat4f( const std::string& name, const glm::mat4& mat4 ) {
	GLCall( glUniformMatrix4fv( getUniformLocation( name ), 1, GL_FALSE, &mat4[ 0 ][ 0 ] ) );
}

/////// Uniforms

std::string Shader::parseShader( const std::string& filepath ) {
	std::fstream stream( filepath );
	std::string line, ss;

	while( getline( stream, line ) ) {
		ss += line += '\n';
	}

	return ss;
}

unsigned int Shader::compileShader( unsigned int type, const std::string& source ) {
	GLCall( unsigned int id = glCreateShader( type ) );
	const char* src = source.c_str();
	GLCall( glShaderSource( id, 1, &src, nullptr ) );
	GLCall( glCompileShader( id ) );

	int result;
	GLCall( glGetShaderiv( id, GL_COMPILE_STATUS, &result ) );
	if( result == GL_FALSE ) {

		int length;
		GLCall( glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length ) );
		char* message = ( char* ) malloc( length * sizeof( char ) );
		GLCall( glGetShaderInfoLog( id, length, &length, message ) );
		std::cout << "Failed to compile Shader" << ( type == GL_VERTEX_SHADER ? "vertex" : type == GL_FRAGMENT_SHADER ? "fragment" : "geometry" ) << std::endl;
		std::cout << message << std::endl;
		GLCall( glDeleteShader( id ) );
		return 0;

	}

	return id;
}

unsigned int Shader::createShader( const std::string& vertexShader, const std::string& fragmentShader ) {
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader( GL_VERTEX_SHADER, vertexShader );
	unsigned int fs = compileShader( GL_FRAGMENT_SHADER, fragmentShader );

	GLCall( glAttachShader( program, vs ) );
	GLCall( glAttachShader( program, fs ) );

	GLCall( glLinkProgram( program ) );
	GLCall( glValidateProgram( program ) );

	GLCall( glDeleteShader( vs ) );
	GLCall( glDeleteShader( fs ) );

	return program;
}

unsigned int Shader::createShader( const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader ) {
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader( GL_VERTEX_SHADER, vertexShader );
	unsigned int fs = compileShader( GL_FRAGMENT_SHADER, fragmentShader );
	unsigned int gs = compileShader( GL_GEOMETRY_SHADER, geometryShader );

	GLCall( glAttachShader( program, vs ) );
	GLCall( glAttachShader( program, fs ) );
	GLCall( glAttachShader( program, gs ) );

	GLCall( glLinkProgram( program ) );
	GLCall( glValidateProgram( program ) );

	GLCall( glDeleteShader( vs ) );
	GLCall( glDeleteShader( fs ) );
	GLCall( glDeleteShader( gs ) );

	return program;
}

int Shader::getUniformLocation( const std::string& name ) {
	if( uniformLocationCache.find( name ) != uniformLocationCache.end() )
		return uniformLocationCache[ name ];

	GLCall( int location = glGetUniformLocation( rendererId, name.c_str() ) );
	if( location == -1 )
		std::cout << "Warning: uniform '" << name << "' doesn't exists!" << std::endl;

	uniformLocationCache[ name ] = location;
	return location;
}
