#ifndef _PHYS_
#define _PHYS_
#include <OpenGL/gl.h>	// OpenGL
#include <OPenGL/glu.h>	// OpenGL Utility Library
// Apple does things differently for the GLUT directory, so if Apple
#ifdef __APPLE__
#include <GLUT/glut.h>	// OpenGL Utility Toolkit (For Apple)
#else
#include <GL/glut.h>	// OpenGL Utility Toolkit (For Non Apple)
#endif
#include <cstdlib> 		// exit function
#define Gravity - 9.81f
#define AirFrictionCoefficient 0.002f
#define GroundRepulsionConstant 100.f
#define GroundAbsorptionConstant 2.0f
#define GroundFrictionConstant 0.6f
#define GroundHeight 0.f
#define StiffCoeff 80.f
#define FrictCoeff 0.6f
#define MStrength 20.f//1.f

#define AUTODETECT -1
#endif
