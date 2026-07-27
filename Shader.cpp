#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <tuple>
#include "Shader.h"

using namespace std;
using namespace glm;

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
{
	createShaderProgram(vertex_file_path, fragment_file_path);
}

Shader::~Shader()
{
	// Suppression du Shader Program
	#ifdef MAP
		cout << "Suppression du Shader Program" << endl;
	#endif

	glDeleteProgram(shaderProgram);
}

unsigned int Shader::getShaderProgram()
{
    return shaderProgram;
}

void Shader::updateAllUniform()
{
    for (const auto& [name, updateFn] : uUpdateMap)
    {
        updateFn();
    }
}

void Shader::createShaderProgram(const char* vertex_file_path, const char* fragment_file_path)
{
    #ifdef MAP
        int nrAttributes;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
        cout << "Création du shader program: " << endl; 
        cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;
    #endif

	// shaders (code in GLSL)
	const char* vertexShaderSource = readShaderFile(vertex_file_path);
	const char* fragmentShaderSource = readShaderFile(fragment_file_path);

	unsigned int vertexShader; // Shader #1 : Applies transformation to the vertices
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Apply source (shader, number of char*s, char*s, NULL)
    glCompileShader(vertexShader); // Compiles it

    unsigned int fragmentShader; // Shader #2 : Aplies colors to pixels
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader); // Compiles it

    // Check shader compiling success  [[[

    char infoLog[512];
    int  v_success, f_success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &v_success);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &f_success);
    if(!v_success)
    {   glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;} 
    if(!f_success)
    {   glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;} 

    //          ]]]

    #ifdef MAP
        cout << "Shaders compilés" << endl;
    #endif

    // Shader program - final linked version of multiple shaders combined
    shaderProgram = glCreateProgram(); // Returns the ID of the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check shader program linking success    [[[

    int p_success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &p_success);
    if(!p_success) 
    {   glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER_PROGRAMM::LINKING_FAILED\n" << infoLog << endl;} 

    ///         ]]]

    #ifdef MAP
        cout << "Shaders linkés" << endl;
    #endif

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader); // no longer nedded
}

char* Shader::readShaderFile(const char* filepath)
{
	#ifdef MAP
		cout << "Lecture du fichier: " << filepath << "..." << endl;
	#endif

    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open shader file: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer =  (char*)malloc(length + 1);
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);

    #ifdef MAP
		cout << "- - - CONTENU de " << filepath << " - - -" << endl;
		for (int i = 0;i < length ; i++)
		{
			cout << buffer[i];
		}
		cout << endl << "- - - - - - - - - - - - -" << endl;
	#endif

    return buffer;
}
