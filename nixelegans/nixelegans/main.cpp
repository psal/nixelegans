/**
 * ** ADAPTED FROM CyberElegans to run on Unix--based platforms, and modified to accept signal input from files
 * main.cpp
 * 
 *
 *
 *  Ported on: Sep 10, 2014
 *      Author: Sean Grimes, Andrew W.E. McDonald
 *  Original Authors of CyberElegans can be found at: <https://github.com/openworm/CyberElegans>
 */
#include <OpenGL/gl.h>	// OpenGL
#include <OPenGL/glu.h>	// OpenGL Utility Library
// Apple does things differently for the GLUT directory, so if Apple
#ifdef __APPLE__
#include <GLUT/glut.h>	// OpenGL Utility Toolkit (For Apple)
#else
#include <GL/glut.h>	// OpenGL Utility Toolkit (For Non Apple)
#endif
#include <cstdlib> 		// exit function


#include "VectorMath.h"	// Header file for Physics2.h??
#include "CPoint.h"
#include "CConnector.h"
#include "CNeuron.h"
#include "CElegans.h"
#include <fstream>
#include <sstream>
#include <fstream> 		// I don't know why the local copy is inlcuded above
#include <iostream>
#include <ctime>
#include "netdb.h"
#include <sys/time.h> // Replace GetTickCount() from WINAPI
#include "sgStringUtils.h" //general utility functions for strings
#include <cstdlib>
#include "errno.h"
#include <iostream>
//#include "pthread.h"
#define MaxPossible_dt 0.009f
#define window_width 1280
#define window_height 800

// Java neuron data/related stuff
std::ifstream neuron_data_stream;
struct NeuronOutput** neuron_outputs;
int num_neuron_outputs;
int num_used_neuron_outputs = 0;
unsigned long start_time;
unsigned long passed_time_windows = 0;


struct NeuronOutput {
    long timestamp; // milliseconds
    std::string name;
};



//Movement trials
//**************************
// angle of rotation for the camera direction
float angle=0.0;
float pan_angle = 0.0;
// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f, ly = 0.0f;
// XZ position of the camera
float x=0.0f,z=0.0f, y=1.0f;
//*****************************


// SEAN --> Worm Size
float scale = 0.50f;



/**
 * This stuff was included in the original main.cpp, but it's linked
 * 		as a framework on Apple.
 *
 * This is here b/c it may be necessary for linux, we'll see.
 *
 * #pragma comment( lib, "opengl32.lib" )
 * #pragma comment( lib, "glu32.lib" )
 * #pragma comment( lib, "glaux.lib" )
 *
 *
 * // Not defined by some compilers, stops errors
 * #ifndef CDS_FULLSCREEN
 * #define CDS_FULLSCREEN 4
 * #endif
 *
 */

// extern int window_height;


/**
 * Quick print to file function for some error logging what's up logging
 */

// From graphics.h
const float neuron_radius = 0.005f;


// Here's out worm

CElegans * worm = new CElegans(0.5, 26);

static struct timeval last_idle_time;

extern Vector3D ort1, ort2, ort3;
GLuint myTexture[1];

//Problems here:
//GL_Window* g_window;

/**
 * Trying to recreate the keyboard update function w/out WINAPI
 * The structure holds 256 true/false values for the keys
 * ***** NOT ALL ARE CONFIGURED IN THE FUNCTION BELOW!! *****
 */
typedef struct{
	bool keyDown[256];
} Keys;

//getting Key object pointers for regular keys
Keys* g_keys; //--> name taken from their code, no idea what g_keys means
//getting Key object pointers for special keys
Keys* specKeys;


// More defines for button input
#define GLUT_LEFT_BUTTON  1
#define GLUT_RIGHT_BUTTON 2
#define GLUT_DOWN 1
#define GLUT_UP   0


/**
 * floats for various points to be displayed
 * Using GLfloat instead of float b/c it's recommended,
 * apparently better cross platform performance
 */
GLfloat rx = 0;
GLfloat ry = 0;
GLfloat rz = 0;

/**
 * Some more declarations for the mouse
 */

unsigned mouse_button = 0;	// --> unsigned necessary?
int mouse_state = 0;		// --> why not bool?
int mx, my;					// --> mouse x, mouse y
bool ldown = false;			// --> no idea
bool rdown = false;			// --> no idea
Vector3D vcenter(0,0,-8);
//Vector3D vcenter(-4,-2,-8);	// --> no idea
Vector3D pos_rc(0,0,0);		// --> no idea



