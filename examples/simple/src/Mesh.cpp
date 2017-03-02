#include "Mesh.h"

#include <iostream>
#include <algorithm>
#include "parallel_stable_sort.h"
using std::cout;
using std::endl;
using std::sort;





//functor for sorting
class AscendingOrder 
{
	Data & data;
	public:
	AscendingOrder( Data & d ) : data(d) {}
	bool operator()(const uint & a, const uint & b) const { 
		return data.less( a , b );
	}
};


void Mesh::createGraph(std::vector<size_t> & order) 
{
	order.resize( data.totalSize );
	
	for (uint i = 0; i < order.size(); i++) 
		order[i] = i;
#pragma omp parallel
#pragma omp master
	pss::parallel_stable_sort( order.begin() , order.end(), AscendingOrder(data) );
}

size_t Mesh::find6Neighbors( uint x, uint y, uint z, size_t *neighbors) 
{
	size_t size = 0;
	auto check = [=, &size, &neighbors](uint x, uint y, uint z) {
		if (x < data.size[0] && y < data.size[1] && z < data.size[2]) {
			*neighbors++ = data.convertIndex(x, y, z);
			size++;
		}
	};

	check(x, y, z - 1);
	check(x, y, z + 1);
	check(x, y - 1, z);
	check(x, y + 1, z);
	check(x - 1, y, z);
	check(x + 1, y, z);
	return size;
}

size_t Mesh::find18Neighbors( uint x, uint y, uint z, size_t *neighbors) 
{
	size_t size = 0;
	auto check = [=, &size, &neighbors](uint x, uint y, uint z) {
		if (x < data.size[0] && y < data.size[1] && z < data.size[2]) {
			*neighbors++ = data.convertIndex(x, y, z);
			size++;
		}
	};

	check(x, y - 1, z);
	check(x, y - 1, z - 1);
	check(x, y - 1, z + 1);
	check(x, y + 1, z + 1);
	check(x, y + 1, z);
	check(x, y + 1, z - 1);
	check(x, y, z - 1);
	check(x, y, z + 1);

	check(x - 1, y, z + 1);
	check(x - 1, y, z);
	check(x - 1, y, z - 1);
	check(x + 1, y, z - 1);
	check(x + 1, y, z + 1);
	check(x + 1, y, z);

	check(x + 1, y - 1, z);
	check(x + 1, y + 1, z);
	check(x - 1, y + 1, z);
	check(x - 1, y - 1, z);
	return size;
}
