#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class Shader
{

public:

	Shader(const char * vertex_file_path, const char * fragment_file_path);

	~Shader();

	void init(const char* vert_prog_path, const char* frag_prog_path);

	void use();

	void bind_bool(const char* name, const bool b);

	void bind_int(const char* name, const int i);

	void bind_float(const char* name, const float f);

	//void bind_texture(const char* name, uint texture, uint id, enum texture_type = GL_TEXTURE_2D);

	unsigned int shaderProgram; // ID of the shader program

protected:

	void createShaderProgram(const char * vertex_file_path, const char * fragment_file_path);

	char *readShaderFile(const char *filepath);
};

#endif