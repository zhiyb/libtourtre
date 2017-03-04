#include "Data.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <xmmintrin.h>
#if USE_ZLIB
#include <zlib.h>
#endif

#if defined(WIN32)
	#define DIR_SYMBOL "\\"
#else
	#define DIR_SYMBOL "/"
#endif

bool Data::load(const char * file, char * prefix, bool * compressed) {

	char fullPath[1024];
	strcpy(fullPath,file);
	
	//scan for actual filename, rather than full path
	char * filename = fullPath;
	while(true) {
		char * x = strstr(filename,DIR_SYMBOL);
		if (x == '\0') break;
		else {
			filename = x;
			filename++;
		}
	}
	
	char * dimP, * typeP, *gzP;

	dimP = strstr(filename,".");
	*dimP = '\0';
	dimP++;

	typeP = strstr(dimP,".");
	*typeP = '\0';
	typeP++;

	gzP = strstr(typeP,".");
	if (gzP) {
		*gzP = '\0';
		gzP++;
		*compressed = true;
	}
	
	char *xSize, *ySize, *zSize;
	
	xSize = dimP;
	
	ySize = strstr(dimP,"x");
	*ySize = '\0';
	ySize++;

	zSize = strstr(ySize,"x");
	*zSize = '\0';
	zSize++;
	
	size[0] = atoi(xSize);
	size[1] = atoi(ySize);
	size[2] = atoi(zSize);

	std::clog << "Loading " << filename << " with dimensions " << size[0] << " x " << size[1] << " x " << size[2] << ", of type " << typeP << std::endl;
	strcpy(prefix,fullPath);

	totalSize = size[0] * size[1] * size[2];


	std::ifstream infile;
	gzFile zinfile;

	#if USE_ZLIB
		zinfile = gzopen(file,"rb");
		if (!zinfile) {
			std::cerr << "could not open" << filename << std::endl;
			return false;
		}
	#else
		infile.open(file,std::ios::in);
		if (!infile) {
			std::clog << "could not open " << filename << std::endl;
			return false;
		}
	#endif

	data = new DataType[totalSize];
	__m128* pSrc1 = (__m128*) data;

	if ( strcmp(typeP,"uint8") == 0 ) {

		unsigned char * charData = new unsigned char [totalSize];
		
		__m128* pSrc2 = (__m128*) charData;
		#if USE_ZLIB
		gzread( zinfile , reinterpret_cast<char*>(charData) , totalSize );
		#else
		infile.read( reinterpret_cast<char*>(charData) , totalSize );
		#endif
// #pragma ivdep
		for (uint i = 0; i < totalSize/16; i++) {
			// data[i] = static_cast<DataType>( charData[i] );
			*pSrc1++ = *pSrc2++;
		}

		delete[] charData;

	} else if ( strcmp(typeP,"uint16") == 0 ) {

		unsigned short int * intData = new unsigned short int [totalSize];

		__m128* pSrc3 = (__m128*) intData;
		#if USE_ZLIB
		gzread( zinfile , reinterpret_cast<char*>(intData) , totalSize * 2 );
		#else
		infile.read( reinterpret_cast<char*>(intData) , totalSize * 2 );
		#endif

// #pragma ivdep
		for (uint i = 0; i < totalSize; i++) {
			data[i] = static_cast<DataType>( intData[i] );
			// *pSrc1++ = *pSrc3++;

		}

		delete[] intData;

	} else if ( strcmp(typeP,"float") == 0 ) {

		float * floatData = new float [totalSize];

		__m128* pSrc4 = (__m128*) floatData;
		#if USE_ZLIB
		gzread( zinfile , reinterpret_cast<char*>(floatData) , totalSize * 4 );
		#else
		infile.read( reinterpret_cast<char*>(floatData) , totalSize * 4 );
		#endif

		for (uint i = 0; i < totalSize/4; i++) {
			// data[i] = static_cast<DataType>( floatData[i] );
			*pSrc1++ = *pSrc4++;
		}

		delete[] floatData;

	} else if ( strcmp(typeP,"double") == 0 ) {

		double * doubleData = new double [totalSize];

		__m128* pSrc5 = (__m128*) doubleData;
		#if USE_ZLIB
		gzread( zinfile , reinterpret_cast<char*>(doubleData) , totalSize * 8 );
		#else
		infile.read( reinterpret_cast<char*>(doubleData) , totalSize * 8 );
		#endif
		
// #pragma ivdep
		for (uint i = 0; i < totalSize/2; i++) {
			// data[i] = static_cast<DataType>( doubleData[i] );
			*pSrc1++ = *pSrc5++;
		}

		delete[] doubleData;

	} else {

		std::cerr << "unrecognized type : " << typeP << std::endl;
		return false;
	}


	maxValue = minValue = data[0];
	for (uint i = 0; i < totalSize; i++) {
		if (data[i] > maxValue) maxValue = data[i];
		if (data[i] < minValue) minValue = data[i];
	}

	std::clog << "max value was " << (int)maxValue << " and min value was " << (int)minValue << std::endl;

	#if USE_ZLIB
	gzclose( zinfile );
	#else
	infile.close();
	#endif
	
	

	return true;
		
}
