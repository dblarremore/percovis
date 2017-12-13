#include <iostream>
#include "Cluster.h"
#include "Percnode.h"
using namespace std;

//Constructor
Cluster::Cluster(int id) :
myClusterID(id)
{
}

//Get size of a cluster
int Cluster::getSize()
{
	return myNodes.size();
}

//Get a reference to the list of nodes that belong to a cluster
vector <Percnode*> *Cluster::getNodes()
{
	return &myNodes;
}

//Add a node to a cluster. TO BE USED ONLY WHEN INITIALIZING
void Cluster::initializeCluster(Percnode* toBeAdded)
{
	//put the node in the list
	myNodes.push_back(toBeAdded);
	//since it's the only node in the list, it should 
	myMaxDegreeNode = toBeAdded;
}

//Combine two clusters by having one cluster eat the other one
void Cluster::eatCluster(Cluster* dinner)
{	
	for(vector<Percnode*>::iterator updateThisPercnode = dinner->getNodes()->begin();
		updateThisPercnode != dinner->getNodes()->end();
		++updateThisPercnode)
	{
		//First, eat the node by adding it to myNodes
		myNodes.push_back(*updateThisPercnode);
		//Second, update the node to know who its daddy is.
		(*updateThisPercnode)->setCluster(this);
	}
	
	//update the max degree node of "this" if "dinner" has a bigger one
	if(this->getMaxDegreeNode()->getDegree() < dinner->getMaxDegreeNode()->getDegree())
	{
		this->setMaxDegreeNode(dinner->getMaxDegreeNode());
	}
	
	dinner->getNodes()->clear();
}

//Combine two clusters CAREFULLY when the max degree node is in question
void Cluster::pickyEatCluster(Cluster* dinner)
{
	// Same as the normal eating
	for(vector<Percnode*>::iterator updateThisPercnode = dinner->getNodes()->begin();
		updateThisPercnode != dinner->getNodes()->end();
		++updateThisPercnode)
	{
		//First, eat the node by adding it to myNodes
		myNodes.push_back(*updateThisPercnode);
		//Second, update the node to know who its daddy is.
		(*updateThisPercnode)->setCluster(this);
	}
	
	// Different from regular eating.
	// Now, we go through all of myNodes and change the myMaxDegreeNode pointer to the highest degree.
	Percnode* bestCandidate = myNodes[0];
	unsigned int largestDegreeSoFar=0;
	// go through myNodes and get the max degree guy
	for(unsigned int i=0; i < myNodes.size(); ++i)
	{
		if(myNodes[i]->getDegree() > largestDegreeSoFar)
		{
			largestDegreeSoFar = myNodes[i]->getDegree();
			bestCandidate = myNodes[i];
		}
	}
	this->setMaxDegreeNode(bestCandidate);
	
	dinner->getNodes()->clear();
}

int Cluster::getID()
{
	return myClusterID;
}

void Cluster::setMaxDegreeNode(Percnode* toBeSet)
{
	myMaxDegreeNode=toBeSet;
}

Percnode* Cluster::getMaxDegreeNode()
{
	return myMaxDegreeNode;
}