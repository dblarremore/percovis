#include "Percnode.h"
#include "Cluster.h"

//Constructor
Percnode::Percnode(Cluster* c, int id) :
myCluster(c), myNodeID(id),myDX(0.0),myDY(0.0),isEligible(1)
{
}

unsigned int Percnode::getDegree()
{
	return myNeighbors.size();
}

vector<Percnode*> *Percnode::getNeighbors()
{
	return &myNeighbors;
}

void Percnode::addNeighbor(Percnode* newNeighbor)
{
	myNeighbors.push_back(newNeighbor);
}

int Percnode::getID()
{
	return myNodeID;
}
// X and Y
float Percnode::getX()
{
	return myX;
}

float Percnode::getY()
{
	return myY;
}

void Percnode::setX(float x)
{
	myX = x;
}

void Percnode::setY(float y)
{
	myY = y;
}

// DX and DY
float Percnode::getDX()
{
	return myDX;
}

float Percnode::getDY()
{
	return myDY;
}

void Percnode::setDX(float dx)
{
	myDX = dx;
}

void Percnode::setDY(float dy)
{
	myDY = dy;
}

void Percnode::setCluster(Cluster* c)
{
	myCluster = c;
}

Cluster* Percnode::getCluster()
{
	return myCluster;
}

void Percnode::nominateNeighbors(vector<Percnode*> *candidates, int *levelsSearched)
{	
	//Welcome down one level. Take note
	(*levelsSearched)--;
	// You've just asked to nominate your neighbors!
	// Go through your neighbors
	for(unsigned int i=0; i<myNeighbors.size(); ++i)
	{
		// If your neighbor hasn't yet been nominated, add him to the list 
		// AND get him to nominate his nbrs
		if(myNeighbors[i]->checkEligibility())
		{
			// add the bugger
			candidates->push_back(myNeighbors[i]);
			// toggle his candidacy
			myNeighbors[i]->makeIneligible();
			// get him to nominate his buddies IF you have any more lvls left
			if(*levelsSearched > 0)
			{
				myNeighbors[i]->nominateNeighbors(candidates,levelsSearched);
			}
		}
	}
	//You're going back up a level now when you leave this recursion. Take note
	(*levelsSearched)++;
}


bool Percnode:: checkEligibility()
{
	return isEligible; 
}

void Percnode::makeIneligible()
{
	isEligible = 0;
}

void Percnode::makeEligible()
{
	isEligible = 1;
}
