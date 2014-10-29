#ifndef _SYN_
#define _SYN_

#include <vector>
#include <string>

#include "VectorMath.h"
#include <OpenGL/gl.h>	// OpenGL
#include <OPenGL/glu.h>	// OpenGL Utility Library
// Apple does things differently for the GLUT directory, so if Apple
#ifdef __APPLE__
#include <GLUT/glut.h>	// OpenGL Utility Toolkit (For Apple)
#else
#include <GL/glut.h>	// OpenGL Utility Toolkit (For Non Apple)
#endif
#include <cstdlib> 		// exit function


class CSynapse
{
protected :
	float income;
	float threshold;
	Vector3D drawPos;
	std::string name;
	bool selected;

public:
//	CSynapse(double i_treshild);
	CSynapse(float i_treshold, const std::string &_name);
	virtual ~CSynapse();
	void getSignal(float i_signal);
	void checkActivity();
	void select();
	void unselect();
	float getActivity();
	Vector3D getDrawPos();
	bool isSelected();
	std::string getName();

};

#endif


#ifndef _C_AXON_
#define _C_AXON_

#include "CPoint.h"

class CAxon
{
private:
	double weight;
//	Vector3D pos;
	CSynapse * synapse;

public:
	CAxon(const double i_weight, CSynapse * i_synapse);
	virtual ~CAxon();
	void send(float senderActivity);
	Vector3D getPos();
	std::string getTargName();
	bool isTargSelected();


private:
	CAxon();
};
#endif

#ifndef _C_NEURON_
#define _C_NEURON_


class CNeuron: public CSynapse, public CPoint
{

private:

	std::vector<CAxon *> axon;
	double ratioX;
	double ratioY;
	double ratioZ;
	char type;
	int clrIndex;

public:
	CNeuron(const std::string i_name, const Vector3D & i_pos, const float i_threshold, const double i_ratioX, const double i_ratioY, const double i_ratioZ, const char i_type, const int clr_index);
	virtual ~CNeuron();
	void addAxon(CAxon * i_axon);
	void addAxon(CSynapse * i_synapse, double i_weight);
	void draw();
	void update();
	char getType();
	double getRatioX();
	double getRatioY();
	double getRatioZ();
	int getColorIndex();
	std::string getName();
	void setPosX(double x_plus){ this->pos.x += x_plus; };
	void setPosY(double y_plus){ this->pos.y += y_plus; };
	void setPosZ(double z_plus){ this->pos.z += z_plus; };


private:
	CNeuron();
};
#endif
