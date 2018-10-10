
#include "esUtil.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//camera
#if 1


typedef struct
{
    GLuint  programObject;
    GLuint  VBO1;
    GLuint  EBO1;
    GLuint  VAO1;
    GLuint  texture01;

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
    "layout (location = 1) in vec2 aTexCoord;  \n"
    "out vec2 v_TexCoord;                      \n"
    "void main()                               \n"
    "{                                         \n"
    "   v_TexCoord = aTexCoord;                \n"
    "   gl_Position = u_mvpMatrix * vec4(aPos, 1.0f);         \n"
    "}                                         \n";

    unsigned char *fShaderStr = 
    "#version 300 es                              \n"
    "precision mediump float;                     \n"
    "uniform sampler2D texture01;                 \n"
    "uniform sampler2D texture02;                 \n"
    "out vec4 FragColor;                          \n"
    "in vec2 v_TexCoord;                          \n"
    "void main()                                  \n"
    "{                                            \n"
    "   FragColor = mix( texture(texture01, v_TexCoord), texture(texture02, v_TexCoord), 0.5 );    \n"
    // "   FragColor = texture(texture01, v_TexCoord);    \n"
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

                              //pos               //texcoord                  
    GLfloat vVertices1[] = {                        
                            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

                            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

                            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

                            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
                         };

    // GLushort indices01[6] = { 0, 1, 2, 1, 2, 3};


    glGenBuffers(1, &userData->VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices1), vVertices1, GL_STATIC_DRAW);

    // glGenBuffers(1, &userData->EBO1);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO1);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ( GLushort ) * 6, indices01, GL_STATIC_DRAW);


    //-------------vao1 start 
    glGenVertexArrays(1, &userData->VAO1);

    glBindVertexArray(userData->VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, userData->VBO1);
    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 2), (GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*(3 + 2), (GLvoid*)(3 * sizeof(GLfloat)));

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->EBO1);

    glBindVertexArray(0);
    //--------------vao1 end


    //texture01 start
    glGenTextures(1, &userData->texture01);
    glBindTexture(GL_TEXTURE_2D, userData->texture01);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned width1, height1, nrChannels1;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data1 = stbi_load("p01.jpg", &width1, &height1, &nrChannels1, 0);
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
    //texture01 end

    //texture02 start
    glGenTextures(1, &userData->texture02);
    glBindTexture(GL_TEXTURE_2D, userData->texture02);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned width2, height2, nrChannels2;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data2 = stbi_load("1536284484533.jpg", &width2, &height2, &nrChannels2, 0);
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
    //texture02 end


    glUseProgram(userData->programObject);
    glUniform1i(glGetUniformLocation(userData->programObject, "texture01"), 0); 
    glUniform1i(glGetUniformLocation(userData->programObject, "texture02"), 1); 
    // Get the uniform locations
    userData->mvpLoc = glGetUniformLocation ( userData->programObject, "u_mvpMatrix" );

    glEnable ( GL_DEPTH_TEST );

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

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
   ESMatrix view;
   float    aspect;


   // Compute a rotation angle based on time to rotate the cube
   userData->angle += ( deltaTime * 60.0f );

   if ( userData->angle >= 360.0f )
   {
      userData->angle -= 360.0f;
   }
#if 0
   // printf("w=====%f \n", userData->angle );
   // Compute the window aspect ratio
   aspect = ( GLfloat ) esContext->width / ( GLfloat ) esContext->height;

   // Generate a perspective matrix with a 60 degree FOV
   esMatrixLoadIdentity ( &perspective );
   esPerspective ( &perspective, 45.0f, aspect, 1.0f, 100.0f );
   // esFrustum(&perspective, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);

   // Generate a model view matrix to rotate/translate the cube
   esMatrixLoadIdentity ( &modelview );

   // Translate away from the viewer
   esTranslate ( &modelview, 0.0f, 0.0f, -2.0f );

   // Rotate the cube
   esRotate ( &modelview, userData->angle, 1.0, 1.0, 1.0 );
   // esRotate ( &modelview, 60.f, 1.0f, 0.0f, 0.0f );

   // esMatrixMultiply ( &userData->mvpMatrix, &modelview, &perspective );

    esMatrixLoadIdentity ( &view );
    esMatrixLookAt(&view, 0.0f, 0.0f, 0.1f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f);
    printf("w-----------------%f\n",deltaTime );
    esMatrixMultiply ( &userData->mvpMatrix, &modelview, &view );

   esMatrixMultiply ( &userData->mvpMatrix, &userData->mvpMatrix, &perspective );
#endif
}

void UpdateMatri( ESContext *esContext, GLfloat x, GLfloat y, GLfloat z, int i)
{
    UserData *userData = esContext->userData;
    ESMatrix perspective;
    ESMatrix modelview;
    ESMatrix view;
    float    aspect;


    // Compute the window aspect ratio
    aspect = ( GLfloat ) esContext->width / ( GLfloat ) esContext->height;

    esMatrixLoadIdentity ( &perspective );
    esPerspective ( &perspective, 45.0f, aspect, 1.0f, 100.0f );
    // esFrustum(&perspective, -5.0f, 5.0f, -5.0f, 5.0f, 1.f, 50.0f);

    esMatrixLoadIdentity ( &modelview );

    // Translate away from the viewer
    printf("w-------------%f\n",userData->angle );
    esTranslate ( &modelview, x, y, z - 3.f );

    // Rotate the cube
    esRotate ( &modelview, userData->angle, x, y * userData->angle, z );
    // esRotate ( &modelview, 0.0f, 0.0, 1.0, 0.0 );
    float radius = 20.f;

    esMatrixLoadIdentity ( &view );
    esMatrixLookAt(&view, radius * sin(userData->angle * 0.01), 0.0f, radius * cos(userData->angle * 0.01),  0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f);
    esMatrixMultiply ( &userData->mvpMatrix, &modelview, &view );

    esMatrixMultiply ( &userData->mvpMatrix, &userData->mvpMatrix, &perspective );


}

///
// Draw a triangle using the shader pair created in Init()
//

void Draw ( ESContext *esContext )
{
   
    UserData *userData = esContext->userData;
    

    glViewport(0, 0, esContext->width, esContext->height);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( userData->programObject );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->texture01);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, userData->texture02);


    // glUniformMatrix4fv ( userData->mvpLoc, 1, GL_FALSE, ( GLfloat * ) &userData->mvpMatrix.m[0][0] );
    
    GLfloat cubePositions[10][3] = {
        {0.0f,  0.0f, -3.0f},
        {2.0f,  0.0f, -5.0f},
        {-2.f,  0.0f, -5.5f},
        {-3.8f, -2.0f, -12.3f},
        { 2.4f, -0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        {1.3f, -2.0f, -2.5f},
        {1.5f,  2.0f, -2.5f},
        { 0.0f,  2.f, -7.5f},
        { 0.0f, -2.0f, -8.5f}
    };
    int i = 0;
    glBindVertexArray(userData->VAO1);
    for (; i < 10; i++)
    {
        UpdateMatri(esContext, cubePositions[i][0], cubePositions[i][1], cubePositions[i][2], i);
        glUniformMatrix4fv ( userData->mvpLoc, 1, GL_FALSE, ( GLfloat * ) &userData->mvpMatrix.m[0][0] );
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);


    // glBindVertexArray(userData->VAO1);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0);

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
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "Hello Triangle", 800, 600, ES_WINDOW_RGB | ES_WINDOW_DEPTH );

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