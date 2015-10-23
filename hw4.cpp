// HW4 for CS 637
// Shangqi Wu

#include "Angel.h"
#include <iostream>

using namespace std;

typedef vec4 color4;

// Height and width of main window. 
const int h = 500;
const int w = 500;

// Numbers of points for different objects. 
const int CubePoints = 8;
const int CubeTrianglesPoints = 36;

// RGBA color for background of main window.
float Red = 0;
float Green = 0;
float Blue = 0;
float Alpha = 1;

// Specify transformation modes:
// 0 for disabling all transformations
// 1 for scaling transformation
// 2 for rotation transformation
// 3 for translation transformation
int mode = 0;

// Default scaling, rotation, translation vectors for default view.
vec3 scale = vec3( 1, 1, 1 );
vec3 rotate = vec3( 0, 0, 0 );
vec3 translate = vec3( 0, 0, 0 );

// Default delta for transformations.
// ds for scaling, dt for transformation, dr for rotation
float ds = 0.05;
float dt = 0.05;
float dr = 5;

// IDs for main window. 
int MainWindow;

// Specify vertices & colors for the cube. 
vec4 cube_ver[CubePoints] = {
    vec4( -0.5, -0.5,  0.5, 1 ),
    vec4( -0.5,  0.5,  0.5, 1 ),
    vec4(  0.5,  0.5,  0.5, 1 ),
    vec4(  0.5, -0.5,  0.5, 1 ),
    vec4( -0.5, -0.5, -0.5, 1 ),
    vec4( -0.5,  0.5, -0.5, 1 ),
    vec4(  0.5,  0.5, -0.5, 1 ),
    vec4(  0.5, -0.5, -0.5, 1 )
};

color4 cube_col[CubePoints] = {
    color4( 0.0, 0.0, 0.0, 1.0 ), // black
    color4( 1.0, 0.0, 0.0, 1.0 ), // red
    color4( 1.0, 1.0, 0.0, 1.0 ), // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ), // green
    color4( 0.0, 0.0, 1.0, 1.0 ), // blue
    color4( 1.0, 0.0, 1.0, 1.0 ), // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ), // white
    color4( 0.0, 1.0, 1.0, 1.0 )  // cyan
};

// ID for shaders program.
GLuint program;

//--------------------------------------------------------------------------

void
init( void )
{
    // Reserve vertices & colors for squares. 
    vec4 vertices[CubeTrianglesPoints];
    color4 colors[CubeTrianglesPoints];
    
    // Create a vertex array object.
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    cout<<"glGenVertexArrays(), glBindVertexArray() for main window initialization."<<endl;

    // Create and initialize a buffer object.
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    cout<<"glGenBuffer(), glBindBuffer() for main window initialization."<<endl;
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors), NULL, GL_STATIC_DRAW );
    cout<<"glBufferData(), glBufferSubData() for main window initialization."<<endl;

    // Load shaders and use the resulting shader program.
    program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );
    cout<<"InitShader(), glUseProgram() for main window initialization."<<endl;

    // Initialize the vertex position attribute from the vertex shader.
    GLuint loc_ver = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc_ver );
    glVertexAttribPointer( loc_ver, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint loc_col = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( loc_col );
    glVertexAttribPointer( loc_col, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(vertices)) );
    cout<<"glEnableVertexAttribArray(), glVertexAttribPointer() for main window initialization."<<endl;

    // Passing matrix to shader. 
    mat4 m = identity();
    GLint loc_trans = glGetUniformLocation( program, "trans" );
    glUniformMatrix4fv( loc_trans, 1, true, m );
    cout<<"glGetUniformLocation(), glUniformMatrix4fv() for main window initialization."<<endl;
}

//----------------------------------------------------------------------------

void
quad( int a, int b, int c, int d, vec4 points[], color4 colors[], int *Index )
{
    colors[*Index] = cube_col[a]; points[*Index] = cube_ver[a]; *Index = *Index + 1;
    colors[*Index] = cube_col[b]; points[*Index] = cube_ver[b]; *Index = *Index + 1;
    colors[*Index] = cube_col[c]; points[*Index] = cube_ver[c]; *Index = *Index + 1;
    colors[*Index] = cube_col[a]; points[*Index] = cube_ver[a]; *Index = *Index + 1;
    colors[*Index] = cube_col[c]; points[*Index] = cube_ver[c]; *Index = *Index + 1;
    colors[*Index] = cube_col[d]; points[*Index] = cube_ver[d]; *Index = *Index + 1;
}