int meet_obstacle = 0;		// --> BROKEN
float direction = 1.0f;		// --> start direction

bool mode = 1;				// --> Show full worm vs neuromuscular only, 	FULL BY DEFAULT
bool mode2 = false;				// --> Physics on and off, 						ON BY DEFAULT
bool mode3 = true;				// --> Send sin pattern directly to muscles, 	ON BY DEFAULT
bool mode4 = false; 			// --> Send sin pattern to ventral motorneurons OFF BY DEFAULT
bool f9 = false;			// --> Not sure what f9 does
bool f1 = true;			// --> Don't show full help menu at start
bool f10 = false;			// --> Not sure what f10 does
bool f11 = false;			// --> Not sure what f11 does
bool f6 = false;				// --> Physics is off at start --> CHANGED TO ON
bool key_a = false;			// --> active selected neuron
bool key_a_prev = false;	// --> no idea
bool key_r = false;			// --> rotate around long axis
bool key_r_prev = false;	// --> no idea


/**
 * build the font, not sure what to do here, WINAPI + OpenGL
 */
//extern GLvoid BuildFont();
//extern GLvoid KillFont();


/**
 * EXTERN FUNCTIONS FROM GRAPHICS.CPP
 */
Vector3D vbeg(0,0,0);
Vector3D vend(0,0,0);
Vector3D ort1(1,0,0),ort2(0,1,0),ort3(0,0,1);


 GLvoid glPrint(GLint x, GLint y, const char *fmt, ...)					// Custom GL "Print" Routine
 {
/*
 float		length=0;								// Used To Find The Length Of The Text
 char		text[256];								// Holds Our String
 va_list		ap;										// Pointer To List Of Arguments
 
 if (fmt == NULL)									// If There's No Text
 return;											// Do Nothing
 
 va_start(ap, fmt);									// Parses The String For Variables
 vsprintf_s(text, fmt, ap);						// And Converts Symbols To Actual Numbers
 va_end(ap);											// Results Are Stored In Text
 
 for (unsigned int loop=0;loop<(strlen(text));loop++)	// Loop To Find Text Length
 {
 length+=gmf[text[loop]].gmfCellIncX;			// Increase Length By Each Characters Width
 }
 
 glTranslatef(x,y,0.0f);					// Center Our Text On The Screen
 
 glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
 glListBase(base);									// Sets The Base Character to 0
 glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
 glPopAttrib();
 */
// Pops The Display List Bits

     char		text[256];								// Holds Our String
     va_list		ap;										// Pointer To List Of Arguments
     
    if (fmt == NULL)									// If There's No Text
        return;											// Do Nothing

    va_start(ap, fmt);									// Parses The String For Variables
    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
    va_end(ap);											// Results Are Stored In Text

    glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
    glDisable(GL_LIGHTING);
    glPushMatrix();												// Store The Projection Matrix
    glLoadIdentity();											// Reset The Projection Matrix
    //glOrtho(0,window.right,0,window.bottom,0,1);				// Set Up An Ortho Screen
    glOrtho(0, 0, 0, 0, 0, 1);
    glTranslatef((GLfloat)x,(GLfloat)(window_height-y),-0.1f);
    glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
    GLuint base = glGenLists(96);
    glListBase(base - 32);								// Sets The Base Character to 32
    glPushMatrix();
    glRasterPos2f(0.1f,0.1f);
    //glTranslatef(x,y,-1.0f);
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
    glPopMatrix();
    glPopAttrib();										// Pops The Display List Bits
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


GLvoid glPrint2(GLdouble x, GLdouble y, GLdouble z, GLubyte r, GLubyte g, GLubyte b, const char *string, ...)		// Where The Printing Happens
{
    
    GLint    viewport[4];
    GLdouble projection[16];
    GLdouble modelview[16];
    GLdouble wx,wy,wz;
    
    //glScalef(scale,scale,scale);
    
    glGetIntegerv(GL_VIEWPORT,viewport);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    //  vx = mouse_x;
    //  vy = window_height - mouse_y - 1;
    //  vz = -1;
    gluProject(x, y, z, modelview, projection, viewport, &wx, &wy, &wz);
    
    glColor3ub(0, 0, 0);
    glPrint((int)wx+1,window_height-(int)wy  ,string);
    glPrint((int)wx-1,window_height-(int)wy  ,string);
    glPrint((int)wx  ,window_height-(int)wy+1,string);
    glPrint((int)wx  ,window_height-(int)wy-1,string);
    
    glColor3ub(r, g, b);
    glPrint((int)wx,window_height-(int)wy,string);
    
    /*	char		text[256];										// Holds Our String
     va_list		ap;												// Pointer To List Of Arguments
     
     if (string == NULL)											// If There's No Text
     return;													// Do Nothing
     
     va_start(ap, string);										// Parses The String For Variables
     vsprintf(text, string, ap);								// And Converts Symbols To Actual Numbers
     va_end(ap);													// Results Are Stored In Text
     
     glPushAttrib(GL_LIST_BIT);	// Position The Text (0,0 - Bottom Left)
     glPushMatrix();												// Store The Modelview Matrix
     glLoadIdentity();											// Reset The Modelview Matrix
     //glTranslated(x,y,z);
     glTranslatef(wx,window_height-wy,-1.f);
     glListBase(base-32);
     glDisable(GL_LIGHTING);
     glRasterPos2f(0.03*scale,0.03*scale);// Choose The Font Set
     glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);			// Draws The Display List Text
     glEnable(GL_LIGHTING);
     glPopMatrix();	
     glPopAttrib();// Restore The Old Projection Matrix
     */
}


/**
 * We need a new keyboard function since the one written is *VERY*
 * specific to windows
 *
 * This function may or may not make use of the bools above, we'll see.
 */
void keyboard(unsigned char key, int mouse_x, int mouse_y){
	switch(key){
            // ESCAPE KEY (NO REPLACEMENT)
        case 27:
            strUtils::appendErrLog("ESC Pressed. Sean: I don't think exit is releasing the memory, let's do something about this!!");
            strUtils::appendErrLog("\n ---------- END OF A RUN ---------- \n");
            strUtils::appendLog("ESC Pressed, shutting down visualization");
            strUtils::appendLog("\n ---------- END OF A RUN ---------- \n");
            exit(0); //Quick and dirt, must fix, memory leaks(I think)
            break; //Really? After a forceful exit??
            // grab the 'a' key press
        case 97:
        case 65:
            if(g_keys->keyDown[65] || g_keys->keyDown[97]){
                strUtils::appendLog("Toggling 'a', no replacement");
                if(key_a_prev){
                    key_a = true;
                }
                key_a_prev = true;
            }
            else{
                strUtils::appendErrLog("Hit else in case 97/65, why?");
                if(!key_a_prev){
                    key_a = false;
                }
                key_a_prev = false;
            }
            break;
            
            // grab the 'r' key press
        case 114:
        case 82:
            if(g_keys->keyDown[114] || g_keys->keyDown[82]){
                strUtils::appendLog("Toggling 'r', no replacement");
                if(key_r_prev){
                    key_r = true;
                }
                key_r_prev = true;
            }
            else{
                strUtils::appendErrLog("Hit else in case 114/82, why?");
                if(!key_r_prev){
                    key_r = false;
                }
                key_r_prev = false;
            }
            break;
        default:
            strUtils::appendLog("Hit keyboard switch statement default, bad?");
            strUtils::appendErrLog("Hit keyboard switch statement default, bad?");
            break;
	} //switch(key)
    
	/**
	 * Some fun if else if else for changing worm state from keyboard entry
	 */
    
    
	// rotate the worm
	if(key_r){
        strUtils::appendLog("Rotating the worm");
		worm->rotateWormAroundAnterPosterAxis(1.f);
		f6 = false;
	}
    
	if(f6){
		if(g_keys->keyDown[100]){
            strUtils::appendLog("Keyboard input 'd': UpdateSelection(1)");
			worm->UpdateSelection(1);
		}
		if(g_keys->keyDown[102]){
			strUtils::appendLog("Keyboard input 'f': UpdateSelection(2)");
			worm->UpdateSelection(2);
		}
		if(g_keys->keyDown[104]){
			strUtils::appendLog("Keyboard input 'h': UpdateSelection(3)");
			worm->UpdateSelection(3);
		}
		if(g_keys->keyDown[98]){
			strUtils::appendLog("Keyboard input 'b': UpdateSelection(4)");
			worm->UpdateSelection(4);
		}
		if(g_keys->keyDown[97]){
			strUtils::appendLog("Keyboard input 'a': UpdateSelection(5)");
			worm->UpdateSelection(5);
		}
		if(g_keys->keyDown[105]){
			strUtils::appendLog("Keyboard input 'i': UpdateSelection(6)");
			worm->UpdateSelection(6);
		}
		if(g_keys->keyDown[83]){
			strUtils::appendErrLog("Keyboard input 'S': savePosition(), Can't do it!");
			strUtils::appendLog("Save attempted, can't do it, marked 'S' as false");
			g_keys->keyDown[83] = false;
			// Can't do this:
			//worm->savePosition();
		}
        
	} // if(f6)
} //keyboard

void specialKeyboard(int key, int mouse_x, int mouse_y){
    float fraction = 0.1f;

    switch(key){
        case GLUT_KEY_F1:
            if(f1 == false){
                strUtils::appendLog("Showing Menu");
                f1 = true; // Set f1 to true, show the menu
                f1 = !f1; // I don't know why I need this, they have it.
            }
            else if(f1 == true){
                strUtils::appendLog("Hiding Menu");
                f1 = false; // Set f1 to false, hide the menu
                f1 = !f1; // This still seems like a bad idea to have
            }
            else{
                strUtils::appendErrLog("KEYBOARD ERROR: SHOWING OR HIDING MENU");
            }
            break; 
            // I don't have a replacement for full screen, unlikely it will work
            // Moving on, F3 comes next in their list, but plays with f10, guess I'll
            // do it anyway, though I don't think it does anything.
          /*
        case GLUT_KEY_F3:
            if(g_keys->keyDown[109] || g_keys->keyDown[77]){
                strUtils::appendLog("Toggling F3, sets f10 to !f10, not sure what else");
                f10 = !f10; // Seriously, I don't know why I'm writing this, I don't think it does anything at all
            }
            else{
                strUtils::appendErrLog("Found the bug with F3");
            }
            break;
            */
        case GLUT_KEY_F4:
            if(g_keys->keyDown[110] || g_keys->keyDown[78]){
                strUtils::appendLog("Toggling F4, sets f11 to !f11, not sure what else");
                f11 = !f11; // Seriously, I don't know why I'm writing this, I don't think it does anything at all
            }
            else{
                strUtils::appendErrLog("Found the bug with F4");
            }
            break;
            
            // This toggles the full view / neuromuscular system only view
        case GLUT_KEY_F5:
            strUtils::appendLog("Toggling F5, full view / neuromuscular system only view");
            // they set f5 false when this is hit, dunno what to do there
            /*
            if(g_keys->keyDown[120] == false || g_keys->keyDown[88] == false){
                strUtils::appendErrLog("Found bug in F5, keyboard function, toggle view, probably can't toggle it back to normal");
            }
            
            g_keys->keyDown[120] = false;
            g_keys->keyDown[88] = false;
            */
            mode = !mode;
            break;
            
            // This toggles physics on/off, OFF by default
        case GLUT_KEY_F6:
            strUtils::appendLog("Toggling F6, turns physics on and off");
            // they set f6 false, lets do the same since we have it.
            mode2 = !mode2;
            
            if(f6 == false){
                strUtils::appendErrLog("Found bug in F6, keyboard function, physics on/off, probably can't toggle it back");
            }
            
            f6 = false; // Enable physics
            break;
            
            // send sin wave directly to muscles
        case GLUT_KEY_F7:
            strUtils::appendLog("Toggling F7, send sin wave direct to muscles");
            mode3 = !mode3; 
            
            if(mode4 == false){
                strUtils::appendErrLog("Found a bug in F7, keyboard function, sin to muscles, probably can't toggle it off");
            }
            
            mode4 = false;
            f6 = false; // can't toggle physics anymore
            break;
            
            // send sin wave to ventral cord motorneurons
        case GLUT_KEY_F8:
            strUtils::appendLog("Toggling F8, send sin wave to neurons");
            mode4 = !mode4;
            if (mode3 == false){
                strUtils::appendErrLog("Found a bug in F8, keyboard function, sin to neuron, probably can't toggle it off");
            }
            mode3 = false;
            f6 = false; // can't toggle physics anymore
            break;
            //****************************************


        case GLUT_KEY_LEFT :
            angle -= 0.01f;
            lx = sin(angle);
            lz = -cos(angle);
            break;
        case GLUT_KEY_RIGHT :
            angle += 0.01f;
            lx = sin(angle);
            lz = -cos(angle);
            break;
        case GLUT_KEY_F2:
            pan_angle -= 0.01f;
            ly = sin(pan_angle);
            break;
        case GLUT_KEY_F3:
            pan_angle += 0.01f;
            ly = sin(pan_angle);
            x += lx * fraction;
            z += lz * fraction;
            break;
        case GLUT_KEY_UP :
            x += lx * fraction;
            z += lz * fraction;
            break;
        case GLUT_KEY_DOWN :
            x -= lx * fraction;
            z -= lz * fraction;
            break;
        case GLUT_KEY_F9:
            scale += 0.15f;
            strUtils::appendLog("Scale is: " + strUtils::numToString(scale));
            break;
        case GLUT_KEY_F10:
            scale -= 0.15f;
            strUtils::appendLog("Scale is: " + strUtils::numToString(scale));
            break;
            
            //****************************************
            
            
            
            
            
            
        default:
            strUtils::appendLog("Hit speialKeyboard default. Something went wrong.");
            strUtils::appendErrLog("Hit specialKeyboard default. Something we wrong.");
            break; // really?
            
	} //switch
 } // special keyboard

void CElegans::ClearSelection()
{
	for(unsigned int i=0; i< neuron.size(); ++i)
		neuron[i] ->unselect();
}

void CElegans::UpdateSelection(int direction)
{
	for(int i = 0; i<(int)neuron.size(); i++)
	{
		std::basic_string <char>::size_type contain;
		contain = neuron[i]->getName().find("Pse_");
		if(contain != std::string::npos)
		{
			if(neuron[i]->isSelected())
			{
				if(direction == 1)
					neuron[i]->setPosX(0.001);
				if(direction == 2)
					neuron[i]->setPosX(-0.001);
				if(direction == 3)
					neuron[i]->setPosZ(0.001);
				if(direction == 4)
					neuron[i]->setPosZ(-0.001);
				if(direction == 5)
					neuron[i]->setPosY(0.001);
				if(direction == 6)
					neuron[i]->setPosY(-0.001);
				return;
			}
		}
	}
}


CNeuron * CElegans::UpdateSelection()
{
    //scale = scale * 2.0f;
    unsigned int i;
	Vector3D u = (vbeg - vend).unit();
	Vector3D w,n;//,uw;
	double d,z;
	int j=-1;
    
	for(i=0; i< neuron.size(); ++i)
	{
		n = Vector3D(neuron[i]->getX(),neuron[i]->getY(),neuron[i]->getZ());
		w = (ort1*(neuron[i]->getX()-pos_rc.x) + ort2*(neuron[i]->getY()-pos_rc.y) + ort3*(neuron[i]->getZ()-pos_rc.z))*scale + vcenter - vend;
		n = u*(u*w) + vend;
		w = w + vend;
        
		d = (n - w).length();
        
		if(d<=neuron_radius*1*scale)
		{
			if((j==-1)||((j>=0)&&(w.z>z)))
			{
				z = w.z;
				j = i;
			}
		}
	}
    
	if(j>=0)
	{
		if(neuron[j] ->isSelected() == false)
		{
			std::basic_string <char>::size_type contain;
			contain = neuron[j]->getName().find("Pse_");
			if(contain != std::string::npos)
			{
				for(int k = 0; k < (int)neuron.size(); k++)
				{
					std::basic_string <char>::size_type contain_temp;
					contain_temp = neuron[k]->getName().find("Pse_");
					if(contain_temp != std::string::npos)
					{
						if(neuron[k]->isSelected())
						{
							neuron[k]->unselect();
							/*sel = true;
                             break;*/
						}
						//neuron[j] ->select();
					}
				}
				neuron[j] ->select();
			}
			else
			{
				neuron[j] ->select();
			}
			return neuron[j];
		}
		else
			neuron[j] ->unselect();
        
		vcenter += (ort1*(neuron[j]->getX()-pos_rc.x) + ort2*(neuron[j]->getY()-pos_rc.y) + ort3*(neuron[j]->getZ()-pos_rc.z))*scale;
        
		pos_rc = neuron[j]->getPos();
		return NULL;
	}
	return NULL;
}



/**
 * printing method
 */

void my_glPrint(GLint x, GLint y, const char *string, int r, int g, int b){
	glColor3ub(0,0,100);
	glPrint(x-1,y,string);
	glPrint(x,y-1,string);
	glPrint(x+1,y,string);
	glPrint(x,y+1,string);
    
	glColor3ub(r, g, b);
	glPrint(x,y,string);
}


/**
 * This is the display function, it does the majority of the drawing
 * Some of the code is mine, some of it comes right from CyberElegans
 */
void display(void){
    // Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	char title [] = "TITLE IS SET IN DISPLAY FUNCTION IN MAIN.CPP";
    //gluLookAt(0.0f, 0.0f, 0.0f,
    //          0.0f, 0.0f, 0.0f,
    //          0.0f, 0.0f, 0.0f);
    //glTranslatef(0.0f,0.0f,0.0f);

    
	GLUquadricObj * quadObj;
    quadObj = gluNewQuadric();
    
	glPushMatrix();
    
	glColor3ub(255, 255, 0);
	glLineWidth(2);
	double sc = 0.7*scale;

    // I can comment this block, from begin to end, with no noticable ill effects
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);
	glVertex3d(vcenter.x, vcenter.y, vcenter.z);
	glVertex3d(vcenter.x+ort1.x*sc, vcenter.y+ort1.y*sc, vcenter.z+ort1.z*sc);
    
	glColor3ub(0,255, 0);
	glVertex3d(vcenter.x,vcenter.y,vcenter.z);
	glVertex3d(vcenter.x+ort2.x*sc,vcenter.y+ort2.y*sc,vcenter.z+ort2.z*sc);
    
	glColor3ub(0, 0, 255);
	glVertex3d(vcenter.x,vcenter.y,vcenter.z);
	glVertex3d(vcenter.x+ort3.x*sc,vcenter.y+ort3.y*sc,vcenter.z+ort3.z*sc);
 
	glEnd();

	Vector3D rp1 = (ort1*(0-pos_rc.x) + ort2*(0-pos_rc.y) + ort3*(-0.2-pos_rc.z))*scale + vcenter;
	Vector3D rp2 = (ort1*(0-pos_rc.x) + ort2*(0-pos_rc.y) + ort3*(1-pos_rc.z))*scale + vcenter;
	float pi = 3.14159f;
    
	Vector3D p = rp2 - rp1;
	double h = p.length() / 2;
    
	double bkg_w = 10;
    
	// Is this the sand? If so, no go.
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, myTexture[0]);
    
    // Petri dish floor color (sand??)
    glColor3ub(200, 200, 200);
	// Petri Dish Floor
    glBegin(GL_TRIANGLE_FAN);
    
	//Vector3D center = (ort1*(0-pos_rc.x) + ort2*(0-pos_rc.y) + ort3*( -0.1-pos_rc.z))*scale + vcenter;//(ort1*( -pos_rc.x) + ort2*( -pos_rc.y) + ort3*(-pos_rc.z) )*scale + vcenter;
    
	bkg_w = 10.4;
    
    // More petri dish floor
	for(double phi=0; phi<=PI*2; phi+=PI/8){
		//Vector3D ro = ( ort1*((bkg_w-center.x)*sin(phi)) + ort2*(( bkg_w-center.y)*cos(phi)) + ort3*(-0.1-center.z) )*scale + vcenter;
		//Vector3D vv1 = (ort1*(bkg_w-pos_rc.x)*sin(phi) + ort2*(0-pos_rc.y) + ort3*( -0.1-pos_rc.z))*scale;
		//Vector3D vv2 = (ort1*(0-pos_rc.x) + ort2*(bkg_w-pos_rc.y)*cos(phi) + ort3*( -0.1-pos_rc.z))*scale;
        
		Vector3D ro = rp1+(ort1*bkg_w*sin(phi)+ort2*bkg_w*cos(phi))*scale;
		GLdouble point[] = {(GLfloat)ro.x,(GLfloat)ro.y,(GLfloat)ro.z};
		glTexCoord2f((GLfloat)(bkg_w*sin(phi)),(GLfloat)(bkg_w*cos(phi)));
		glVertex3dv(point);
	}
    
	bkg_w = 10;
	glEnd();
	glDisable(GL_TEXTURE_2D);
    
    // Disable ring color
	glColor3ub(100,150,200);
    
    
	glPushMatrix();
    
    // Both inner and outter petri rings
	glTranslated(rp1.x, rp1.y, rp1.z);
    
	glRotated(180.0f*(double)atan2(p.x,p.z)/pi,0,1,0);
	glRotated(-180.0f*(double)atan2(p.y,sqrt(p.x*p.x+p.z*p.z))/pi,1,0,0);
    
    // Dark blue inner petri ring
	gluCylinder(quadObj, bkg_w*scale, bkg_w*scale, h, 32, 1);
	// Light blue outside perti ring
    gluCylinder(quadObj, bkg_w*scale*1.1, bkg_w*scale, h, 32, 1);
    
	glPopMatrix();
    
    /*
     // angle of rotation for the camera direction
     float angle=0.0;
     // actual vector representing the camera's direction
     float lx=0.0f,lz=-1.0f;
     // XZ position of the camera
     float x=0.0f,z=5.0f;
     */
