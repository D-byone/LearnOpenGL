



// VAO 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



#if 1


typedef struct
{
   // Handle to a program object
   GLuint programObject;
   GLuint VBO;
   GLuint vboColor;
   GLuint VAO;
   GLuint EBO;

} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
    GLuint shader;
    GLuint compiled;

    shader = glCreateShader(type);

    if (shader == 0)
    {
        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, NULL);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if ( !compiled )
    {
        GLuint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if ( infoLen > 1)
        {
            char *infoLog = malloc( sizeof(char) * infoLen );

            glGetShaderInfoLog( shader, infoLen, NULL, infoLog);
            esLogMessage("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);

        return 0;
    }

    return shader;

}




///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
    UserData* userData = esContext->userData;
    char vShaderStr[] =
    "#version 300 es                            \n"
    "layout(location = 0) in vec3 vPosition;    \n"
    "layout(location = 1) in vec4 a_color;      \n"
    "out vec4 v_color;                          \n"
    "void main()                                \n"
    "{                                          \n"
    // "    v_color = a_color;                     \n"
    "    v_color = vec4(clamp(vPosition, 0.0, 1.0), 1.0);   \n"
    "     gl_Position = vec4(vPosition, 1.0);              \n"
    "}                                          \n";

    char fShaderStr[] = 
    "#version 300 es                            \n"
    "precision mediump float;                   \n"
    "out vec4 fragColor;                        \n"
    "in vec4 v_color;                           \n"
    "void main()                                \n"
    "{                                          \n"
    "    fragColor = v_color;                   \n"
    "}                                          \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLuint linked;

    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    programObject = glCreateProgram();

    if (programObject == 0)
    {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLuint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char * infoLog = malloc( sizeof(char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            esLogMessage("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }
        glDeleteProgram(programObject);
        return FALSE;
    }

    userData->programObject = programObject;


    //moved from draw start
    GLfloat vVertices[] = {  -1.0,  -1.0f,  0.0f,   1.0,  0.0,  0.0,  1.0,
                             0.5f,  -0.5f,  0.0f,   0.0,  1.0,  0.0,  1.0,
                            -0.5f,   0.5f,  0.0f,   0.0,  0.0,  1.0,  1.0,
                             1.0f,   1.0f,  0.0f,   0.0,  0.0,  0.0,  1.0
                    
                            };

    unsigned int indices[] = {
      0, 1, 2,
      1, 2, 3
    };


    glGenBuffers(1, &userData->VBO);
    glGenBuffers(1, &userData->vboColor);
    glGenBuffers(1, &userData->EBO);

    glGenVertexArrays(1, &userData->VAO);
    glBindVertexArray ( userData->VAO );

    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 28, vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboColor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 28, vVertices + sizeof(GLfloat) * 3, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboColor);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (GLvoid*)(sizeof(GLfloat) * 3));

    glBindVertexArray ( 0 );

     
    return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
float color = 0.0f;

void Draw ( ESContext *esContext )
{
    UserData *userData = esContext->userData;



    glViewport(0, 0, esContext->width, esContext->height);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   
    glClear(GL_COLOR_BUFFER_BIT);


    glUseProgram(userData->programObject);

    glBindVertexArray ( userData->VAO );



    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray ( 0 );


}

void Shutdown ( ESContext *esContext )
{
    UserData *userData = esContext->userData;

    glDeleteProgram(userData->programObject);
    glDeleteBuffers ( 1, &userData->VBO );
    glDeleteBuffers ( 1, &userData->vboColor );
    glDeleteBuffers ( 1, &userData->EBO );
    glDeleteVertexArrays ( 1, &userData->VAO );
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "Hello Triangle", 640, 480, ES_WINDOW_RGB );

    if ( !Init ( esContext ) )
    {
        return GL_FALSE;
    }

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterDrawFunc ( esContext, Draw );

    return GL_TRUE;
}


#endif




