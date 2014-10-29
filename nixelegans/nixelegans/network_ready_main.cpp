/**
 * main.cpp
 *
 *  Created on: Sep 10, 2014
 *      Author: Sean Grimes, Andrew W.E. McDonald
 *
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


//#include "graphics.h"	// Broken windows graphics file, uses WINAPI
#include "VectorMath.h"	// Header file for Physics2.h??
#include "CPoint.h"
#include "CConnector.h"
#include "CNeuron.h"
#include "CElegans.h"
#include "fstream"
#include <sstream>
#include <fstream> 		// I don't know why the local copy is inlcuded above
#include <iostream>
#include <ctime>
#include "netdb.h"
#include <sys/time.h> // Replace GetTickCount() from WINAPI
#include "sgStringUtils.h" //general utility functions for strings
//#include "Graphics.h"
#include <cstdlib>
#include "errno.h"
#include <iostream>
//#include "pthread.h"
#define MaxPossible_dt 0.009f
#define window_width 1280
#define window_height 800

struct ServerConnection* connection;
struct MessageReader* message_reader;

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

unsigned mouse_button = 0;
int mouse_state = 0;		// --> why not bool?
int mx, my;					// --> mouse x, mouse y
bool ldown = false;
bool rdown = false;
Vector3D vcenter(0,0,-8);
Vector3D pos_rc(0,0,0);



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
bool key_a_prev = false;
bool key_r = false;			// --> rotate around long axis
bool key_r_prev = false;

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
    glGetIntegerv(GL_VIEWPORT,viewport);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    //  vx = mouse_x;
    //  vy = window_height - mouse_y - 1; // ãäå height - òåêóùàÿ âûñîòà îêíà.
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
             f10 = !f10;
             }
             else{
             strUtils::appendErrLog("Found the bug with F3");
             }
             break;
             */
        case GLUT_KEY_F4:
            if(g_keys->keyDown[110] || g_keys->keyDown[78]){
                strUtils::appendLog("Toggling F4, sets f11 to !f11, not sure what else");
                f11 = !f11;
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
        default:
            strUtils::appendLog("Hit speialKeyboard default. Something went wrong.");
            strUtils::appendErrLog("Hit specialKeyboard default. Something we wrong.");
            break;
            
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
   
    // No sand in this version (yet?)
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
    //ON TOP VIEW
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
}


/***
 *
 *
 * Begin nework code
 *
 *
 ***/

/** begin copy from csapp **/
/** Partially taken from "Computer Systems a Programmers Perspective" **/
typedef struct sockaddr SA;

/* Persistent state for the robust I/O (Rio) package */
/* $begin rio_t */
#define RIO_BUFSIZE 8192
typedef struct {
    int rio_fd;                /* descriptor for this internal buf */
    int rio_cnt;               /* unread bytes in internal buf */
    char *rio_bufptr;          /* next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* internal buffer */
} rio_t;
/* $end rio_t */

void Close(int fd)
{
    int rc;
    
    if ((rc = close(fd)) < 0)
        strUtils::appendErrLog("Close error");
}

/****************************
 * Sockets interface wrappers
 ****************************/

int Socket(int domain, int type, int protocol)
{
    int rc;
    
    if ((rc = socket(domain, type, protocol)) < 0)
        strUtils::appendErrLog("Socket error");
    return rc;
}

void Setsockopt(int s, int level, int optname, const void *optval, int optlen)
{
    int rc;
    
    if ((rc = setsockopt(s, level, optname, optval, optlen)) < 0)
        strUtils::appendErrLog("Setsockopt error");
}

void Bind(int sockfd, struct sockaddr *my_addr, int addrlen)
{
    int rc;
    
    if ((rc = bind(sockfd, my_addr, addrlen)) < 0)
        strUtils::appendErrLog("Bind error");
}

void Listen(int s, int backlog)
{
    int rc;
    
    if ((rc = listen(s,  backlog)) < 0)
        strUtils::appendErrLog("Listen error");
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    int rc;
    
    if ((rc = accept(s, addr, addrlen)) < 0)
        strUtils::appendErrLog("Accept error");
    return rc;
}

void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
{
    int rc;
    
    if ((rc = connect(sockfd, serv_addr, addrlen)) < 0)
        strUtils::appendErrLog("Connect error");
}

/*
 * rio_read - This is a wrapper for the Unix read() function that
 *    transfers min(n, rio_cnt) bytes from an internal buffer to a user
 *    buffer, where n is the number of bytes requested by the user and
 *    rio_cnt is the number of unread bytes in the internal buffer. On
 *    entry, rio_read() refills the internal buffer via a call to
 *    read() if the internal buffer is empty.
 */
/* $begin rio_read */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;
    
    while (rp->rio_cnt <= 0) {  /* refill if buf is empty */
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf,
                           sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0) {
            if (errno != EINTR) /* interrupted by sig handler return */
                return -1;
        }
        else if (rp->rio_cnt == 0)  /* EOF */
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf; /* reset buffer ptr */
    }
    
    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;
    if (rp->rio_cnt < n)
        cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}
/* $end rio_read */


