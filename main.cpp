#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/glu.h>     // Header File For The GLu32 Library

#include <GL/gl.h>
#include <SOIL/SOIL.h>

#include "ClFunctions.h"


#define WIN_SIZE_W 800
#define WIN_SIZE_H 600

#define MAP_SIZE 64

# define GLUT_WHEEL_UP 3
# define GLUT_WHEEL_DOWN 4

//declaration
GLuint LoadGLTexture( const char * filename );
GLuint LoadTexture(const char * filename);

//Static data
GLuint texture;

/*Data for camera position*/
float deltaAngle = 0.0f;
float gamaAngle = 0.0f;
int xOrigin = -1;
int yOrigin = -1;
int yOriginStraight = -1;
int xOriginStraight = -1;
float stepY = 0.0f;
float stepX = 0.0f;
// angle of rotation for the camera direction
float angle=0.0;
float angle2=0.0;
// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f,ly=0.0f,lz2=-1.0f;
// XZ position of the camera
float x=0.0f,z=5.0f, y=0.0f;
//starting z position
float tz = -8.0f;


//height matrix
float **height;
//window number
int window;

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)              // We call this right after our OpenGL window is created.
{
    glEnable(GL_TEXTURE_2D);                        // Zapne mapování textur
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);         // This Will Clear The Background Color To Black
    glClearDepth(1.0);                            // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LEQUAL);                         // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);                      // Enables Depth Testing
    glShadeModel(GL_SMOOTH);                      // Enables Smooth Color Shading

    texture = LoadTexture("./grassTexture.bmp");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                             // Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);     // Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);
}

GLuint LoadTexture( const char * filename )
{
    GLuint texture;

    int width, height;

    unsigned char * data;

    FILE * file;

    file = fopen( filename, "rb" );

    if ( file == NULL ) return 0;
    width = 1024;
    height = 512;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    for(int i = 0; i < width * height ; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];

        data[index] = R;
        data[index+2] = B;
    }


    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
    free( data );

    return texture;
}

GLuint LoadGLTexture(const char * filename )                                    // Load Bitmaps And Convert To Textures
{
    /* load an image file directly as a new OpenGL texture */
    /*texture = SOIL_load_OGL_texture
        (
        filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
        );*/
 
    if(texture == 0)
        return texture;
 
 
    // Typical Texture Generation Using Data From The Bitmap
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
 
    return texture;                                        // Return Success
}

void  DrawGLScene()// Vykreslování
{
        int HALF_SIZE = MAP_SIZE/2;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Smaže obrazovku a hloubkový buffer
	glLoadIdentity();// Reset matice      

        glTranslatef(stepX,0.0f,0.0f);
        
        glTranslatef(0.0f,stepY,0.0f);
        
        glTranslatef(0.0f,0.0f,tz);
        
        // Set the camera
	gluLookAt(	x, y, z,
			x+lx, y+ly,  z+lz,
			0.0f, 1.0f,  0.0f);
        
        //glBindTexture(GL_TEXTURE_2D, texture);// Zvolí texturu
       
        
	// Sem kreslit
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (int ix = 0; ix < MAP_SIZE-1; ix++) {
            for (int iz = 0; iz < MAP_SIZE-1; iz++) {
                //glColor3f(0.2f,1.0f,0.2f);		
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f( (float)ix-HALF_SIZE, height[ix][iz]    , (float)iz-HALF_SIZE );
                    
                    glTexCoord2f(0.3f, 0.0f);
                    glVertex3f( (float)(ix+1)-HALF_SIZE, height[(ix+1)][iz]  , (float)iz-HALF_SIZE );
                    
                    glTexCoord2f(0.3f, 0.3f);
                    glVertex3f( (float)(ix+1)-HALF_SIZE, height[(ix+1)][(iz+1)],(float)(iz + 1)-HALF_SIZE);
                    
                    glTexCoord2f(0.0f, 0.3f);
                    glVertex3f( (float)ix-HALF_SIZE, height[ix][(iz+1)]  ,(float) (iz + 1)-HALF_SIZE);
                glEnd();
            }
        }
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPopMatrix();

    // we need to swap the buffer to display our drawing.
    glutSwapBuffers();
}


