//
//  Display a rotating cube
//
#define GL_SILENCE_DEPRECATION

#include "Angel.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


typedef vec4  color4;
typedef vec4  point4;

// for moving the object
float projection = 5.0f;
float gravity = 0.01;
GLfloat xPos;
GLfloat yPos;
GLfloat xVel;
GLfloat yVel;
GLfloat previousYVel;
bool firstJump = true;

// for reshaping object according to the width/height of the screen
GLfloat originalWidth = 720;
GLfloat originalHeight = 720;
GLfloat currentWidth = 720;
GLfloat currentHeight = 720;
float scale = originalWidth/originalHeight;
float scaleOrtho = originalWidth/originalHeight;
GLfloat widthScale = 1.0;

// for VAOs and VBOs
GLuint vaoCube;
GLuint vaoSphere;
GLuint vaoBunny;
GLuint sphere_buf;
GLuint cube_buf;
GLuint bunny_buf;

// for color
int colorChange = 1;
vec4 colorWhite = {1.0, 1.0, 1.0, 1.0};
vec4 colorDarkslategray = {47.0/255, 79.0/255, 79.0/255, 1.0};
color4 white = color4( 1.0, 1.0, 1.0, 1.0 );
GLuint fColorUni;

enum {Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3};
GLfloat Theta[NumAxes] = {0.0, 0.0, 0.0};


int drawingMode = 1; // 1 = solid -1 = wireframe

//Object Types
enum {Cube = 0, Sphere = 1, Bunny = 2};

//Display modes
enum {Shading = 0, Wireframe = 1, Texture = 2 };

//Shading options
enum {Gouraud = 0, Phong = 1};

//Light Source Position
enum {Fixed = 0, Moving = 1};

//Material Options
enum {Plastic = 5, Metallic = 10, Thirty = 20, Hundred = 100};

//Component Options
enum {Specular = 0, Diffuse = 1, Ambient = 2, All = 3};

int objectType = Sphere;
int displayMode = Shading;
int shadingOption = Gouraud;
int lightSourcePosition = Fixed;
int materialOption = Plastic;
int componentOption = All;
int bounceFlag = 1;
int textureType = 0;


// from SpinCube example
const int NumVertices_cube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

//  from texbook Appendix 7
const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;

// (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices_sphere = 3 * NumTriangles;
typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

point4 points_sphere[NumVertices_sphere];
vec3   normals_sphere[NumVertices_sphere];
color4 colors_sphere[NumVertices_sphere];

//Bunny datas
int numberOfCoordinates_bunny;
int numberOfIndicies_bunny;

//Bunny coordinates and indicies (read from off file)
vec3 *coordinates;
GLuint *indicies;

// for texture mapping
GLubyte* imageBasketball;
GLubyte* imageEarth;

// I found the dimensions from the get Info of the ppm
const int  sizeXBasketball = 512;
const int sizeYBasketball = 256;
GLubyte basketball[sizeXBasketball][sizeYBasketball][3];

const int  sizeXEarth = 2048;
const int sizeYEarth = 1024;
GLubyte earth[sizeXEarth][sizeYEarth][3];

vec2    tex_coords[NumVertices_sphere];
GLfloat    tex_coords1D[NumVertices_sphere];
GLuint textures[3];

