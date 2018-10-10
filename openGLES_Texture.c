


//texture
#if 1



#include "esUtil.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;
   GLuint VBO;
   GLuint VAO;
   GLuint EBO;
   GLuint texture01;
   GLuint texture02;
   GLuint texture03;

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

unsigned int texture;
int Init ( ESContext *esContext )
{
    UserData* userData = esContext->userData;
    char vShaderStr[] =
    "#version 300 es                            \n"
    "layout(location = 0) in vec4 vPosition;    \n"
    "layout(location = 1) in vec4 a_color;      \n"
    "layout(location = 2) in vec2 aTexCoord;    \n"
    "out vec4 v_color;                          \n"
    "out vec2 TexCoord;                         \n"
    "void main()                                \n"
    "{                                          \n"
    "     gl_Position = vPosition;   \n"
    "      v_color = a_color;                   \n"
    "     TexCoord = aTexCoord;                 \n"
    "}                                          \n";

    char fShaderStr[] = 
    "#version 300 es                            \n"
    "precision mediump float;                   \n"
    "out vec4 fragColor;                        \n"
    "in vec4 v_color;                           \n"
    "in vec2 TexCoord;                          \n"
    "uniform sampler2D texture1;                \n"
    "uniform sampler2D texture2;                \n"
    "uniform sampler2D texture3;                \n"
    "void main()                                \n"
    "{                                          \n"
    "   vec4 temTexture = mix( texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5 );"
    "  fragColor = mix(temTexture, texture(texture3, TexCoord), 0.3);      \n"
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


    //moved from draw start // position               //color                       //texcoord
    GLfloat vVertices[] = { -0.8f,  -0.8f,  0.0f,     1.0f,  0.0f,  0.0f,  0.0f,    0.0f, 0.0f,   //vertex 01
                             0.8f,  -0.8f,  0.0f,     0.0f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f,   //vertex 02
                            -0.8f,   0.8f,  0.0f,     0.0f,  0.0f,  1.0f,  0.0f,    0.0f, 1.0f,   //vertex 03
                             0.8f,   0.8f,  0.0f,     0.5f,  0.5f,  0.5f,  0.0f,    1.0f, 1.0f    //vertex 04
                    
                            };

    unsigned int indices[] = {
      0, 1, 2,
      1, 2, 3
    };

    glGenVertexArrays(1, &userData->VAO);
    glGenBuffers(1, &userData->VBO);
    glGenBuffers(1, &userData->EBO);


    glBindVertexArray ( userData->VAO );

    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*)(sizeof(GLfloat) * 7));
    glEnableVertexAttribArray(2);
    
      


    glGenTextures(1, &userData->texture01);
    glBindTexture(GL_TEXTURE_2D, userData->texture01);

    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S,  GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T,  GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER,  GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER,  GL_LINEAR);


    int width01, height01, nrChannels01;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data01 = stbi_load("p01.jpg", &width01, &height01, &nrChannels01, 0);
    printf("width01:%d height01:%d nrChannels01:%d \n", width01, height01, nrChannels01);
    if (data01)
    {

      unsigned int i;

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width01, height01, 0, GL_RGB, GL_UNSIGNED_BYTE, data01);
      glGenerateMipmap(GL_TEXTURE_2D); 
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data01);





    glGenTextures(1, &userData->texture02);
    glBindTexture(GL_TEXTURE_2D, userData->texture02);

    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S,  GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T,  GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER,  GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER,  GL_LINEAR);

    int width02, height02, nrChannels02;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data02 = stbi_load("p02.jpg", &width02, &height02, &nrChannels02, 0);
    printf("width02:%d height02:%d nrChannels02:%d \n", width02, height02, nrChannels02);
    if (data02)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width02, height02, 0, GL_RGB, GL_UNSIGNED_BYTE, data02);
      glGenerateMipmap(GL_TEXTURE_2D); 
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data02);




    glGenTextures(1, &userData->texture03);
    glBindTexture(GL_TEXTURE_2D, userData->texture03);

    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S,  GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T,  GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER,  GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER,  GL_LINEAR);

    int width03, height03, nrChannels03;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data03 = stbi_load("p03.jpg", &width03, &height03, &nrChannels03, 0);
    printf("width03:%d height03:%d nrChannels03:%d \n", width03, height03, nrChannels03);
    if (data03)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width03, height03, 0, GL_RGB, GL_UNSIGNED_BYTE, data03);
      glGenerateMipmap(GL_TEXTURE_2D); 
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data03);

    glBindVertexArray ( 0 );


    glUseProgram(userData->programObject);
    glUniform1i(glGetUniformLocation(userData->programObject, "texture1"), 0); 
    glUniform1i(glGetUniformLocation(userData->programObject, "texture2"), 1); 
    glUniform1i(glGetUniformLocation(userData->programObject, "texture3"), 2); 

    return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
float color = 1.0f;

void Draw ( ESContext *esContext )
{
    UserData *userData = esContext->userData;

  



    // glViewport(480 * color, 480 * color, esContext->width, esContext->height);
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   


    glUseProgram(userData->programObject);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->texture01);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, userData->texture02);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, userData->texture03);


    

    glBindVertexArray ( userData->VAO );


    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray ( 0 );


}

void Shutdown ( ESContext *esContext )
{
    UserData *userData = esContext->userData;

    glDeleteProgram(userData->programObject);
    glDeleteBuffers ( 1, &userData->VBO );
    glDeleteBuffers ( 1, &userData->EBO );
    glDeleteBuffers ( 1, &userData->texture01 );
    glDeleteBuffers ( 1, &userData->texture02 );
    glDeleteBuffers ( 1, &userData->texture03 );
    glDeleteVertexArrays ( 1, &userData->VAO );
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "Hello Triangle", 960, 540, ES_WINDOW_RGB );

    if ( !Init ( esContext ) )
    {
        return GL_FALSE;
    }

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterDrawFunc ( esContext, Draw );

    return GL_TRUE;
}


#endif


