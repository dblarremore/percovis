#ifndef _CLUSTER_H_
#define _CLUSTER_H_

#include <vector>
#include "Percnode.h"

using namespace std;

class Percnode;

class Cluster {

public:
	
	//Constructor
	Cluster(int);
	
	//Destructor
	~Cluster()
	{
	}
	
	//Public Data
	int getSize();
	vector <Percnode*> *getNodes();
	void eatCluster(Cluster*);
	void pickyEatCluster(Cluster*);
	Percnode* getMaxDegreeNode();
	void setMaxDegreeNode(Percnode*);
	int getID();
	void initializeCluster(Percnode*);


	//Private Data
private:
	vector<Percnode*> myNodes;
	int myClusterID;
	Percnode* myMaxDegreeNode;
	
	Cluster(); //Constructor
	Cluster(const Cluster &rhs); 
	//
	// Copy constructor.  Not implemented.
	//
	
	Cluster &operator=(const Cluster &rhs); //Assignment operator
};

#endif