void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			angle += deltaAngle;
            angle2 += gamaAngle;
			xOrigin = -1;
            yOrigin = -1;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
            yOrigin = y;
		}
	}
        
        if (button == GLUT_RIGHT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			yOriginStraight = -1;
            xOriginStraight = -1;
		}
		else  {// state = GLUT_DOWN
			yOriginStraight = y;
            xOriginStraight = x;
		}
	}
        
        if(button == GLUT_WHEEL_UP) {
            tz += 1;
        }
        
        if(button == GLUT_WHEEL_DOWN) {
            tz -= 1;
        }
}

void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		// update deltaAngle
		deltaAngle = (x - xOrigin) * 0.002f;
        gamaAngle = (y - yOrigin) * 0.002f;

		// update camera's direction
		lx = sin(angle + deltaAngle);
        ly = sin(angle2 + gamaAngle);
		lz = -cos(angle + deltaAngle);
        lz2 = -cos(angle2 + gamaAngle);
	}
        
        // this will only be true when the left button is down
	if (xOriginStraight >= 0) {

		// update deltaAngle
		stepX -= (x-xOriginStraight) * 0.002f;
        stepY += (y-yOriginStraight) * 0.002f;
//                oldY = yOriginStraight;
//                oldX = xOriginStraight;
	}
}

void cpuFaultAlgorithm(float **matrix, int w, int l, int iterationCount)
{
    float v;
    float a,b,d,c;
    float displacement = 0.25;
    
    for(int it = 0; it < iterationCount; it++){
        //one iteration
        v = rand();
        a = sin(v);
        b = cos(v);
        d = sqrt((float)w*w + l*l);
        // rand() / RAND_MAX gives a random number between 0 and 1.
        // therefore c will be a random number between -d/2 and d/2
        c = ((float)rand() / (float)RAND_MAX) * d - d/2;

        //change height in whole map
        for(int iz = 0; iz < l; iz++)
            {
            for(int ix = 0; ix < w; ix++)
            {
                if(a*ix + b*iz - c > 0)
                    matrix[iz][ix] +=  displacement;
                else
                    matrix[iz][ix] -=  displacement;
            }
        }
    }
}

void gpuFaultAlgorithm(float **matrix, int w, int l, int iterationCount)
{
	float v = 0.0f;
	float a = 0.0f, b = 0.0f, d = 0.0f, c = 0.0f;
	float displacement = 0.25;
    

	faultFormationCl(a, b, c, displacement, w, l, matrix, iterationCount);

}


void createHeightMap(float ***height, int width, int length){
    (*height) = (float **)malloc(length*sizeof(float *));
    
    for(int i = 0; i < length; i++)
    {
        (*height)[i] = (float *)malloc(width*sizeof(float));
		memset((*height)[i], 0, width*sizeof(float));
    }


}

void freeHeight(float ***height, int length){
    for(int i = 0; i < length; i++)
    {
        free((*height)[i]);
    }
    free(*height);
}

int main(int argc, char **argv) {
    
    createHeightMap(&height, MAP_SIZE, MAP_SIZE);
    gpuFaultAlgorithm(height, MAP_SIZE, MAP_SIZE, 512);
    
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or 
    X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */
    glutInit(&argc, argv);

    /* Select type of Display mode:   
       Double buffer 
          RGBA color
       Alpha components supported 
       Depth buffer */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

    /* get a  window */
    glutInitWindowSize(WIN_SIZE_W, WIN_SIZE_H);

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);

    /* Open a window */
    window = glutCreateWindow("Generace terenu");

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene);

    /* Go fullscreen.  This is as soon as possible. */
    //glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(&DrawGLScene);

    /* Register the function called when our window is resized. */
    //glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    //glutKeyboardFunc(&keyPressed);

    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);

    /* Initialize our window. */
    InitGL(WIN_SIZE_W, WIN_SIZE_H);

    /* Start Event Processing Engine */
    glutMainLoop();

    //freeHeight(&height, MAP_SIZE  );
    return 1;
}
