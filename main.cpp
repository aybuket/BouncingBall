//
//  main.cpp
//  graphics_bouncing_ball
//
//  Created by Aybüke Buket Akgül on 28.03.2021.
//  Copyright © 2021 Aybüke Buket Akgül. All rights reserved.
//

#include "Angel.h"
#include <iostream>
#include <fstream>
using namespace std;
typedef vec4  point4;
typedef vec4 color4;

// Default values
color4 object_color = color4(1.0, 0.0, 0.0, 0.0);
GLenum drawing_mode = GL_TRIANGLES;

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

// buffers
GLuint cubeBuffer, sphereBuffer, bunnyBuffer;

// vertex array, attributes, program
GLuint vao[3], vPosition, vColor, program;

const int cubeNumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
const int sphereNumVertices = 100;

point4 points[cubeNumVertices];
color4 colors[cubeNumVertices];

point4 * bunnyVertices;
point4 * bunnyPoints;
color4 * bunnyColors;
int bunnyVertexNum;
int bunnyPointNum;

point4 sphereVertices[sphereNumVertices];
point4 spherePoints[3*(sphereNumVertices-1)];
color4 sphereColors[3*(sphereNumVertices-1)];

// original size
float original_x = 1024;
float original_y = 480;

// initial point
float a_init = -original_x/20; float a = -original_x/20;
float b = original_y/12; float ceeling = original_y/12; float ceeling_init = original_y/12;
float gravity = -0.98;

// enums for menu selection
enum ObjectType { Cube, Sphere, Bunny };
enum DrawingMode { Wireframe, Solid };
enum Color { Red, Green, Blue, Magenta, Cyan, Yellow, White, Black };

// define current object and related info to use globally
ObjectType currentObject = Cube;
int currentCount = cubeNumVertices;

//----------------------------------------------------------------------------
void fill_circle()
{
    for(int i=0; i<sphereNumVertices; i++)
    {
        float angle = i*360.0 / sphereNumVertices;
        sphereVertices[i] = point4(1.0*cos(angle), 1.0*sin(angle), 0.0, 1.0);
        sphereColors[i] = object_color;
    }
    
    for(int i=1; i<sphereNumVertices-1; i++)
    {
        spherePoints[i*3] = sphereVertices[0];
        spherePoints[i*3+1] = sphereVertices[i+1];
        spherePoints[i*3+2] = sphereVertices[i+2];
        sphereColors[i*3] = object_color;
        sphereColors[i*3+1] = object_color;
        sphereColors[i*3+2] = object_color;
    }
}

//----------------------------------------------------------------------------
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad( int a, int b, int c, int d )
{
    colors[Index] = object_color; points[Index] = vertices[a]; Index++;
    colors[Index] = object_color; points[Index] = vertices[b]; Index++;
    colors[Index] = object_color; points[Index] = vertices[c]; Index++;
    colors[Index] = object_color; points[Index] = vertices[a]; Index++;
    colors[Index] = object_color; points[Index] = vertices[c]; Index++;
    colors[Index] = object_color; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------
// initialize buffer for cube object
void initialize_cube_buffer(){
    glGenBuffers( 1, &cubeBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, cubeBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
                 NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );
}

//initialize buffer for sphere object
void initialize_sphere_buffer(){
    glGenBuffers( 1, &sphereBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, sphereBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(spherePoints) + sizeof(sphereColors),
                 NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(spherePoints), spherePoints );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(spherePoints), sizeof(sphereColors), sphereColors);
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(spherePoints)) );
}

// initialize buffer for bunny object
void initialize_bunny_buffer()
{
    glGenBuffers( 1, &bunnyBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, bunnyBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(point4)*bunnyPointNum*2,
                 NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*bunnyPointNum, bunnyPoints );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*bunnyPointNum, sizeof(color4)*bunnyPointNum, bunnyColors);
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*bunnyPointNum) );
}

void read_bunny();

