


#if 1

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct
{
    GLuint  programObject;
    GLuint  VBO1;
    GLuint  EBO1;
    GLuint  VAO1;
    GLuint  texture01;

    GLuint  VAO2;
    GLuint  texture02;


    // Rotation angle
   GLfloat   angle;
    // Uniform locations
   GLint  mvpLoc;
    // MVP matrix
   ESMatrix  mvpMatrix;
} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
    GLuint shader;
    GLuint compiled;

    shader = glCreateShader( type );

    if (shader == 0) {
        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, NULL);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if ( !compiled ) {

        GLuint infoLen = 0;

        glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char *infoLog = malloc(sizeof (char) * infoLen);

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
    unsigned char *vShaderStr = 
    "#version 300 es                           \n"
    "uniform mat4 u_mvpMatrix;                 \n"
    "layout (location = 0) in vec3 aPos;       \n"
    "layout (location = 1) in vec4 aColor;     \n"
    "layout (location = 2) in vec2 aTexCoord;  \n"
    "out vec4 v_color;                         \n"
    "out vec2 v_TexCoord;                      \n"
    "void main()                               \n"
    "{                                         \n"
    "   v_color = aColor;                      \n"
    "   v_TexCoord = aTexCoord;                \n"
    "   gl_Position = u_mvpMatrix * vec4(aPos, 1.0);         \n"
    "}                                         \n";

    unsigned char *fShaderStr = 
    "#version 300 es                              \n"
    "precision mediump float;                     \n"
    "uniform sampler2D texture01;                 \n"
    "out vec4 FragColor;                          \n"
    "in vec4 v_color;                             \n"
    "in vec2 v_TexCoord;                          \n"
    "void main()                                  \n"
    "{                                            \n"
    "   FragColor = texture(texture01, v_TexCoord);    \n"
    "}                                            \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLuint linked;

    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    programObject = glCreateProgram();

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {

        GLuint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {

            char *infoLog = malloc ( sizeof (char ) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            esLogMessage ( "Error linking program:\n%s\n", infoLog );

            free ( infoLog );
        }  

        glDeleteProgram(programObject);
        return 0; 
    }

    UserData *userData = esContext->userData;
    userData->programObject = programObject;

                              //pos                   //color                  
    GLfloat vVertices1[] = { -0.9f, -0.9f, 0.0f,      1.0f,  0.0f,  0.0f,  0.0f,     0.0f, 0.0f,  //vertex 01
                             -0.9f,  0.0f, 0.0f,      0.0f,  1.0f,  0.0f,  0.0f,     0.0f, 1.0f,  //vertex 02
                              0.0f, -0.9f, 0.0f,      0.0f,  0.0f,  1.0f,  0.0f,     1.0f, 0.0f,  //vertex 03
                              0.0f,  0.0f, 0.0f,      0.5f,  0.5f,  0.5f,  0.0f,     1.0f, 1.0f,  //vertex 04

                              -0.0f, -0.0f, 0.0f,      1.0f,  0.0f,  0.0f,  0.0f,     0.0f, 0.0f,  //vertex 01
                             -0.0f,  0.9f,  0.0f,      0.0f,  1.0f,  0.0f,  0.0f,     0.0f, 1.0f,  //vertex 02
                              0.9f, -0.0f,  0.0f,      0.0f,  0.0f,  1.0f,  0.0f,     1.0f, 0.0f,  //vertex 03
                              0.9f,  0.9f,  0.0f,      0.5f,  0.5f,  0.5f,  0.0f,     1.0f, 1.0f  //vertex 04
                         };

    GLushort indices01[6] = { 0, 1, 2, 1, 2, 3};

    GLfloat vVertices2[] = { -0.0f, -0.0f, 0.0f,      1.0f,  0.0f,  0.0f,  0.0f,     0.0f, 0.0f,  //vertex 01
                             -0.0f,  0.9f, 0.0f,      0.0f,  1.0f,  0.0f,  0.0f,     0.0f, 1.0f,  //vertex 02
                              0.9f, -0.0f, 0.0f,      0.0f,  0.0f,  1.0f,  0.0f,     1.0f, 0.0f,  //vertex 03
                              0.9f,  0.9f, 0.0f,      0.5f,  0.5f,  0.5f,  0.0f,     1.0f, 1.0f  //vertex 04
                         };

    GLushort indices02[6] = { 0, 1, 2, 1, 2, 3};  

    glGenBuffers(1, &userData->VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices1), vVertices1, GL_STATIC_DRAW);

    glGenBuffers(1, &userData->EBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ( GLushort ) * 6, indices01, GL_STATIC_DRAW);


    //-------------vao1 start 
    glGenVertexArrays(1, &userData->VAO1);

    glBindVertexArray(userData->VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO1);
    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );
    glEnableVertexAttribArray ( 2 );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 4 + 2), (GLvoid*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 4 + 2), (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 4 + 2), (GLvoid*)(7 * sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO1);

    glBindVertexArray(0);
    //--------------vao1 end


    //------------------------------vao2 start 
    glGenVertexArrays(1, &userData->VAO2);

    glBindVertexArray(userData->VAO2);

    // glBindBuffer(GL_ARRAY_BUFFER, userData->VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices1), vVertices1, GL_STATIC_DRAW);

    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );
    glEnableVertexAttribArray ( 2 );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 4 + 2), (GLvoid*)(36 * sizeof(GLfloat) + 0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 4 + 2), (GLvoid*)(36 * sizeof(GLfloat) + 3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 4 + 2), (GLvoid*)(36 * sizeof(GLfloat) + 7 * sizeof(GLfloat)));

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ( GLushort ) * 6, indices01, GL_STATIC_DRAW);

    glBindVertexArray(0);
    //---------------------------------------vao2 end


    //texture01 start
    glGenTextures(1, &userData->texture01);
    glBindTexture(GL_TEXTURE_2D, userData->texture01);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned width1, height1, nrChannels1;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data1 = stbi_load("p02.jpg", &width1, &height1, &nrChannels1, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data1);



    //texture02 start
    glGenTextures(1, &userData->texture02);
    glBindTexture(GL_TEXTURE_2D, userData->texture02);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned width2, height2, nrChannels2;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data2 = stbi_load("p03.jpg", &width2, &height2, &nrChannels2, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data2);


    // Get the uniform locations
   userData->mvpLoc = glGetUniformLocation ( userData->programObject, "u_mvpMatrix" );

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    return TRUE;

}


