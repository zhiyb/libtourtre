#ifndef MESH_H
#define MESH_H

#include <vector>

#include "Global.h"
#include "Data.h"

//abstract mesh class
class Mesh 
{
	public:
	
	Data & data;
	Mesh(Data & d) : data(d) {}
	
	size_t getNeighbors(size_t i, size_t *n)
	{
		uint x,y,z;
		data.convertIndex( i, x, y, z );
		if ( (x+y+z)%2 == ODD_TET_PARITY ) {
			return find6Neighbors(x,y,z,n);
		} else {
			return find18Neighbors(x,y,z,n);
		}
	}

	void createGraph(std::vector<size_t> & order);
	uint numVerts();
	
	

	size_t find6Neighbors( uint x, uint y, uint z, size_t *neighbors);
	size_t find18Neighbors( uint x, uint y, uint z, size_t *neighbors);
	
};

#endif