void
init( void )
{
    colorcube();
    fill_circle();
    read_bunny();
    
    // Create a vertex array object
    glGenVertexArrays( 3, vao );
    
    // Load shaders and use the resulting shader program
    program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );
    
    // Initialize the vertex position attribute from the vertex shader
    vPosition = glGetAttribLocation( program, "vPosition" );
    vColor = glGetAttribLocation( program, "vColor" );
    
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    
    // Cube
    glBindVertexArray( vao[0] );
    initialize_cube_buffer();
    
    // Sphere
    glBindVertexArray( vao[1] );
    initialize_sphere_buffer();
    
    // Bunny
    glBindVertexArray( vao[2] );
    initialize_bunny_buffer();
    
    // Draw cube first
    glBindVertexArray( vao[0] );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
    
    //  Generate the model-view matrix, move both horizontal and vertical
    const vec3 displacement( 0.0+a, 0.0+b, 0.0 );
    mat4  model_view ;
    if (currentObject == Cube)
    {
        model_view = (Translate( displacement ) * Scale(5, 5, 5) *
                      RotateX(120) *
                      RotateY(120) *
                      RotateZ(120));  // Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h
    }
    else if (currentObject == Bunny)
    {
        model_view = (Translate( displacement ) * Scale(0.75, 0.75, 0.75) *
                      RotateX(90) *
                      RotateY(180) *
                      RotateZ(-50));  // Scale(), Translate(), RotateX(), RotateY(), RotateZ(): user-defined functions in mat.h
    }
    else {
        model_view = (Translate( displacement ) * Scale(4, 4, 4));
    }
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    
    glDrawArrays( drawing_mode, 0, currentCount );
    
    glutSwapBuffers();
}

//----------------------------------------------------------------------------
void reset();
void print_help();
void change_color();
void color_popup_handling(int item);
bool stop = false;

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
        case 'I':
        case 'i':
            reset(); // return to the inital point
            break;
        case 'H':
        case 'h':
            print_help(); // print info to console
            break;
        case 'Q':
        case 'q':
            exit( EXIT_SUCCESS ); // quit
            break;
        case 'c':
        case 'C':
            color_popup_handling(Color(rand() % 8)); //choose random color for object
            change_color();
            break;
        case 's':
        case 'S':
            stop = !stop; //stop&start animation
            break;
        case 'd':
        case 'D':
            if (drawing_mode == GL_LINES)
            {
                drawing_mode = GL_TRIANGLES;
            }
            else
            {
                drawing_mode = GL_LINES;
            }
            break;
    }
}

// print helper information to console.
void print_help()
{
    std::cout << "------------------------------" << std::endl;
    std::cout << "Right click to choose objects:" << std::endl;
    std::cout << "--Cube, Sphere, Bunny" << std::endl;
    std::cout << "Right click to choose mode:" << std::endl;
    std::cout << "--Wireframe, Solid" << std::endl;
    std::cout << "Right click to choose color:" << std::endl;
    std::cout << "--Red, Green, Blue, Magenta, Cyan, Yellow, White, Black" << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::cout << "Q/q : Quit" << std::endl;
    std::cout << "C/c : Change color to random color" << std::endl;
    std::cout << "S/s : Stop/start animation" << std::endl;
    std::cout << "H/h : Helper" << std::endl;
    std::cout << "I/i : Initialize" << std::endl;
    std::cout << "D/d : Change drawing mode" << std::endl;
}

