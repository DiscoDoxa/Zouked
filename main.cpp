#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// shaders (code in GLSL)
const char *vertexShaderSource = "#version 330 core\n" 
                                "layout (location = 0) in vec3 aPos;\n" // Input (vec3)
                                "void main()\n"
                                "{\n"
                                "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // Output (vec4)
                                "}\0"; // Most simple vertex shader that does nothing

const char *fragmentShaderSource ="#version 330 core\n"
                                "out vec4 FragColor;\n"
                                "void main()\n"
                                "{\n"
                                "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                "}\0"; //always outputs an orange-ish color

int main()
{   
    // ------------------------------
    // glfw: initialize and configure
    // ------------------------------

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGl Version 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Profil Core

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // --------------------
    // glfw window creation
    // --------------------

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Totalement Zouked", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Définit cette fenetre comme contexte du thread actuel
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Met en place le callback
    // > launched by PollEvents when triggered

    // ---------------------------------------
    // glad: load all OpenGL function pointers
    // ---------------------------------------

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    // ------------------------------------------------------------------------
    // gl : initialize objects (buffers, shaders, shader programs, vertices...)
    // ------------------------------------------------------------------------

    // 1. vertices (= sommets)
    // _______________________

    float vertices[] = { // Normalized Device Coordinates (NDC) of a 2D triangle
        -0.5f, -0.5f, 1.0f, // bottom left
         0.5f, -0.5f, 0.0f, // bottom right
        -0.5f,  0.5f, 0.0f, // top left
         0.5f,  0.5f, 0.0f  // top right 
    }; // Will be transformed to Screen Space Cordinates (SSC) via the ViewPort transform

    unsigned int indices[] = {  // note that we start from 0!
    0, 1, 2,   // first triangle
    1, 2, 3    // second triangle
    }; 

    // 2. buffers (in the GPU for fast access)
    // _________________________________________

    unsigned int VBO;  // Vertex Buffer Object - for uploading vertex data (position, normal vector, color, etc.) 
    glGenBuffers(1, &VBO); // Creation (ID, storage variable of the ID)
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind it as an array buffer

    unsigned int VAO; // Vertex Array Object - to store vertex configuration (enabled, location, size, format, binding calls, etc...)
    glGenVertexArrays(1, &VAO); // It is bound to the VBO (adds a layer to it)
    glBindVertexArray(VAO);

    unsigned int EBO; // Elements Buffer Object - to workaround vertices redundance and instead specify orders
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind it as an element array buffer

    // copy our vertices array in a buffer for OpenGL to use

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Type, size, data, and:
    // GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    // GL_STATIC_DRAW: the data is set only once and used many times.
    // GL_DYNAMIC_DRAW: the data is changed a lot and used many times.

    // then set the vertex attributes pointers (calls stored in the VAO)

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Tells the vertex shader how to read the VBO
    glEnableVertexAttribArray(0);  // We want to pass data to (vertex attribute) location of 0 (and enable it)

    // 3. shaders and shader program
    // _____________________________

    unsigned int vertexShader; // Shader #1 : Applies transformation to the vertices
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Apply source (shader, number of strings, strings, NULL)
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

    unsigned int shaderProgram; // Shader program - final linked version of multiple shaders combined
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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader); // no longer nedded


    // -----------
    // render loop
    // -----------

    while (!glfwWindowShouldClose(window))
    {
        // -----
        // input
        // -----

        processInput(window); // Process inputs (crazy right)

        // ----------------------------------
        // gl: rendering (write into buffers)
        // ----------------------------------

        //glClearColor(0.5f, 0.3f, 0.3f, 1.0f); // state-setting
        //glClear(GL_COLOR_BUFFER_BIT); // state-using
        
        // use our shader program when we want to render an object

        glUseProgram(shaderProgram); // Activates shader program (from then and on)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // or GL_FILL for full triangles

        // now draw the object 

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3); // Type, starting index, number of vertices to draw.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Type, number of elements, indices types, offset

        // -------------------------------------------------------------------------------
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ------------------------------------------------------------------
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

    glfwTerminate();
    return 0;
}

// ---------------------------------------------------------------------------------------------------------
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// ---------------------------------------------------------------------------------------------
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}