#ifndef _C_POINT_
#define _C_POINT_

#include  "VectorMath.h"
#include <OpenGL/gl.h>	// OpenGL
#include <OPenGL/glu.h>	// OpenGL Utility Library
// Apple does things differently for the GLUT directory, so if Apple
#ifdef __APPLE__
#include <GLUT/glut.h>	// OpenGL Utility Toolkit (For Apple)
#else
#include <GL/glut.h>	// OpenGL Utility Toolkit (For Non Apple)
#endif
#include <cstdlib> 		// exit function

class CPoint
{
protected:
	Vector3D pos;

public:
	CPoint(const Vector3D &i_pos);
	virtual ~CPoint();
	virtual CPoint & operator=(const CPoint &i_point);
	void virtual draw() = 0;
	double getX();
	double getY();
	double getZ();
	Vector3D getPos();
	Vector3D * getAPos();
	void setPos(const Vector3D & i_pos);

private:
	CPoint();
};
#endif

#ifndef _C_MASS_POINT_
#define _C_MASS_POINT_
class CMassPoint: public CPoint
{
private:
	double mass;
	Vector3D vel;
	Vector3D force;

public:
	CMassPoint(const double i_mass, const Vector3D &i_pos);
	virtual ~CMassPoint();
	virtual CMassPoint & operator=(const CMassPoint &i_mpoint);
	void virtual draw();
	void virtual select();
	void init();
	void update();
	void applyForce(const Vector3D & force);
	void timeTick(const double dt);
	Vector3D getVel();
	Vector3D getForce();
	double getMass();

private:
	CMassPoint();

};
#endif
