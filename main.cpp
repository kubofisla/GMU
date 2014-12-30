#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>     
#include <GL/glu.h>     
#include <GL/gl.h>

#ifdef USE_SOIL
#include <SOIL/SOIL.h>
#endif

#include "ClFunctions.h"

#define WIN_SIZE_W 800
#define WIN_SIZE_H 600

#define MAP_SIZE 512
#define ITERATIONS 4096

#define STEP 1
#define DISPLACEMENT 0.20

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
float x=0.0f,z=6.0f, y=10.0f;
//starting z position
float tz = -8.0f;

int mapsize = MAP_SIZE;
int iterations = ITERATIONS;


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

	#ifdef USE_SOIL
	texture = LoadGLTexture("./grassTexture.bmp");
	#endif // USE_SOIL


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

#ifdef USE_SOIL
GLuint LoadGLTexture(const char * filename )                                    // Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	texture = SOIL_load_OGL_texture
		(
		filename,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

	if (texture == 0)
		return texture;


	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;                                        // Return Success
}
#endif

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
        
	#ifdef USE_SOIL
		glBindTexture(GL_TEXTURE_2D, texture);// Zvolí texturu
	#else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	#endif // USE_SOIL
        
	// Sem kreslit
	glPushMatrix();
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
		#ifndef USE_SOIL
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		#endif
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

void mouseMove(int mx, int my) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		// update deltaAngle
		deltaAngle = (mx - xOrigin) * 0.002f;
        gamaAngle = (my - yOrigin) * 0.002f;

		// update camera's direction
		lx = sin(angle + deltaAngle);
        ly = sin(angle2 + gamaAngle);
		lz = -cos(angle + deltaAngle);
        lz2 = -cos(angle2 + gamaAngle);
	}
        
        // this will only be true when the left button is down
	if (xOriginStraight >= 0) {

		// update deltaAngle
		stepX -= (mx-xOriginStraight) * 0.002f;
        stepY += (my-yOriginStraight) * 0.002f;
	}
}

float Cosine_Interpolate(float a, float b, float x)
{
	float ft = x * 3.1415927;
	float f = (1.0f - cos(ft)) * .5;

	return  a*(1.0f - f) + b*f;
}

void cpuFaultAlgorithm(float **matrix, int w, int l, int iterationCount)
{
    float v;
    float a,b,d,c;
	float displacement = DISPLACEMENT;
    
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
				if (a*ix + b*iz - c > 2.0f)
                    matrix[iz][ix] += displacement;
				else if (a*ix + b*iz - c < -2.0f)
                    matrix[iz][ix] -= displacement;
				else
					matrix[iz][ix] += Cosine_Interpolate(-displacement, displacement, (a*ix + b*iz - c + 2) / 4.0);;
            }
        }
    }
}