//----------------------------------------------------------------------------

void
recal( void )
{
    // Vertices & colors for converting cube into triangles. 
    vec4 vertices[CubeTrianglesPoints];
    color4 colors[CubeTrianglesPoints];

    // Function to convert cube points into triangles.
    int Index = 0;
    quad( 1, 0, 3, 2, vertices, colors, &Index );
    quad( 2, 3, 7, 6, vertices, colors, &Index );
    quad( 3, 0, 4, 7, vertices, colors, &Index );
    quad( 6, 5, 1, 2, vertices, colors, &Index );
    quad( 4, 5, 6, 7, vertices, colors, &Index );
    quad( 5, 4, 0, 1, vertices, colors, &Index );

    // Create a transformation matrix.
    mat4 m = identity();
    // Calculate matrix by scaling, rotation, and translation vectors. 
    m *= ( Translate( translate )*RotateX( rotate.x )*RotateY( rotate.y )*RotateZ( rotate.z )*Scale( scale ) );

    // Passing vertices & colors data to opengl buffer object.
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors );
    cout<<"glBufferSubData() for vertices calculation."<<endl;
    
    // Pass tranformation matrix to vertex shader. 
    GLint loc_trans = glGetUniformLocation( program, "trans" );
    glUniformMatrix4fv( loc_trans, 1, true, m );
    cout<<"glGetUniformLocation(), glUniformMatrix4fv() for transformation matrix."<<endl;
}

//----------------------------------------------------------------------------

void
display( void )
{
    recal(); // Calculates vertices & colors for objects in main window. 
    glClearColor( Red, Green, Blue, Alpha ); // Set background color of main window.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Clear main window.
    glMatrixMode( GL_MODELVIEW ); // Set up model view.
    glDrawArrays( GL_TRIANGLES, 0, CubeTrianglesPoints ); // Draw the points by one square.
    glutSwapBuffers(); // Double buffer swapping. 
    glFlush(); // Flush. 
    cout<<"glClearColor(), glClear(), glDrawArrays(), glutSwapBuffers(), glFlush() for main window display function."<<endl;
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
        case 033: exit( EXIT_SUCCESS ); break; // "Esc": exit the program.
        case (int)'r': scale = vec3( 1, 1, 1 ); rotate = vec3( 0, 0, 0 ); translate = vec3( 0, 0, 0 ); ds = 0.05; dt = 0.05; dr = 5; break; // Rest.
        case (int)'c': // Increasing x.
            if (mode == 1) scale = vec3( scale.x + ds, scale.y, scale.z ); 
            else if (mode == 2) rotate = vec3( rotate.x + dr, rotate.y, rotate.z );
            else if (mode == 3) translate = vec3( translate.x + dt, translate.y, translate.z );
            break;
        case (int)'x': // Decreasing x.
            if (mode == 1) scale = vec3( scale.x - ds, scale.y, scale.z ); 
            else if (mode == 2) rotate = vec3( rotate.x - dr, rotate.y, rotate.z );
            else if (mode == 3) translate = vec3( translate.x - dt, translate.y, translate.z );
            break;
        case (int)'y': // Incresing y.
            if (mode == 1) scale = vec3( scale.x, scale.y + ds, scale.z );
            else if (mode == 2) rotate = vec3( rotate.x, rotate.y + dr, rotate.z );
            else if (mode == 3) translate = vec3( translate.x, translate.y + dt, translate.z );
            break;
        case (int)'h': // Decreasing y.
            if (mode == 1) scale = vec3( scale.x, scale.y - ds, scale.z );
            else if (mode == 2) rotate = vec3( rotate.x, rotate.y - dr, rotate.z );
            else if (mode == 3) translate = vec3( translate.x, translate.y - dt, translate.z );
            break;
        case (int)'z': // Increasing z.
            if (mode == 1) scale = vec3( scale.x, scale.y, scale.z + ds );
            else if (mode == 2) rotate = vec3( rotate.x, rotate.y, rotate.z + dr );
            else if (mode == 3) translate = vec3( translate.x, translate.y, translate.z + dt );
            break;
        case (int)'a': // Decreasing z.
            if (mode == 1) scale = vec3( scale.x, scale.y, scale.z - ds );
            else if (mode == 2) rotate = vec3( rotate.x, rotate.y, rotate.z - dr );
            else if (mode == 3) translate = vec3( translate.x, translate.y, translate.z - dt );
            break;
        case (int)'d': // Increasing delta.
            if (mode == 1) ds += 0.05; 
            else if (mode == 2) dr += 5; 
            else if (mode == 3) dt += 0.05; 
            break; 
        case (int)'f': // Decreasing delta.
            if (mode == 1) ds -= 0.05; 
            else if (mode == 2) dr -= 5; 
            else if (mode == 3) dt -= 0.05; 
            break; 
    }
    glutPostRedisplay();
    cout<<"glutPostRedisplay() for keyboard function."<<endl;
}