// Vertices of a unit cube centered at origin, sides aligned with axes from SpinCube example
point4 points[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 colors[8] = {
    white,
    white,
    white,
    white,
    white,
    white,
    white,
    white
};


// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

//---------------------------------------------------------------------
//
// from texbook App 7 to draw sphere
//

// TO m
vec2 mapUV (point4 point) {
   
    point4 V = normalize(point);
    double r = 0.58;

    double phi = atan2(V.y, V.x);  // Calculate phi angle
    double theta = acos(V.z / r);  // Calculate theta angle

    double u = phi / (2 * M_PI);
    double v = theta / M_PI;

    // Adjust u and v to handle negative values and wrap around
    if (u < 0) u += 1.0;
    if (v < 0) v += 1.0;

    return vec2(u, v);
    
}

// TO m
GLfloat mapU (point4 point) {
   
    point4 V = normalize(point);
    double r = 0.58;

    double phi = atan2(V.y, V.x);  // Calculate phi angle
    double theta = acos(V.z / r);  // Calculate theta angle

    double u = phi / (2 * M_PI);
    double v = theta / M_PI;

    // Adjust u and v to handle negative values and wrap around
    if (u < 0) u += 1.0;
    if (v < 0) v += 1.0;

    return u;
    
}



int Index = 0;

void
triangle( const point4& a, const point4& b, const point4& c )
{
    vec3 norm = normalize(vec3 (a.x,a.y,a.z));
    tex_coords[Index] = mapUV(a);
    tex_coords1D[Index] = mapU(a);
    normals_sphere[Index] = vec3(norm.x, norm.y, norm.z);;  points_sphere[Index] = a;  Index++;
    norm = normalize(vec3 (b.x,b.y,b.z));
    tex_coords[Index] = mapUV(b);
    tex_coords1D[Index] = mapU(b);
    normals_sphere[Index] = vec3(norm.x, norm.y, norm.z);   points_sphere[Index] = b;  Index++;
    norm = normalize(vec3 (c.x,c.y,c.z));
    tex_coords[Index] = mapUV(c);
    tex_coords1D[Index] = mapU(c);
    normals_sphere[Index] = vec3(norm.x, norm.y, norm.z);;  points_sphere[Index] = c;  Index++;
}
//----------------------------------------------------------------------
point4
unit( const point4& p )
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    
    point4 t;
    if ( len > DivideByZeroTolerance ) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}

void
divide_triangle( const point4& a, const point4& b,
       const point4& c, int count )
{
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle(  a, v1, v2, count - 1 );
        divide_triangle(  c, v2, v3, count - 1 );
        divide_triangle(  b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
}
else {
        triangle( a, b, c );
    }
}
void
tetrahedron( int count )
{
    point4 v[4] = {
        vec4( 0.0, 0.0, 1.0, 1.0 ),
        vec4( 0.0, 0.942809, -0.333333, 1.0 ),
        vec4( -0.816497, -0.471405, -0.333333, 1.0 ),
        vec4( 0.816497, -0.471405, -0.333333, 1.0 )
};
    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}


// to load the file of the bunny
void loadOffFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // OFF
    std::getline(file, line);
    std::istringstream iss(line);

    
    iss >> numberOfCoordinates_bunny >> numberOfIndicies_bunny;
    
    coordinates = new vec3[numberOfCoordinates_bunny];
    indicies = new GLuint[numberOfIndicies_bunny * 3];
    
    //reading the coordinates
    for (int i = 0; i < numberOfCoordinates_bunny; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        iss >> coordinates[i].x >> coordinates[i].y >> coordinates[i].z;
    }
    
    //reading the indicies
    int temp = 0;
       for (int i = 0; i < numberOfIndicies_bunny * 3; i +=3) {
           getline(file, line);
           std::istringstream iss(line);
           iss >> temp >> indicies[i] >> indicies[i+1] >> indicies[i+2];
           
       }
}

GLubyte* readAndInitPpm(const char* imageName) {
    int n, m;
    FILE* fd;
    int k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    fd = fopen(imageName, "r");
    fscanf(fd, "%[^\n]", b);
    if (b[0] != 'P' || b[1] != '3') {
        printf("%s is not a PPM file!\n",b);
        exit(0);
    }
    printf("%s is a PPM file\n",b);
    fscanf(fd, "%c", &c);
    while (c == '#')
    {
        fscanf(fd, "%[^\n]",b);
        printf("%s\n", b);
        fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);

    fscanf(fd, "%d %d %d", &n, &m, &k);
    printf("%d rows %d columns max value= %d\n", n, m, k);
    nm = n * m;
    GLubyte* image = (GLubyte*) malloc(3 * sizeof(GLuint) * nm);
    for (i = nm; i > 0; i--)
    {
        fscanf(fd, "%d %d %d", &red, &green, &blue);
        image[3 * nm - 3 * i] = red;
        image[3 * nm - 3 * i + 1] = green;
        image[3 * nm - 3 * i + 2] = blue;
    }
    return image;
    
}

