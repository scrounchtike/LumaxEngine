
#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdlib>

typedef long int64;
typedef int int32;
typedef short int16;
typedef char int8;

typedef float float32;
typedef double float64;

typedef unsigned long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef float TYPE_2D;
typedef int TYPE_3D;

static uint16 genID(){
	return rand();
}

#endif
