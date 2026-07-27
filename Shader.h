#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <iostream>
#include <map>
#include <tuple>
#include <functional>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Shader
{

public:

	Shader(const char* vertex_file_path, const char* fragment_file_path);

	~Shader();

	unsigned int getShaderProgram();

	template<typename T>
    int addUniform(const T* uniform_variable, const char* name);

    void updateAllUniform();

	void init(const char* vert_prog_path, const char* frag_prog_path);

	void use();

	void bind_bool(const char* name, const bool b);

	void bind_int(const char* name, const int i);

	void bind_float(const char* name, const float f);

	//void bind_texture(const char* name, uint texture, uint id, enum texture_type = GL_TEXTURE_2D);


protected:

	void createShaderProgram(const char* vertex_file_path, const char* fragment_file_path);

	char* readShaderFile(const char* filepath);

private:

	unsigned int shaderProgram; // ID of the shader program

	map<string, int > uVarMap; // name, location ID pairs for every Uniform variable

	map<string, function<void()>> uUpdateMap; // name, update function associated with its type

};

template<typename T>
int Shader::addUniform(const T* uniform_variable,const char* name)
{
    int location = glGetUniformLocation(shaderProgram, name);
    uVarMap[name] = location;

    // chooses which function will be used for update based on type
    uUpdateMap[name] = [location, uniform_variable]()
    {
        if constexpr (is_same_v<T, float>) // constexpr evaluates during compilation so the function isn't overloaded
            glUniform1f(location, *uniform_variable);
        else if constexpr (is_same_v<T, int>)
            glUniform1i(location, *uniform_variable);
        else if constexpr (is_same_v<T, unsigned int>)
            glUniform1ui(location, *uniform_variable);
        else if constexpr (is_same_v<T, bool>)
            glUniform1i(location, *uniform_variable ? 1 : 0);
        else if constexpr (is_same_v<T, vec2>)
            glUniform2fv(location, 1, value_ptr(*uniform_variable));
        else if constexpr (is_same_v<T, vec3>)
            glUniform3fv(location, 1, value_ptr(*uniform_variable));
        else if constexpr (is_same_v<T, vec4>)
            glUniform4fv(location, 1, value_ptr(*uniform_variable));
        else if constexpr (is_same_v<T, mat3>)
            glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(*uniform_variable));
        else if constexpr (is_same_v<T, mat4>)
            glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(*uniform_variable));
        else
            static_assert(!sizeof(T), "addUniform: unsupported uniform type");
    };

    #ifdef MAP
        cout << "Added uVarMap[" << name << "] = " << location << endl;
    #endif

    return location;
}


#endif