// Dimensions of the texture
const int width = 256;
const int height = 1;

// Array to store the texture data
const int textureSize = 256;
const int lineWidth = 16;
unsigned char texture1D[textureSize][3];
//GLubyte texture1D[2] = { 0, 255 };
// Generate the 1D texture with different colored pixels
void generate1DTexture()
{
    for (int i = 0; i < textureSize; i++) {
            if (i % lineWidth == 0) {
                // Set red color for line pattern
                texture1D[i][0] = 255; // Red
                texture1D[i][1] = 0;   // Green
                texture1D[i][2] = 0;   // Blue
            } else {
                // Set blue color for the rest
                texture1D[i][0] = 0;   // Red
                texture1D[i][1] = 0;   // Green
                texture1D[i][2] = 255; // Blue
            }
        }
}



// Initialize shader lighting parameters
point4 light_position(1.0, 0, 1.0, 0 ); //directional light source
color4 light_ambient( 0.2, 0.2, 0.2, 1.0 ); // L_a
color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 ); // L_d
color4 light_specular( 1.0, 1.0, 1.0, 1.0 ); // L_s

color4 material_ambient( 1.0, 0.0, 1.0, 1.0 ); // k_a
color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 ); // k_d
color4 material_specular( 1.0, 0.8, 0.0, 1.0 ); // k_s
float  material_shininess = Hundred;

//color4 material_ambient( 0.25, 0.25, 0.25, 1.0 ); // k_a
//color4 material_diffuse( 0.4, 0.4, 0.4, 1.0 );  // k_d
//color4 material_specular(0.774597, 0.774597, 0.774597, 1.0 );// k_s
//float  material_shininess = 0.6 * 126;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;

mat4 projectionMat;

GLuint program, DisplayMode, ShadingOption, LightSourcePosition, ComponentOption, TextureOption;

//unsigned int depthMapFBO;
//
//const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//
//unsigned int depthMap;