///
// Update MVP matrix based on time
//
void Update ( ESContext *esContext, float deltaTime )
{
   UserData *userData = esContext->userData;
   ESMatrix perspective;
   ESMatrix modelview;
   float    aspect;

   // Compute a rotation angle based on time to rotate the cube
   userData->angle += ( deltaTime * 60.0f );

   if ( userData->angle >= 360.0f )
   {
      userData->angle -= 360.0f;
   }

   printf("w=====%f \n", userData->angle );
   // Compute the window aspect ratio
   aspect = ( GLfloat ) esContext->width / ( GLfloat ) esContext->height;

   // Generate a perspective matrix with a 60 degree FOV
   esMatrixLoadIdentity ( &perspective );
   esPerspective ( &perspective, 45.0f, aspect, 1.0f, 100.0f );
   // esFrustum(&perspective, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);

   // Generate a model view matrix to rotate/translate the cube
   esMatrixLoadIdentity ( &modelview );

   // Translate away from the viewer
   esTranslate ( &modelview, 0.0f, 0.0f, -3.0f );

   // Rotate the cube
   // esRotate ( &modelview, userData->angle, 0.0, 1.0, 0.0 );
   esRotate ( &modelview, 60.f, 1.0f, 0.0f, 0.0f );

   // Compute the final MVP by multiplying the
   // modevleiw and perspective matrices together
   esMatrixMultiply ( &userData->mvpMatrix, &modelview, &perspective );
}


///
// Draw a triangle using the shader pair created in Init()
//

void Draw ( ESContext *esContext )
{
   
    UserData *userData = esContext->userData;
    

    glViewport(0, 0, esContext->width, esContext->height);

    glClear( GL_COLOR_BUFFER_BIT );https://mp.weixin.qq.com/s/H6eEcpfpCspoOjvhxTMmew

    glUseProgram( userData->programObject );

    glUniformMatrix4fv ( userData->mvpLoc, 1, GL_FALSE, ( GLfloat * ) &userData->mvpMatrix.m[0][0] );
    
    glBindVertexArray(userData->VAO1);
    // glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glBindVertexArray(userData->VAO2);
    // glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

}

void Shutdown ( ESContext *esContext )
{
    UserData *userData = esContext->userData;

    glDeleteProgram ( userData->programObject );
    glDeleteBuffers ( 1, &userData->VBO1 );
    glDeleteBuffers ( 1, &userData->EBO1 );

    glDeleteBuffers ( 1, &userData->texture01 );
    glDeleteBuffers ( 1, &userData->texture02 );

    glDeleteVertexArrays ( 1, &userData->VAO1 );
    glDeleteVertexArrays ( 1, &userData->VAO2 );
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "Hello Triangle", 800, 600, ES_WINDOW_RGB );

    if ( !Init ( esContext ) )
    {
      return GL_FALSE;
    }

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterUpdateFunc ( esContext, Update );
    esRegisterDrawFunc ( esContext, Draw );

    return GL_TRUE;
}


#endif