// return to the initial point
void reset(){
    a = a_init;
    b = ceeling_init;
    ceeling = ceeling_init;
    return;
}
//----------------------------------------------------------------------------
// Sets the new color to the all object but only initialize the current object.
void change_color()
{
    for (int i = 0; i<cubeNumVertices; i++)
    {
        colors[i] = object_color;
    }
    
    for (int i = 0; i<bunnyPointNum; i++)
    {
        bunnyColors[i] = object_color;
    }
    
    for (int i = 0; i<3*(sphereNumVertices-1); i++)
    {
        sphereColors[i] = object_color;
    }
    
    switch (currentObject) {
        case Cube:
            currentCount = cubeNumVertices;
            glBindVertexArray( vao[0] );
            initialize_cube_buffer();
            break;
        case Sphere:
            currentCount = (sphereNumVertices-1)*3;
            glBindVertexArray( vao[1] );
            initialize_sphere_buffer();
            break;
        case Bunny:
            currentCount = bunnyPointNum;
            glBindVertexArray( vao[2] );
            initialize_bunny_buffer();
        default:
            break;
    }
    
}

//----------------------------------------------------------------------------
// Read .off file to create bunny
void read_bunny()
{
    string line;
    string filename ="/Users/Aybuke/Documents/graphics_bouncing_ball/graphics_bouncing_ball/bunny.off";
    ifstream myfile (filename);
    if (!myfile.is_open()) {
        std::cerr << "Could not open the file - '"
        << filename << "'" << endl;
    }
    
    myfile >> line;
    if ( line == "OFF" )
    {
        int edge_count;
        myfile >> bunnyVertexNum;
        myfile >> bunnyPointNum;
        myfile >> edge_count;
        
        // second part of the file contains indices of an triangle, so makes 3 points.
        bunnyPointNum*=3;
        bunnyVertices = new point4[bunnyVertexNum];
        bunnyPoints = new point4[bunnyPointNum];
        bunnyColors = new point4[bunnyPointNum];
        
        // (x,y,z) coordinates of vertices.
        for ( int v = 0; v < bunnyVertexNum ; v++ )
        {
            float x; float y; float z;
            myfile >> x;
            myfile >> y;
            myfile >> z;
            bunnyVertices[v] = point4(x, y, z, 1.0);
        }
        
        // indices of triangles
        for ( int t = 0; t < bunnyPointNum ; t+=3 )
        {
            int n; int x; int y; int z;
            myfile >> n;
            myfile >> x;
            myfile >> y;
            myfile >> z;
            bunnyPoints[t] = bunnyVertices[x];
            bunnyPoints[t+1] = bunnyVertices[y];
            bunnyPoints[t+2] = bunnyVertices[z];
            bunnyColors[t] = object_color;
            bunnyColors[t+1] = object_color;
            bunnyColors[t+2] = object_color;
        }
        myfile.close();
    }
}

//----------------------------------------------------------------------------
void object_popup_handling(int item);
void mode_popup_handling(int item);

void popup_menu(){
    // Create a menu for object type
    int objectTypeMenu = glutCreateMenu(object_popup_handling);
    
    // Add menu items
    glutAddMenuEntry("Cube", Cube);
    glutAddMenuEntry("Sphere", Sphere);
    glutAddMenuEntry("Bunny", Bunny);
    
    // Create a menu for drawing mode
    int drawingModeMenu = glutCreateMenu(mode_popup_handling);
    
    // Add menu items
    glutAddMenuEntry("Wireframe", Wireframe);
    glutAddMenuEntry("Solid", Solid);
    
    // Create a menu for object color
    int colorMenu = glutCreateMenu(color_popup_handling);
    
    // Add menu items
    glutAddMenuEntry("Red", Red);
    glutAddMenuEntry("Green", Green);
    glutAddMenuEntry("Blue", Blue);
    glutAddMenuEntry("Magenta", Magenta);
    glutAddMenuEntry("Cyan", Cyan);
    glutAddMenuEntry("Yellow", Yellow);
    glutAddMenuEntry("White", White);
    glutAddMenuEntry("Black", Black);
    
    glutCreateMenu(NULL);
    glutAddSubMenu("Object Type", objectTypeMenu);
    glutAddSubMenu("Drawing Mode", drawingModeMenu);
    glutAddSubMenu("Color", colorMenu);
    
    // Associate a mouse button with menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
}