void init()
{
    // draw the sphere from Appendix 7
    tetrahedron( NumTimesToSubdivide );
    
    //get the images from ppm
    imageBasketball = readAndInitPpm("basketball.ppm");
    imageEarth = readAndInitPpm("earth.ppm");
    generate1DTexture();
    
    // default color white
    colorChange = 1;
    
    // initialize the objects location and the gravity according to the inital screen and projection
    xPos = -projection;
    yPos = 5;
    xVel = 5 * gravity;
    yVel = gravity;
    previousYVel = yVel;
  
    // Load and use the resulting shader program
    program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );
    
    // --------------------CUBE------------------------------------------------------
    // Create a VAO for cube (from SpinCube)
    glGenVertexArrays( 1, &vaoCube );
    glBindVertexArray( vaoCube );
    // Create and initialize a buffer object for cube
    glGenBuffers( 1, &cube_buf );
    glBindBuffer( GL_ARRAY_BUFFER, cube_buf );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    
    GLuint cube_indices[] = {
            0, 1, 2,
            2, 3, 0,
            1, 5, 6,
            6, 2, 1,
            7, 6, 5,
            5, 4, 7,
            4, 0, 3,
            3, 7, 4,
            4, 5, 1,
            1, 0, 4,
            3, 2, 6,
            6, 7, 3
        };
    
    // Create and initialize an index buffer object for cube (from SpinCube)
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    // get the fragment shader's color location to change the color of the object  using shader
    fColorUni = glGetUniformLocation( program, "color" );
    
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );
    glUniform4fv(fColorUni, 1, colorWhite);
    // --------------------------------------------------------------------------------
    
    // --------------------TEXTURE---------------------------------------------------
    
    glGenTextures( 3, textures );
    
    // for Basketball texture
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, sizeXBasketball, sizeYBasketball, 0,
              GL_RGB, GL_UNSIGNED_BYTE, imageBasketball );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // for Earth texture
    glBindTexture( GL_TEXTURE_2D, textures[1] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, sizeXEarth, sizeYEarth, 0,
              GL_RGB, GL_UNSIGNED_BYTE, imageEarth );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        
    // for 1D texture
    glActiveTexture( GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_1D, textures[2] );
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, textureSize, 0, GL_RED, GL_UNSIGNED_BYTE, texture1D);
    //glGenerateMipmap(GL_TEXTURE_1D);
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        
    glBindTexture( GL_TEXTURE_2D, textures[0] ); //set current texture
    // --------------------------------------------------------------------------------
    
    
    // --------------------SPHERE------------------------------------------------------
    // Create a vertex array object for sphere (from Appendix 7)
    
    glGenVertexArrays( 1, &vaoSphere );
    glBindVertexArray( vaoSphere );
    // Create and initialize a buffer object (from Appendix 7)
    glGenBuffers( 1, &sphere_buf );
    glBindBuffer( GL_ARRAY_BUFFER, sphere_buf );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points_sphere) + sizeof(normals_sphere) + sizeof(tex_coords) + sizeof(tex_coords1D) , NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points_sphere), points_sphere );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_sphere),sizeof(normals_sphere), normals_sphere );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_sphere) + sizeof(normals_sphere), sizeof(tex_coords), tex_coords );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_sphere) + sizeof(normals_sphere) + sizeof(tex_coords),sizeof(tex_coords1D), tex_coords1D );
    // set up vertex arrays
    GLuint vPosition_sphere = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition_sphere );
    glVertexAttribPointer( vPosition_sphere, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    GLuint vNormal_sphere = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal_sphere );
    glVertexAttribPointer( vNormal_sphere, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_sphere)) );
    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
           BUFFER_OFFSET(sizeof(points_sphere) + sizeof(normals_sphere)));
    GLuint vTexCoord1D = glGetAttribLocation(program, "vTexCoord1D");
    glEnableVertexAttribArray(vTexCoord1D);
    glVertexAttribPointer(vTexCoord1D, 1, GL_FLOAT, GL_FALSE, 0,
           BUFFER_OFFSET(sizeof(points_sphere) + sizeof(normals_sphere)+ sizeof(tex_coords)));
    // --------------------------------------------------------------------------------
    
    // --------------------BUNNY------------------------------------------------------
    
    glGenVertexArrays( 1, &vaoBunny );
    glBindVertexArray( vaoBunny );
    // Create and initialize a buffer object for cube
    glGenBuffers( 1, &bunny_buf );
    glBindBuffer( GL_ARRAY_BUFFER, bunny_buf );

    glBufferData( GL_ARRAY_BUFFER, sizeof(coordinates) * numberOfCoordinates_bunny, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(coordinates) * numberOfCoordinates_bunny, coordinates );
    
    GLuint index_buffer_bunny;
    glGenBuffers(1, &index_buffer_bunny);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_bunny);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies) * numberOfIndicies_bunny, indicies, GL_STATIC_DRAW);
    
    GLuint vPosition_bunny = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition_bunny );
    glVertexAttribPointer( vPosition_bunny, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    // --------------------------------------------------------------------------------
//    unsigned int depthMapFBO;
//    glGenFramebuffers(1, &depthMapFBO);
//
//    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//
//    unsigned int depthMap;
//    glGenTextures(1, &depthMap);
//    glBindTexture(GL_TEXTURE_2D, depthMap);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
//                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//    glDrawBuffer(GL_NONE);
//    glReadBuffer(GL_NONE);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // ----------------------------------Shadow----------------------------------------
    
    

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
                 1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
                 1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
                 1, specular_product );
    
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
                 1, light_position);
    
    glUniform1f( glGetUniformLocation(program, "Shininess"),
                material_shininess );
    
    DisplayMode = glGetUniformLocation(program, "DisplayMode");
    ShadingOption = glGetUniformLocation(program, "ShadingOption");
    LightSourcePosition = glGetUniformLocation(program, "LightSourcePosition");
    ComponentOption = glGetUniformLocation(program, "ComponentOption");
    TextureOption = glGetUniformLocation(program, "TextureOption");
    
    glUniform1f( DisplayMode, displayMode );
    glUniform1f( ShadingOption, shadingOption );
    glUniform1f( LightSourcePosition, lightSourcePosition );
    glUniform1f( ComponentOption, componentOption );
    glUniform1f( TextureOption, textureType );
    
    
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    
    // initialize a projection using Ortho
    projectionMat = Perspective(45.0, (double)currentWidth / (double)currentHeight, 0.1, 100.0 );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projectionMat );
    