/*
 * rio_readlineb - robustly read a text line (buffered)
 */
/* $begin rio_readlineb */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    int n, rc;
    char c;
    char* bufp = (char*) usrbuf;
    
    for (n = 1; n < maxlen; n++) {
        if ((rc = rio_read(rp, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return 0; /* EOF, no data read */
            else
                break;    /* EOF, some data was read */
        } else
            return -1;	  /* error */
    }
    *bufp = 0;
    return n;
}
/* $end rio_readlineb */

/*
 * rio_readinitb - Associate a descriptor with a read buffer and reset buffer
 */
/* $begin rio_readinitb */
void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}
/* $end rio_readinitb */

/*
 * rio_readnb - Robustly read n bytes (buffered)
 */
/* $begin rio_readnb */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char*) usrbuf;
    
    while (nleft > 0) {
        if ((nread = rio_read(rp, bufp, nleft)) < 0) {
            if (errno == EINTR) /* interrupted by sig handler return */
                nread = 0;      /* call read() again */
            else
                return -1;      /* errno set by read() */
        }
        else if (nread == 0)
            break;              /* EOF */
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);         /* return >= 0 */
}
/* $end rio_readnb */

struct hostent *Gethostbyaddr(const char *addr, int len, int type)
{
    struct hostent *p;
    
    if ((p = gethostbyaddr(addr, len, type)) == NULL)
        strUtils::appendErrLog("Gethostbyaddr error");
    return p;
}

void Rio_readinitb(rio_t *rp, int fd)
{
    rio_readinitb(rp, fd);
}

#define LISTENQ  1024  /* second argument to listen() */
/*
 * open_listenfd - open and return a listening socket on port
 *     Returns -1 and sets errno on Unix error.
 */
/* $begin open_listenfd */
int open_listenfd(int port)
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
    
    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;
    
    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;
    
    /* Listenfd will be an endpoint for all requests to port
     on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;
    
    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
}
/* $end open_listenfd */


/** end copy from csapp **/

#define BUF_SIZE 512

struct ServerConnection{
	int connfd;
	int* listenfd;
	int clientlen;
	struct sockaddr_in clientaddr;
	struct hostent* hp;
	rio_t rio_server;
	//char* send_buffer[BUF_SIZE];
	//char* recieve_buffer[BUF_SIZE];
	pthread_t reader, writer;
	pthread_mutex_t lock;
	//WINDOW* output;
	//WINDOW* input;
};

struct MessageSender{
	int* fd;
	char* alias;
	int* still_chatting_ptr;
	pthread_mutex_t* lock_ptr;
	//WINDOW** output_p;
	//WINDOW** input_p;
};

struct MessageReader{
	rio_t* rio;
	char* alias;
	int* still_chatting_ptr;
	pthread_mutex_t* lock_ptr;
	//WINDOW** output_p;
	//WINDOW** input_p;
};

/* This was taken from csapp.c, I only changed 'gethostbyname' to 'gethostbyaddr'*/
int open_clientfd_addr(struct in_addr* hostaddr, int port){
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;
    
	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1; /* check errno for cause of error */
    
	/* Fill in the server's IP address and port */
	if ((hp = gethostbyaddr(hostaddr,sizeof(*hostaddr),AF_INET)) == NULL) // Get host by address rather than by name!
		return -2; /* check h_errno for cause of error */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)hp->h_addr_list[0],
          (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port = htons(port);
    
	/* Establish a connection with the server */
	if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	return clientfd;
}

ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    ssize_t rc;
    
    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
        strUtils::appendErrLog("Rio_readlineb error");
    return rc;
}


char* read_and_print_message(void* message_reader){
	/* read reply from server */
	struct MessageReader* reader = (struct MessageReader*)message_reader;
	char receive_buffer[BUF_SIZE];
	int line_number = 0;
	//while(*(reader->still_chatting_ptr)){
    //pthread_mutex_lock(reader->lock_ptr);
    //printf("Recieving message\n");
    memset(receive_buffer,0,BUF_SIZE);
    Rio_readlineb(reader->rio,receive_buffer,BUF_SIZE);
    std::cout << "func: >" << receive_buffer << "<" << std::endl;
    if(!(receive_buffer[0] == '\0')){
        //if(recieve_buffer
        if(strstr(receive_buffer,".exit") != NULL){
            // if first line, and first char is a '.', check for valid command. If no vaild command, simply print message.
            //printf("Command: ");
            *(reader->still_chatting_ptr) = 0;
            int uni_flow[2];
            pipe(uni_flow); // creat a unidirectional pipe.
            dup2(uni_flow[0],0); // set uni_flow[0] to point to stdin
            //FILE* stdin_fake = fdopen(uni_flow[1],"w");
            write(uni_flow[1],"\n",2*sizeof(char));
            //close(stdin_fake);
            //break;
        }
        //fputs(recieve_buffer,stdout);
        //printf("\r                                                                         \r");
        //write_to_console(*(reader->output_p),recieve_buffer);
    }
    else{
        strncpy(receive_buffer,"EMPTY\0",6);
    }
    //memset(recieve_buf,0,MAX_RECIEVE);
    //printf("Finished recieving message\n");
    //pthread_mutex_unlock(reader->lock_ptr);
	//}
	//pthread_exit(NULL); // Do I want the thread to exit here???? Not sure...
    return receive_buffer;
}

