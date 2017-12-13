#ifndef _PERCNODE_H_
#define _PERCNODE_H_

#include <vector>
#include "Cluster.h"

using namespace std;

	class Cluster;

class Percnode {
	
public:
	
	//Constructor
	Percnode(Cluster*, int);
	
	//Destructor
	~Percnode()
	{
	}
	
	//Public Data
	unsigned int getDegree();
	vector<Percnode*> *getNeighbors();
	void addNeighbor(Percnode*);
	int getID();
	float getX();
	float getY();
	float getDX();
	float getDY();
	void setX(float);
	void setY(float);
	void setDX(float);
	void setDY(float);
	
	void nominateNeighbors(vector<Percnode*>*, int*);
	void makeEligible();
	void makeIneligible();
	bool checkEligibility();
	
	void setCluster(Cluster*);
	Cluster* getCluster();
	
	//Private Functions
private:
	Cluster* myCluster;
	int myDegree;
	vector<Percnode*> myNeighbors;
	int	myNodeID;
	float myX;
	float myY;
	float myDX;
	float myDY;
	
		bool isEligible;

	
	Percnode();
	Percnode(const Percnode &rhs);
	Percnode &operator=(const Percnode &rhs);
};

#endif