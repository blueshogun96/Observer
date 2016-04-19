#include "GLView.h"
#include "opencv_drv.h"
#include "tgaLoader.h"
#include "tmath.h"
#include "platform.h"

#include <GLUT/GLUT.h>



// Globals
// Data would normally be read from files
GLfloat vertices[] =
{
    -1, -1,  1,
    -1,  1,  1,
     1,  1,  1,
     1, -1,  1,
    
    -1, -1, -1,
    -1,  1, -1,
     1,  1, -1,
     1, -1, -1,
    
    -1, -1, -1,
    -1,  1, -1,
    -1,  1,  1,
    -1, -1,  1,
    
     1, -1, -1,
     1,  1, -1,
     1,  1,  1,
     1, -1,  1,
};

struct Vector3 target_vector = { 0, 0, 0 };
int target_test_mode = Yes;

void checkError(const char *functionName);


// Copied and pasted from http://www.lighthouse3d.com/cg-topics/code-samples/opengl-3-3-glsl-1-5-sample/
// because I was too lasy to write all this myself. Plus I was in a hurry (as usual).

float projMatrix[16];
float modelViewMatrix[16];

GLuint g_MVPMatrix = 0;

GLuint tex[2];

// ----------------------------------------------------
// VECTOR STUFF
//

// res = a cross b;
void crossProduct( float *a, float *b, float *res) {
    
    res[0] = a[1] * b[2]  -  b[1] * a[2];
    res[1] = a[2] * b[0]  -  b[2] * a[0];
    res[2] = a[0] * b[1]  -  b[0] * a[1];
}

// Normalize a vec3
/*void normalize(float *a) {
    
    float mag = sqrt(a[0] * a[0]  +  a[1] * a[1]  +  a[2] * a[2]);
    
    a[0] /= mag;
    a[1] /= mag;
    a[2] /= mag;
}*/

// ----------------------------------------------------
// MATRIX STUFF
//

// sets the square matrix mat to the identity matrix,
// size refers to the number of rows (or columns)
void setIdentityMatrix( float *mat, int size) {
    
    // fill matrix with 0s
    for (int i = 0; i < size * size; ++i)
        mat[i] = 0.0f;
    
    // fill diagonal with 1s
    for (int i = 0; i < size; ++i)
        mat[i + i * size] = 1.0f;
}

//
// a = a * b;
//
void multMatrix(float *a, float *b) {
    
    float res[16];
    
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res[j*4 + i] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                res[j*4 + i] += a[k*4 + i] * b[j*4 + k];
            }
        }
    }
    memcpy(a, res, 16 * sizeof(float));
    
}

// Defines a transformation matrix mat with a translation
void setTranslationMatrix(float *mat, float x, float y, float z) {
    
    setIdentityMatrix(mat,4);
    mat[12] = x;
    mat[13] = y;
    mat[14] = z;
}

// ----------------------------------------------------
// Projection Matrix
//

/*void buildProjectionMatrix(float fov, float ratio, float nearP, float farP) {
    
    float f = 1.0f / tan (fov * (M_PI / 360.0));
    
    setIdentityMatrix(projMatrix,4);
    
    projMatrix[0] = f / ratio;
    projMatrix[1 * 4 + 1] = f;
    projMatrix[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
    projMatrix[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
    projMatrix[2 * 4 + 3] = -1.0f;
    projMatrix[3 * 4 + 3] = 0.0f;
}
*/

void __gluMakeIdentityf(GLfloat m[16])
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void glOrthof(float left, float right,float bottom, float top,float near, float far)
{
    
    float a = 2.0f / (right - left);
    float b = 2.0f / (top - bottom);
    float c = -2.0f / (far - near);
    
    float tx = - (right + left)/(right - left);
    float ty = - (top + bottom)/(top - bottom);
    float tz = - (far + near)/(far - near);
    
    float ortho[16] = {
        a, 0, 0, 0,
        0, b, 0, 0,
        0, 0, c, 0,
        tx, ty, tz, 1
    };
    
 //   glMultMatrixf( ortho );
    memcpy( projMatrix, ortho, sizeof( float ) * 4 );
}


// Note: free data afterwards.
char* loadFile(char *filename, GLint *fSize)
{
	char * data;
	FILE *theFile;
	char c;
	long howMuch;
    
	// Get file length
	theFile = fopen(filename, "rb");
	if (theFile == NULL)
	{
		printf("%s not found\n", filename);
		return NULL;
	}
	howMuch = 0;
	c = 0;
	while (c != EOF)
	{
		c = getc(theFile);
		howMuch++;
	}
	fclose(theFile);
    
	printf("%ld bytes\n", howMuch);
    
	// Read it again
	data = (char *)malloc(howMuch);
	theFile = fopen(filename, "rb");
	fread(data, howMuch-1,1,theFile);
	fclose(theFile);
	data[howMuch-1] = 0;
    
	printf("\n--- Shader source %s ---\n%s\n-- end of %s --\n", filename, data, filename);
	printf("%s loaded from disk\n", filename);
	*fSize = howMuch;
    
	return data;
}