//    gluLookAt(10.0f, 8.0f, 20.0f,
//              10.0f, 8.0f, 0.0f,
//              0.0f, 1.0f, 0.0f);

    gluLookAt(x, y, z,
              x+lx, y+ly, z+lz,
              0.0f, 1.0f, 0.0f);
    
    worm->draw();
    
    
	my_glPrint(15,20,title,0,255,255);
	//Enable this if second title[]
	//my_glPrint(15,40,title2,0,255,255);
	my_glPrint(15,60,"F1 = help show / hide",0,255,175);
    f1 = true;
	if(f1){
		my_glPrint(15, 80,"F2 = toggle fullscreen",0,255,75);
		my_glPrint(15,100,"F5 = toggle full view / neuromuscular system only",0,255,75);
		my_glPrint(15,120,"F6 = physics on/off",0,255,75);
		my_glPrint(15,140,"F7 = send sinusoidal pattern directly to muscles",0,255,75);
		my_glPrint(15,160,"F8 = send sinusoidal pattern to ventral cord motorneurons",0,255,75);
		my_glPrint(15,180,"mouse scroll - change scale",175,255,0);
		my_glPrint(15,200,"mouse right button - rotate scene",175,255,0);
		my_glPrint(15,220,"mouse right button - move scene",175,255,0);
		my_glPrint(15,240,"click at neuron with mouse right button - select / deselect neuron",175,255,0);
		my_glPrint(15,260,"key 'a' - activate selected neuron(s)",255,255,0);
		my_glPrint(15,280,"key 'r' - rotate nematode worm around longitudinal axis",255,255,0);
		my_glPrint(15,300,"NumPad's '4' = x--, '6' = x++, '2' = y--, '8' = y++, '1' = z--, '9' = z++",255,125,0);
		my_glPrint(15,320,"for moving small points defining connection paths between neurons",255,125,0);
		my_glPrint(15,340,"key 's' to save its positions changes [only in editor mode, when physics is still off and no worm rotation was performed]",255,125,0);
	}
    
	glFlush();
	glPopMatrix();
	gluDeleteQuadric(quadObj);
    glutSwapBuffers();
    
	return;
} // display