//    mat4  projection = Perspective( 45.0, scale, 0.5, 6.0 );
//    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
    
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_CULL_FACE); //to discard invisible faces from rendering
    
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); /* white background */
}

float x,z;
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //  Generate the model-view matrix
    vec3 displacement( xPos, yPos, -12.0f);
    mat4   model_view;
    
    //Cube
    if (objectType == Cube) {
        //draw the cube from its VAO
        glBindVertexArray(vaoCube);
        glDrawElements(GL_TRIANGLES, NumVertices_cube, GL_UNSIGNED_INT, 0);
        //rotate the cube
        model_view = (Translate(displacement) * Scale(scale, scale, scale) * RotateX( 15 ) * RotateY( 15 ) * RotateZ( 15 ));
    }
    //Sphere
    else if (objectType == Sphere)  {
        //draw the sphere from its VAO
        glBindVertexArray(vaoSphere);
        //draw the sphere using glDrawArray (from Appendix 7)
        glDrawArrays( GL_TRIANGLES, 0, NumVertices_sphere );
        
        // Bouncing sphere
        if(bounceFlag) {
            model_view =  (model_view * Translate(displacement) * Scale(scale * 0.5f, scale * 0.5f, scale * 0.5f) * ( RotateX( Theta[Xaxis] - x) * RotateY( Theta[Yaxis] ) * RotateZ( Theta[Zaxis] + z)));
        }
        // Fixed sphere
        else {
            model_view =  (model_view * Translate(0,0,-12.0f) * Scale(scale, scale, scale) * ( RotateX( Theta[Xaxis] -  x) * RotateY( Theta[Yaxis] ) * RotateZ( Theta[Zaxis] + z)));
        }
    }
    //Bunny
    else if (objectType == Bunny)  {
        glBindVertexArray(vaoBunny);
        glDrawElements(GL_TRIANGLES, numberOfCoordinates_bunny * 3, GL_UNSIGNED_INT, 0);
        model_view = (Translate(displacement) * Scale(scale*0.04f, scale*0.04f, scale*0.04f)) * RotateY( 90 ) * RotateX( 270 );
    }
    //Solid drawing mode
    if(displayMode == Wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    //Wireframe drawing mode
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    
//    // 1. first render to depth map
//    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glClear(GL_DEPTH_BUFFER_BIT);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    // 2. then render scene as normal with shadow mapping (using depth map)
//    glViewport(0, 0, originalWidth, originalHeight);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glBindTexture(GL_TEXTURE_2D, depthMap);
//
//    mat4 lightView = LookAt(1, 0, 1);
//
//    mat4 lightSpaceMatrix = projectionMat * lightView;
//
//    GLuint LightSpaceMatrix =  glGetUniformLocation( program, "LightSpaceMatrix" );
//    glUniformMatrix4fv( LightSpaceMatrix, 1, GL_TRUE, lightSpaceMatrix );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); //may not need this since the default is usually the window size
    
    // Set projection matrix
    mat4  projection;
    if (width <= height){
        //if the height of the window is larger then we adjust the bottom and top projection
//        projection = Ortho(-5.0, 5.0, -5.0 * (GLfloat) height / (GLfloat) width,
//                           5.0 * (GLfloat) height / (GLfloat) width, -5, 5);
        projection = Perspective(45.0, (double)currentWidth / (double)currentHeight, 0.1, 100.0 );
        // adjusting the movement of the object according to the ortho projection
        xVel = width/originalWidth * gravity * 5;
        yVel *= height/originalHeight;
    }
    else{
//        projection = Ortho(-5.0 * (GLfloat) width / (GLfloat) height, 5.0 *
//                             (GLfloat) width / (GLfloat) height, -5.0, 5.0, -5, 5);
        projection = Perspective(45.0, (double)currentWidth / (double)currentHeight, 0.1, 100.0 );
        // adjusting the movement of the object according to the ortho projection
        xVel = width/originalWidth * gravity * 5;
        yVel *= height/originalHeight;
    }
    currentWidth = width;
    currentHeight = height;
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) {
        switch( key ) {
            case GLFW_KEY_ESCAPE: case GLFW_KEY_Q:
                exit( EXIT_SUCCESS );
                break;
                
            // "Zoom-in" to the object
            case GLFW_KEY_Z:
                scale *= 1.1;
                break;
                
            // "Zoom-out" from the object
            case GLFW_KEY_W:
                scale *= 0.9;
                break;
            // Rotate up the object
            case GLFW_KEY_UP:
                Theta[Xaxis] += 6.0;
                
                if (Theta[Xaxis] > 360.0){
                    Theta[Xaxis] -= 360.0;
                }
                break;
            // Rotate down the object
            case GLFW_KEY_DOWN:
                Theta[Xaxis] -= 6.0;
                
                if (Theta[Xaxis] < -360.0){
                    Theta[Xaxis] += 360.0;
                }
                break;
            // Rotate left the object
            case GLFW_KEY_LEFT:
                Theta[Zaxis] += 6.0;
                
                if (Theta[Zaxis] > 360.0){
                    Theta[Zaxis] -= 360.0;
                }
                break;
            // Rotate right the object
            case GLFW_KEY_RIGHT:
                Theta[Zaxis] -= 6.0;
                
                if (Theta[Zaxis] < -360.0){
                    Theta[Zaxis] += 360.0;
                }
                break;
                
            // Display mode
            case GLFW_KEY_T:
                displayMode += 1;
                displayMode = displayMode % 3;
                
                if(displayMode == Texture) {
                    x = 90;
                    z = 90;
                }
                else {
                    x = 0;
                    z = 0;
                }
                
                glUniform1i(DisplayMode, displayMode);
                break;
                
            // Shading options
            case GLFW_KEY_S:
                shadingOption += 1;
                shadingOption = shadingOption % 2;
                
                std::cout << "Shading Option: " << shadingOption;
                putchar('\n');
                
                glUniform1i(ShadingOption, shadingOption);
                break;
            
            //Light Source Position
            case GLFW_KEY_L:
                lightSourcePosition += 1;
                lightSourcePosition = lightSourcePosition % 2;
                
                std::cout << "Light Source Position: " << lightSourcePosition;
                putchar('\n');
                
                glUniform1i(LightSourcePosition, lightSourcePosition);
                break;
            
            //Material Options
            case GLFW_KEY_M:
                if(material_shininess == Plastic) {
                    material_shininess = Metallic;
                }
                else if(material_shininess == Metallic) {
                    material_shininess = Thirty;
                }
                else if(material_shininess == Thirty) {
                    material_shininess = Hundred;
                }
                else {
                    material_shininess = Plastic;
                }
                glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );
                break;
                
            //Component options
            case GLFW_KEY_O:
                componentOption += 1;
                componentOption = componentOption % 4;
                
                std::cout << "Component Option: " << componentOption;
                putchar('\n');
                
                if(componentOption == 0) {
                    specular_product = vec4(0, 0, 0, 0);
                }
                
                else if(componentOption == 1) {
                    diffuse_product = vec4(0, 0, 0, 0);

                }
                else if (componentOption == 2) {
                    ambient_product = vec4(0, 0, 0, 0);
                }
                
                else {
                    
                    ambient_product = light_ambient * material_ambient;
                    diffuse_product = light_diffuse * material_diffuse;
                    specular_product = light_specular * material_specular;
                }
                glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
                             1, specular_product );
                glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
                             1, diffuse_product );
                glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
                             1, ambient_product );
                
                break;
            // Bounce toggle
            case GLFW_KEY_F:
                bounceFlag += 1;
                bounceFlag = bounceFlag % 2;
                break;
            
            case GLFW_KEY_R:
                // to adjust the position of the object to the top left corner of the window (according to the ortho projection
                if ( currentWidth > currentHeight) {
                    // changing the position and the velocity of the x axis
                    xPos = - 5.0 * (GLfloat) currentWidth / (GLfloat) currentHeight;
                    yPos = 5;
                    gravity = 0.01f;
                    xVel = 5 *gravity * (GLfloat) currentWidth / ((GLfloat) currentHeight);
                }else {
                    // changing the position and the velocity of the y axis
                    xPos = - 5.0;
                    gravity = 0.01f * (GLfloat) currentHeight / (GLfloat) currentWidth;
                    yPos = 5 * (GLfloat) currentHeight / (GLfloat) currentWidth;
                    xVel = 5 *0.01;
                }
                yVel=gravity;
                previousYVel = yVel;
                firstJump = true;
                break;
            
            case GLFW_KEY_I: case 39:
                //change the textures
                textureType += 1;
                textureType = textureType % 3;
                std::cout << "Texture Option: " << textureType;
                if(textureType == 0){
                    //basketball texture
                    glBindTexture(GL_TEXTURE_2D, textures[0]);
                    std::cout << "Basketball" ;
                    putchar('\n');
                } else if (textureType == 1){
                    //earth texture
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    std::cout << "Earth ";
                    putchar('\n');
                } else {
                    glBindTexture(GL_TEXTURE_1D, textures[2]);
                    std::cout << "1D Texture";
                    putchar('\n');
                }
                glUniform1i(TextureOption, textureType);
                break;
                
            // Change color of the object
            case GLFW_KEY_C:
                colorChange *= -1;
                
                if (colorChange == 1){
                    glUniform4fv(fColorUni, 1, colorWhite);
                } else {
                    glUniform4fv(fColorUni, 1, colorDarkslategray);
                }
                break;
                
            //Print input controls
            case GLFW_KEY_H:
                std::cout << "Input Controls:" << std::endl;
                std::cout << "r -- Initialize the pose (top left corner of the window)" << std::endl;
                std::cout << "t -- three different modes (Shaing - Wireframe - Texture) " << std::endl;
                std::cout << "s -- Switch between Gouraud and Phong shading options" << std::endl;
                std::cout << "o -- turn off the specular, diffuse and ambient components one by one" << std::endl;
                std::cout << "i -- Switch between different textures" << std::endl;
                std::cout << "l -- Switch between fixed in position the light source or moving with the object" << std::endl;
                std::cout << "m -- Switch between material properties" << std::endl;
                std::cout << "z -- Zoom-in the object" << std::endl;
                std::cout << "w -- Zoom-out the object" << std::endl;
                std::cout << "c -- Switch between two colors , which is used to draw lines or triangles."<< std::endl;
                std::cout << "h -- Help; print explanation of your input control (simply to the command line"<< std::endl;
                std::cout << "q -- Quit (exit) the program" << std::endl;
                break;
        }
    }
    
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if ( action == GLFW_PRESS ) {
        switch( button ) {
            case GLFW_MOUSE_BUTTON_RIGHT:
                // changin the type of the object
                objectType += 1;
                objectType = objectType % 3;
                break;
        }
    }
}