//http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
float PseudoRandom_NoiseI(int x, int y)
{
	int n = (int)x + (int)y * 57;
	n = (n << 13) ^ n;
	int nn = (n*(n*n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0 - ((double)nn / 1073741824.0);
}

float Smooth_NoiseI(float x, float y)
{
	float corners = (PseudoRandom_NoiseI(x - 1, y - 1) + PseudoRandom_NoiseI(x + 1, y - 1) + PseudoRandom_NoiseI(x - 1, y + 1) + PseudoRandom_NoiseI(x + 1, y + 1)) / 16;
	float sides = (PseudoRandom_NoiseI(x - 1, y) + PseudoRandom_NoiseI(x + 1, y) + PseudoRandom_NoiseI(x, y - 1) + PseudoRandom_NoiseI(x, y + 1)) / 8;
	float center = PseudoRandom_NoiseI(x, y) / 4;
	return corners + sides + center;
}

float Interpolated_NoiseI(float x, float y, int freq){
	int integer_X = int(x);
	float fractional_X = x - integer_X;

	int integer_Y = int(y);
	float fractional_Y = y - integer_Y;

	float v1 = Smooth_NoiseI(integer_X, integer_Y);
	float v2 = Smooth_NoiseI(integer_X + 1, integer_Y);
	float v3 = Smooth_NoiseI(integer_X, integer_Y + 1);
	float v4 = Smooth_NoiseI(integer_X + 1, integer_Y + 1);

	float i1 = Cosine_Interpolate(v1, v2, fractional_X);
	float i2 = Cosine_Interpolate(v3, v4, fractional_X);

	return Cosine_Interpolate(i1, i2, fractional_Y);
}

float point_PerlinNoise(float x, float y, float persistence, int octaves){
	float total = 0.0f;
	float amplitude = 2.2f;

	for (int i = 0; i < octaves; i++)
	{
		float frequency = pow(2.0f, i);
		amplitude *= persistence;

		total = total + Interpolated_NoiseI(x/frequency, y/frequency, frequency) * amplitude;

	}

	return total;
}

void cpu_PerlinNoise(float **matrix, int w, int l, float persistence, int octaves){
	for (int iz = 0; iz < l; iz++)
	{
		for (int ix = 0; ix < w; ix++)
		{
			matrix[iz][ix] = point_PerlinNoise(iz, ix, persistence, octaves);
		}
	}
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

void Keyboard(unsigned char key, int x, int y)
{
	float dtStart, dtStart2, dtStart3, dtStart4;
	double dt, dt2, dt3, dt4;

	switch (key)
	{
	case 27:             // ESCAPE key
		exit(0);
		break;

	case '+':
		tz += 1;
		break;

	case '-':
		tz -= 1;
		break;

	case 's':
		dtStart = GetTime();
		cpuFaultAlgorithm(height, mapsize, mapsize, iterations);
		dt = GetTime();
		printf("all CPUFaultAlgorithm time: %f\n", dt - dtStart);
		break;

	case 'x':
		dtStart2 = GetTime();
		faultFormationCl(mapsize, mapsize, height, iterations, DISPLACEMENT);
		dt2 = GetTime();
		printf("all GPUFaultAlgorithm time: %f\n", dt2 - dtStart2);
		break;

	case 'd':
		dtStart3 = GetTime();
		cpu_PerlinNoise(height, mapsize, mapsize, 0.75, 8);
		dt3 = GetTime();
		printf("all CPUPerlinNoise time: %f\n", dt3 - dtStart3);
		break;

	case 'c':
		dtStart4 = GetTime();
		perlinNoiseCl(0.75, 8, mapsize, mapsize, height);
		dt4 = GetTime();
		printf("all GPUPerlinNoise time: %f\n", dt4 - dtStart4);
		break;
	}
}

int main(int argc, char **argv) {
	if (argc == 2)
	{
		mapsize = atoi(argv[1]);
		iterations = atoi(argv[2]);
	}

	createHeightMap(&height, mapsize, mapsize);
	
	double dtStart3 = GetTime();
	cpu_PerlinNoise(height, mapsize, mapsize, 0.75, 8);
	double dt3 = GetTime();
	printf("all CPUPerlinNoise time: %f\n", dt3 - dtStart3);

	double dtStart4 = GetTime();
	perlinNoiseCl(0.75, 8, mapsize, mapsize, height);
	double dt4 = GetTime();
	printf("all GPUPerlinNoise time: %f\n", dt4 - dtStart4);
    
	double dtStart = GetTime();
	cpuFaultAlgorithm(height, mapsize, mapsize, iterations);
	double dt = GetTime();
	printf("all CPUFaultAlgorithm time: %f\n", dt - dtStart);
	
	double dtStart2 = GetTime();
	faultFormationCl(mapsize, mapsize, height, iterations, DISPLACEMENT);
	double dt2 = GetTime();
	printf("all GPUFaultAlgorithm time: %f\n",dt2 - dtStart2);


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
	glutKeyboardFunc(&Keyboard);

    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);

    /* Initialize our window. */
    InitGL(WIN_SIZE_W, WIN_SIZE_H);

    /* Start Event Processing Engine */
    glutMainLoop();

    //freeHeight(&height, MAP_SIZE  );
    return 1;
}