void reshape(GLsizei width, GLsizei height)									// Reshape The Window When It's Moved Or Resized
{
    if(height == 0){
        height = 1;
    }
    glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				// Reset The Current Viewport
    glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
    glLoadIdentity ();													// Reset The Projection Matrix
    gluPerspective (-45.0f, (GLfloat)(width)/(GLfloat)(height),0.1f, 1000.0f);
    //glOrtho(-10, 10, -10, 10, -10, 100);
    //SEAN --> ON TOP VIEW
    //gluPerspective (90.0f, (GLfloat)(width)/(GLfloat)(height),0.1f, 1000.0f);
    
    glOrtho(10.0f, 2.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
    glLoadIdentity ();													// Reset The Modelview Matrix
    
    ///////////////
    /*glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_COLOR_MATERIAL);
     
     glEnable(GL_NORMALIZE);
     glEnable(GL_AUTO_NORMAL);
     
     float ambient[4] = {0.7, 0.7, 0.7, 1};
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
     */
    //////////////////
}

void update(){
    //float dt = 0.0089f;
    //int numOfIterations = 15;
    //std::string iter = strUtils::numToString(numOfIterations);
    //std::string dt_s = strUtils::numToString(dt);
    //strUtils::appendLog("num of iterations: " + iter);
    //strUtils::appendLog("num of dt: " + dt_s);
    
    //for(int i = 0; i < numOfIterations; ++i)
    //{
        //worm->iteration(dt);
    int increment = 1;
    int i = num_used_neuron_outputs;// easier/faster to type, less clutter to read.
    if (i < (num_neuron_outputs -1) ){// minus 1 so that we can always calculate the number of reps (if we hit the last output, we have nothing to compare it to..
        unsigned long current = start_time + passed_time_windows;
        if (neuron_outputs[i]->timestamp < current){ // want to keep time scale the same
            passed_time_windows++;
            return;
        }
        while (((neuron_outputs[i]->timestamp) + 1 ) >= (neuron_outputs[i+increment]->timestamp)){ // we want to fire all neurons that fired during the same ms together now as well
            increment++;
        }
        // increment is 1 greater than it should be, but we need to factor in the first output when we just fire the i'th signal, so as long as the condition is j < increment (rather than '<='), we're fine.
        
        //int num_repititions = (int) ((neuron_outputs[i+1]->timestamp - neuron_outputs[i]->timestamp)+1)/2;
        //printf("output #%d... reapeating '%d' times.\n", i, num_repititions);
        int k = 0;
        bool used_output = false;
        for(int j = 0; j < increment; j++){ // this way we send all signals at the "same time" that should be sent together.
            for (int w = 0; w < 25; w++){
                worm->step(neuron_outputs[i+k]->name, 1);
            }
            k++;
            num_used_neuron_outputs++;
        }
        passed_time_windows++;
    }
    else{ // we've gone through all output signals in the file
        // uncommenting the below section of code would cause the worm to repeat itself (assuming you comment out exit(0) too..)
        //num_used_neuron_outputs = 0;
        //passed_time_windows = 0;
        //for(int i = 0; i < 10; i++){
        //    worm->step(neuron_outputs[num_used_neuron_outputs]->name, 1);
        //}
        //num_used_neuron_outputs++;
        exit(0);
    }
    //}
    glutPostRedisplay();
}



int main(int argc, char** argv){
    strUtils::appendLog("program started...");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //rgba color, double buffer
	glutInitWindowSize(window_width, window_height);
    //glutInitWindowSize(640,480);
    glutInitWindowPosition(100,100);
	glutCreateWindow("nixelegans");
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    //----------------------------------------------------------
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
	float ambient[4] = {1.0, 1.0, 1.0, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	worm->ClearSelection();
	worm->rotateWormAroundAnterPosterAxis(90.0f);
    //----------------------------------------------------------
	glutDisplayFunc(display);
    strUtils::appendLog("DISPLAY CALLED");
    glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
    glutIdleFunc(update);
    gettimeofday (&last_idle_time, NULL);
    
    
    // open neuron outputs
    // FILE f = open("motor_neuron_outputs.csv", 'r');
    std::ifstream f;
    f.open("motor_neuron_outputs.csv");
    // read them in, convert time to a long (because it is a long), and store in an array.
    // but first, count lines
    int lcount = 0;
    std::string line;
    //char* line = (char*)malloc(sizeof(char)*256);
    //std::cout << "pos init " << f.tellg() << std::endl;
    if (f.is_open()){
        line.erase();
        while (!f.eof()){
            std::getline(f,line);
            if (line.empty()){
                continue;
            }
            //printf("<lcount> LINE: >%s<\n",line.c_str());
            lcount++;
        }
        //printf("num lines in neuron outputs: %d\n",lcount);
    }
    //neuron_outputs = (struct NeuronOutput*) malloc(lcount*sizeof(struct NeuronOutput));
    neuron_outputs = (struct NeuronOutput**) malloc(lcount*sizeof(struct NeuronOutput*));
    //std::cout << "pos pre " << f.tellg() << std::endl;
    f.clear();
    f.seekg(f.beg);// that's actually just a 0.
    line.erase();
    //std::cout << "pos post " << f.tellg() << std::endl;
    int index = 0;
    while (!f.eof()){
        neuron_outputs[index] = new NeuronOutput();
        std::getline(f,line);
        if (line.empty()){
            continue;
        }
        //printf("LINE: >%s<\n",line.c_str());
        int comma = line.find(',');// only 1 comma right now
        //printf("comma: %d\n",comma);
        neuron_outputs[index]->timestamp = std::atol(line.substr(0,comma).c_str());
        if (index == 0){
            start_time = neuron_outputs[index]->timestamp;
        }
        std::string temp = line.substr(comma+1);
        neuron_outputs[index]->name = temp;
        index++;
    }
    num_neuron_outputs = index;
    
    /*
    for (int i =0; i < lcount; i++){
        printf("output: %s at %ld\n",neuron_outputs[i]->name.c_str(), neuron_outputs[i]->timestamp);
        
    }
    */
	glutMainLoop();
}