// Object selection handling. Sets the current object, bind the array and initializes the buffer of chosen object
// Also sets the position to the initial position.
void object_popup_handling(int item){
    switch (item) {
        case Cube:
            currentCount = cubeNumVertices;
            currentObject = Cube;
            glBindVertexArray( vao[0] );
            initialize_cube_buffer();
            reset();
            break;
        case Sphere:
            currentCount = (sphereNumVertices-1)*3;
            currentObject = Sphere;
            glBindVertexArray( vao[1] );
            initialize_sphere_buffer();
            reset();
            break;
        case Bunny:
            currentCount = bunnyPointNum;
            currentObject = Bunny;
            glBindVertexArray( vao[2] );
            initialize_bunny_buffer();
            reset();
            break;
        default:
            reset();
            break;
    }
    glutPostRedisplay();
    return;
}

// Drawing mode handling, sets the drawing_mode to corresponding macro
void mode_popup_handling(int item){
    switch (item) {
        case Wireframe:
            drawing_mode = GL_LINES;
            break;
        case Solid:
            drawing_mode = GL_TRIANGLES;
            break;
        default:
            break;
    }
    glutPostRedisplay();
    return;
}

// Color selection handling, sets the object_color to chosen color RBGA.
void color_popup_handling(int item){
    switch (item) {
        case Red:
            object_color = vec4(1.0,0.0,0.0,0.0);
            break;
        case Green:
            object_color = vec4(0.0,1.0,0.0,0.0);
            break;
        case Blue:
            object_color = vec4(0.0,0.0,1.0,0.0);
            break;
        case Magenta:
            object_color = vec4(1.0,0.0,1.0,0.0);
            break;
        case Cyan:
            object_color = vec4(0.0,1.0,1.0,0.0);
            break;
        case Yellow:
            object_color = vec4(1.0,1.0,0.0,0.0);
            break;
        case White:
            object_color = vec4(1.0,1.0,1.0,0.0);
            break;
        case Black:
            object_color = vec4(0.0,0.0,0.0,0.0);
            break;
        default:
            break;
    }
    change_color();
    glutPostRedisplay();
    return;
}

//---------------------------------------------------------------------
//
// reshape
//

void reshape( int w, int h )
{
    
    glViewport( 0, 0, w, h );
    
    // Set projection matrix
    mat4  projection;
    projection = Ortho(-51.2, 51.2, 0.0, 48.0, -10.0, 10.0);
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}


//----------------------------------------------------------------------------
float up = -1.0;
float diff = 5;
void
idle( void )
{
    // flag for stopping/starting animation
    if (!stop)
    {
        if (ceeling > 0)
        {
            // horizontal movement
            a += 0.50;
            
            // randomly chosen proportion to increase/decrease speed.
            float portion =ceeling/5;
            if (b < portion)
            {
                b+=5*up;
                diff=5;
            }
            else if (b < 2*portion)
            {
                b+=4*up;
            }
            else if (b < 3*portion)
            {
                b+=3*up;
            }
            else if (b < 4*portion)
            {
                b+=2*up;
            }
            else
            {
                b+=1*up;
                diff = 3;
            }
        }
        else
        {
            b=0;
        }
        
        // Edges control
        if (a > -a_init || a < a_init)
        {
            a = -a;
        }
        
        // top ceeling cpntrol, change direction
        if (b > ceeling)
        {
            up *= -1.0;
        }
        
        // ground control, change direction and lower ceeling
        if(b <= 0)
        {
            ceeling = ceeling-diff;
            up *= -1.0;
        }
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE | GLUT_3_2_CORE_PROFILE );
    glutInitWindowSize( original_x, original_y );
    
    glutCreateWindow( "Bouncing Ball" );
    popup_menu();
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glewExperimental = GL_TRUE;
    glewInit();
    
    init();
    
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );
    glutReshapeFunc( reshape );
    glutMainLoop();
    return 0;
}

