#ifndef _VectorMath_
#define _VectorMath_

#include <math.h>
#include <OpenGL/gl.h>	// OpenGL
#include <OPenGL/glu.h>	// OpenGL Utility Library
// Apple does things differently for the GLUT directory, so if Apple
#ifdef __APPLE__
#include <GLUT/glut.h>	// OpenGL Utility Toolkit (For Apple)
#else
#include <GL/glut.h>	// OpenGL Utility Toolkit (For Non Apple)
#endif
#include <cstdlib> 		// exit function


const double PI=3.14159265;

//An object to represent a 3D vector or a 3D point in space
class Vector3D
{
public:
	double x;									// the x value of this Vector3D
	double y;									// the y value of this Vector3D
	double z;									// the z value of this Vector3D

	Vector3D()									// Constructor to set x = y = z = 0
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vector3D(double x, double y, double z)			// Constructor that initializes this Vector3D to the intended values of x, y and z
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3D& operator= (Vector3D v)			// operator= sets values of v to this Vector3D. example: v1 = v2 means that values of v2 are set onto v1
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	Vector3D operator+ (Vector3D v)				// operator+ is used to add two Vector3D's. operator+ returns a new Vector3D
	{
		return Vector3D(x + v.x, y + v.y, z + v.z);
	}

	Vector3D operator- (Vector3D v)				// operator- is used to take difference of two Vector3D's. operator- returns a new Vector3D
	{
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	Vector3D operator* (double value)			// operator* is used to scale a Vector3D by a value. This value multiplies the Vector3D's x, y and z.
	{
		return Vector3D(x * value, y * value, z * value);
	}


	Vector3D operator/ (double value)			// operator/ is used to scale a Vector3D by a value. This value divides the Vector3D's x, y and z.
	{
		return Vector3D(x / value, y / value, z / value);
	}

	Vector3D& operator+= (Vector3D v)			// operator+= is used to add another Vector3D to this Vector3D.
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3D& operator-= (Vector3D v)			// operator-= is used to subtract another Vector3D from this Vector3D.
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3D& operator*= (double value)			// operator*= is used to scale this Vector3D by a value.
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	Vector3D& operator/= (double value)			// operator/= is used to scale this Vector3D by a value.
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}


	Vector3D operator- ()						// operator- is used to set this Vector3D's x, y, and z to the negative of them.
	{
		return Vector3D(-x, -y, -z);
	}

	double length()								// length() returns the length of this Vector3D
	{
		return sqrt(x*x + y*y + z*z);
	};

	void unitize()								// unitize() normalizes this Vector3D that its direction remains the same but its length is 1.
	{
		double length = this->length();

		if (length == 0)
			return;

		x /= length;
		y /= length;
		z /= length;
	}

	Vector3D unit()								// unit() returns a new Vector3D. The returned value is a unitized version of this Vector3D.
	{
		double length = this->length();

		if (length == 0)
			return *this;

		return Vector3D(x / length, y / length, z / length);
	}

	double scaleM(Vector3D & vec)
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	double meas()
	{
		return sqrt(x * x + y * y + z * z);
	}

	// No need for scope operator within header file
	//double Vector3D::operator*(Vector3D v)
	double operator*(Vector3D v)
	{
		return ( x*v.x + y*v.y + z*v.z );
	}
	//Vector3D Vector3D::operator%(Vector3D v)
	Vector3D operator%(Vector3D v)
	{
		Vector3D t;
		t.x=y*v.z-z*v.y;
		t.y=z*v.x-x*v.z;
		t.z=x*v.y-y*v.x;
		return t;
	}
	//bool Vector3D::operator==(Vector3D v)
	bool operator==(Vector3D v)
	{
		if((x==v.x)&&(y==v.y)&&(z==v.z))
			return true;
		else
			return false;
	}

	//static Vector3D Vector3D::RotateVector1AroundVector2(Vector3D v1, Vector3D v2 ,double alpha)
	static Vector3D RotateVector1AroundVector2(Vector3D v1, Vector3D v2, double alpha)
	{
		if(v1==v2) return v1;

		Vector3D ort1,ort2,ort3;


		alpha*=(float)PI/180.0f;

		ort1 = v2.unit();
		ort2 = (v1%v2).unit();
		ort3 = (ort2%ort1).unit();
/*
		Vector3D t1 = ort1*(v1*ort1);
		Vector3D t2 = ort2*(v1*ort3)*sin(alpha);
		Vector3D t3 = ort3*(v1*ort3)*cos(alpha);
		Vector3D r = t1+t2+t3;
*/
		return ort1*(v1*ort1) + ort2*(v1*ort3)*sin(alpha) + ort3*(v1*ort3)*cos(alpha);
	}
};






#endif