void dumpInfo(void)
{
    printf ("Vendor: %s\n", glGetString (GL_VENDOR));
    printf ("Renderer: %s\n", glGetString (GL_RENDERER));
    printf ("Version: %s\n", glGetString (GL_VERSION));
    printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
    checkError ("dumpInfo");
}

/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
    GLenum error;
    while (( error = glGetError() ) != GL_NO_ERROR)
    {
        fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
    }
}

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader)
{
	GLint infoLogLen = 0;
	GLsizei charsWritten = 0;
	GLchar *infoLog;
    
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
    
	// should additionally check for OpenGL errors here
    
	if (infoLogLen > 0)
	{
		infoLog = (GLchar *)malloc(infoLogLen); //new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
		printf("Infolog: %s\n", infoLog);
		free(infoLog);
	}
	else
		printf("No infolog\n");
    
	// should additionally check for OpenGL errors here
}

//-----------------------------------------------------------------------------
// Name: resize_viewport
// Desc: Changes the viewport and projection matrices.
//-----------------------------------------------------------------------------
void resize_viewport( GLsizei width, GLsizei height )
{
	// Prevent divide by zero exception
	if( !height ) height = 1;
    
	// Reset viewport
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    
	// Reset perspective
	gluPerspective( 45.0f, (GLdouble) width/height, 0.1f, 5000.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

//--------------------------------------------------------------------------------------
// Name: glEnable2D
// Desc: Enables 2D rendering via Ortho projection.
//--------------------------------------------------------------------------------------
GLvoid glEnable2D( GLvoid )
{
	GLint iViewport[4];
    
	// Get a copy of the viewport
	glGetIntegerv( GL_VIEWPORT, iViewport );
    
	// Save a copy of the projection matrix so that we can restore it
	// when it's time to do 3D rendering again.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
    
	// Set up the orthographic projection
	glOrtho( iViewport[0], iViewport[0]+iViewport[2],
            iViewport[1]+iViewport[3], iViewport[1], -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
    
	// Make sure depth testing and lighting are disabled for 2D rendering until
	// we are finished rendering in 2D
	glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
}


//--------------------------------------------------------------------------------------
// Name: glDisable2D
// Desc: Disables the ortho projection and returns to the 3D projection.
//--------------------------------------------------------------------------------------
void glDisable2D( GLvoid )
{
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

//--------------------------------------------------------------------------------------
// Name: draw_quad
// Desc: Renders a 2D quad to the screen (using a triangle fan).
//--------------------------------------------------------------------------------------
void draw_quad( GLuint texture, float x, float y, float w, float h )
{
	float vertices[] = { x, y, x+w, y, x+w, y+h, x, y+h };
	float tex[] = { 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f };
    
	glBindTexture( GL_TEXTURE_2D, texture );
    
    //	glEnable2D();
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, tex );
	glVertexPointer( 2, GL_FLOAT, sizeof(float)*2, vertices );
	glDrawArrays( GL_QUADS, 0, 4 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
    //	glDisable2D();
}

//--------------------------------------------------------------------------------------
// Name: create_texture
// Desc: Opens a .tga texture file from disk.
//--------------------------------------------------------------------------------------
GLuint create_texture( char* filename )
{
	GLubyte* pixels;
	GLuint w, h, bpp;
	GLuint handle;
	GLuint pixel_format = GL_RGB;
    
	glGenTextures( 1, &handle );
    
	if( !LoadTgaImage( filename, &pixels, &w, &h, &bpp ) )
	{
		printf( "[ERROR]: Could not load (%s)\n", filename );
        return 0;
	}
    
	if( bpp == 4 )
		pixel_format = GL_RGBA;
    
	glBindTexture( GL_TEXTURE_2D, handle );
	glTexImage2D( GL_TEXTURE_2D, 0, pixel_format, w, h, 0, pixel_format, GL_UNSIGNED_BYTE, pixels );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
	free( pixels );
    
	return handle;
}

//--------------------------------------------------------------------------------------
// Name: transparent_blend
// Desc: Toggles alpha blending for transparency.
//--------------------------------------------------------------------------------------
void transparent_blend( int enable )
{
	if( enable )
	{
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else
	{
		glDisable( GL_BLEND );
	}
}

void draw_target( struct Vector2* v )
{
    glBindTexture( GL_TEXTURE_2D, 0 );
    glColor3f( 0.0f, 0.0f, 1.0f );
    glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, sizeof(float)*3, vertices );
	glDrawArrays( GL_QUADS, 0, 16 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f( 0.0f, 0.0f, 0.0f );
    glLineWidth( 2.0f );
    glDrawArrays( GL_QUADS, 0, 16 );
    glLineWidth( 1.0f );
	glDisableClientState( GL_VERTEX_ARRAY );
    glColor3f( 1.0f, 1.0f, 1.0f );
    
    float mv[16];
    float proj[16];
    int vp[4];
    struct Vector3 vin, vout;
    
    memset( &vin, 0, sizeof( vin ) );
    glGetFloatv( GL_MODELVIEW_MATRIX, mv );
    glGetFloatv( GL_PROJECTION_MATRIX, proj );
    glGetIntegerv( GL_VIEWPORT, vp );
    
    mv[13] = -mv[13];
    vin.y = -vin.y;
    
    project_vertex( &vin, mv, proj, vp, &vout );
    
    /*glEnable2D();
    draw_quad( 0, vout.x-32, vout.y-32, 64, 64 );
    glDisable2D();*/
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    
    if( target_test_mode )
    {
        if( v->x > vout.x-32 && v->x < vout.x+32 &&
            v->y > vout.y-32 && v->y < vout.y+32 )
        {
            float x = ((float) (rand()%16)) - 8.0f;
            float y = ((float) (rand()%12)) - 6.0f;
            
            target_vector.x = x;
            target_vector.y = y;
        }
    }
}

void init(void)
{
	// GL inits
	glClearColor(0.0,0.0,0.0,1);
	glEnable(GL_DEPTH_TEST);
    
    glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
    
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_FOG_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    
	glDisable( GL_LIGHTING );
	glDisable( GL_BLEND );
	glEnable( GL_DITHER );
	glDisable( GL_CULL_FACE );
	glEnable( GL_TEXTURE_2D );
    
    tex[0] = create_texture( "Observer.app/Contents/Resources/Textures/crosshair1.tga" );
    tex[1] = create_texture( "Observer.app/Contents/Resources/Textures/crosshair2.tga" );
    
	// Reset viewport
	resize_viewport( 320*2, 480 );
    
//    glutInit( NULL, NULL );
    
	checkError("GL inits");
    
    checkError("init");
}

void display(void)
{
    static float rot = 0; rot+= 2.0f;
	checkError("pre display");
    
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    struct object_t hand, face;
    opencv_get_face( &face );
    opencv_get_hand( &hand );
    
    struct Vector2 v = _Vector2( hand.x, hand.y );
    
    glTranslatef( target_vector.x, target_vector.y, -20.0f );
    glRotatef( rot, 0, 1, 0 );
    draw_target( &v );

    glEnable2D();
    transparent_blend(1);
    draw_quad( tex[1], face.x-64, face.y-64, 128, 128 );
    draw_quad( tex[0], hand.x-64, hand.y-64, 128, 128 );
    transparent_blend(0);
    glDisable2D();
    
	checkError("display");
}

void reshape(int w, int h)
{
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
}


// Globals (was in GLViewDataPtr)
NSOpenGLContext	*m_context;
float lastWidth, lastHeight;
NSView *theView;

void MakeContext(NSView *view)
{
    //	NSWindow *w;
	NSOpenGLPixelFormat *fmt;
    
	NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFAOpenGLProfile, /*NSOpenGLProfileVersion3_2Core*/ NSOpenGLProfileVersionLegacy,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 32,
		0
	};
    
	// Create custom data pointer
	theView = view;
    
	// Init GL context
	fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: attrs];
    
	m_context = [[NSOpenGLContext alloc] initWithFormat: fmt shareContext: nil];
	[fmt release];
	[m_context makeCurrentContext];
    
    opencv_enable("Observer (OpenCV)");
	checkError("makeCurrentContext");
}



// -------------------- View ------------------------


float loop;

#define Pi 3.1415

@implementation GLView

-(void)drawRect:(NSRect)rect
{
	if (([theView frame].size.width != lastWidth) || ([theView frame].size.height != lastHeight))
	{
		lastWidth = [theView frame].size.width;
		lastHeight = [theView frame].size.height;
        
		// Only needed on resize:
		[m_context clearDrawable];
        
		reshape([theView frame].size.width, [theView frame].size.height);
	}
    
	[m_context setView: theView];
	[m_context makeCurrentContext];
    
	// Draw
	display();
    opencv_update();
    
	[m_context flushBuffer];
	[NSOpenGLContext clearCurrentContext];
    
	loop = loop + 0.1;
}

-(void)windowWillClose:(NSNotification *)note
{
    opencv_disable();
    glDeleteTextures( 2, tex );
    [[NSApplication sharedApplication] terminate:self];
}
@end


// -------------------- Timer ------------------------

NSTimer	*gTimer;
TimerController	*myTimerController;
NSView	*view;

// Timer!
@implementation TimerController
-(void)timerFireMethod:(NSTimer *)t;
{
	[view setNeedsDisplay: YES];
}
@end