//----------------------------------------------------------------------------

void
MainWindowMenu( int id )
{
    switch ( id ) {
        case 1: 
            mode = 1; 
            cout<<"----------------------------"<<endl<<"Current transformation is scaling."<<endl<<"----------------------------"<<endl;
            break;
        case 2: 
            mode = 2; 
            cout<<"----------------------------"<<endl<<"Current transformation is rotating."<<endl<<"----------------------------"<<endl;
            break;
        case 3: 
            mode = 3; 
            cout<<"----------------------------"<<endl<<"Current transformation is translating."<<endl<<"----------------------------"<<endl;
            break;
        case 4:
            mode = 0;
            cout<<"----------------------------"<<endl<<"All transformations are disabled."<<endl<<"----------------------------"<<endl;
            break;
    }
}

//----------------------------------------------------------------------------

void
setMainWinMenu( void )
{
    int menu_id;

    menu_id = glutCreateMenu( MainWindowMenu ); // Set menu in main window. 
    cout<<"glutCreateMenu() for main window menu."<<endl;
    glutAddMenuEntry( "Scale", 1 );
    glutAddMenuEntry( "Rotate", 2 );
    glutAddMenuEntry( "Translate", 3 );
    glutAddMenuEntry( "Disable all transformation", 4 );
    cout<<"glutAddMenuEntry() for main window menu."<<endl;
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    cout<<"glutAttachMenu() for main window menu."<<endl;
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{   
    glutInit( &argc, argv ); // Initializing environment.
    cout<<"glutInit(&argc,argv) called."<<endl;
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    cout<<"glutInitDisplayMode() called."<<endl;
    glutInitWindowSize( w, h );
    cout<<"glutInitWindowSize() called."<<endl;
    glutInitWindowPosition( 100, 100 );
    cout<<"glutInitWindowPosition() called."<<endl;

    MainWindow = glutCreateWindow( "ICG_hw4" ); // Initializing & setting main window.
    cout<<"glutCreateWindow() for main window."<<endl;
    glewExperimental=GL_TRUE; 
    glewInit();    
    cout<<"glewInit() for main window."<<endl;
    init(); // Initializing VAOs & VBOs. 
    glutDisplayFunc( display ); // Setting display function for main window.
    cout<<"glutDisplayFunc() for main window."<<endl;
    glutKeyboardFunc( keyboard ); // Setting keyboard function for main window.
    cout<<"glutKeyboardFunc() for main window."<<endl;
    setMainWinMenu(); // Setting menu for main window. 

    glEnable( GL_DEPTH_TEST );
    cout<<"glEnable( GL_DEPTH_TEST ) called."<<endl;
    cout<<"glutMainLoop() called."<<endl;
    cout<<endl<<"----------------------------"<<endl<<"Instructions:"<<endl<<"'x' is for decreasing x part in a tranformation vector,"<<endl<<"'c' is for increasing x part in a tranformation vector,"<<endl<<"'h' is for decreasing y part in a tranformation vector,"<<endl<<"'y' is for increasing y part in a tranformation vector,"<<endl<<"'a' is for decreasing z part in a tranformation vector,"<<endl<<"'z' is for increasing z part in a tranformation vector,"<<endl<<"'d' is for increasing delta for transformation,"<<endl<<"'f' is for decreasing delta for transformation."<<endl<<"----------------------------"<<endl<<endl;

    glutMainLoop(); // Start main loop. 

    return 0;
}