void moveObject(GLfloat offlen, GLfloat heightScale) {
    
    // move the object
    yVel -= gravity;
    xPos += xVel;
    yPos += yVel;
    
    // condition to check if the object touches the ground
    if ((yPos <= (-projection * heightScale) + offlen)) {
    
        if(firstJump){
            previousYVel = yVel;
            firstJump = false;
        }
        // to make the object stop
        if (yVel < previousYVel){
            yPos = -projection * heightScale + offlen;
            yVel = 0;
            xVel = 0;
            gravity = 0;
        }
        previousYVel = yVel;
        yVel *= -1.0;
        gravity *= 1.4;
    }
}

void update( void )
{
    // to update object's location (according to the ortho projection)
    if (currentHeight>currentWidth) moveObject(scaleOrtho/2,(GLfloat) currentHeight / (GLfloat) currentWidth);
    else moveObject(scaleOrtho/2 ,1.0);
}


int main()
{
    // to load the coordinates and the indices of the bunny
    loadOffFile("bunny.off");
    
    if (!glfwInit())
            exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(originalWidth, originalHeight, "HW1", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    init();

    double frameRate = 120, currentTime, previousTime = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        if (currentTime - previousTime >= 1/frameRate){
            previousTime = currentTime;
            update();
        }
        
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