void *accept_connection(void* connection){
	struct ServerConnection* acceptor = (struct ServerConnection*)connection;
	acceptor->clientlen = sizeof(acceptor->clientaddr);
	//printf("acceptor->listenfd == %d, value it points to == %d\n",acceptor->listenfd,*(acceptor->listenfd));
	acceptor->connfd = Accept(*(acceptor->listenfd), (SA*)&(acceptor->clientaddr),(socklen_t*) &(acceptor->clientlen));
	acceptor->hp = Gethostbyaddr((const char*)&(acceptor->clientaddr).sin_addr.s_addr,sizeof(acceptor->clientaddr.sin_addr.s_addr),AF_INET);
	//write_to_console(acceptor->output,"********** Connected to client **********\n");
	strUtils::appendLog("***** Connected to Client *****\n");
    std::cout << "connected to client." << std::endl;
    pthread_exit(NULL);
}

void update(){
    float dt = 0.0089f;
    int numOfIterations = 15;
    std::string iter = strUtils::numToString(numOfIterations);
    std::string dt_s = strUtils::numToString(dt);
    //strUtils::appendLog("num of iterations: " + iter);
    //strUtils::appendLog("num of dt: " + dt_s);
    char* message = read_and_print_message(message_reader);
    strUtils::appendLog(message);
    std::cout << ">" << message << "<" << std::endl;
    for(int i = 0; i < numOfIterations; ++i)
    {
        worm->iteration(dt);
    }
    glutPostRedisplay();
}


int main(int argc, char** argv){
    strUtils::appendLog("program started...");
    std::cout << "prog. started..." << std::endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //rgba color, double buffer
	glutInitWindowSize(window_width, window_height);
    //glutInitWindowSize(640,480);
    glutInitWindowPosition(100,100);
	glutCreateWindow("nixElegans");
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
    
    /*** begin networking ***/
    
    char* server_alias = (char*) malloc(strlen("server_alias")+sizeof(char));
    strncpy(server_alias,"server_alias\0",13);
    strcpy(server_alias,argv[3]);
    int listen_port = 64321;//atoi(argv[2]);
    int listenfd = open_listenfd(listen_port);
    int rc;
    
	/* loop to continually accept client requests to connect */
    connection = (ServerConnection*) malloc(sizeof(struct ServerConnection));
    connection->listenfd = &listenfd;
    //printf("value of listenfd == %d, address of listenfd == %d\nvalue of connection->listenfd == %d, value connection->listenfd points to == %d\n",listenfd,&listenfd,connection->listenfd,*(connection->listenfd));
    pthread_mutex_init(&connection->lock,NULL);
    
    pthread_t accept_thread;
    if((rc = pthread_create(&accept_thread,NULL,accept_connection, (void*)connection)) != 0){// sender is a pthread_t, thread is a struct Thread
        perror("Error creating thread to accept connection!");
        exit(2);
    }
    
    pthread_join(accept_thread,NULL);// wait for accept to do it's thing. This is threaded to lay groundwork for supporting multiple connections (incoming). Though at this time, it does not.
    
    //struct MessageSender* message_sender = (MessageSender*) malloc(sizeof(struct MessageSender));
    message_reader = (MessageReader*) malloc(sizeof(struct MessageReader));
    //message_sender->fd = &(connection->connfd);
    //message_sender->lock_ptr = &connection->lock;
    //message_sender->alias = malloc(strlen(server_alias)+3*sizeof(char));
    message_reader->alias = (char*) malloc(strlen(server_alias)+3*sizeof(char));
    //strcpy(message_sender->alias,server_alias);
    //strcat(message_sender->alias,"> "); // so that at the beginning of each sent message you see "alias> "
    //strcpy(message_reader->alias,message_sender->alias);
    message_reader->lock_ptr = &connection->lock;
    Rio_readinitb(&connection->rio_server,connection->connfd);
    message_reader->rio = &connection->rio_server;
    int still_chatting = 1;
    //message_sender->still_chatting_ptr = &still_chatting;
    message_reader->still_chatting_ptr = &still_chatting;
    /*
     if((rc = pthread_create(&connection->writer,NULL,get_and_send_message, (void*)message_sender)) != 0){
     perror("Error creating thread to get_and_send_message!");
     exit(3);
     }
     */
    //if((rc = pthread_create(&connection->reader,NULL,read_and_print_message, (void*)message_reader)) != 0){
    //    perror("Error creating thread to get_and_send_message!");
    //    exit(4);
    //}
    strUtils::appendLog("We begin!");
	glutMainLoop();
    
    pthread_join(connection->writer,NULL);
    pthread_join(connection->reader,NULL);
    free(server_alias);
    //free(message_sender->alias);
    //free(message_sender);
    free(message_reader);
    Close(connection->connfd);
    pthread_mutex_destroy(&connection->lock);
    free(connection);
    
}
