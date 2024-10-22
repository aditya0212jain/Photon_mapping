#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <iostream>
#include "glm/glm.hpp"
#include "light.h"

#define SNOWMAN 1
#define GLASS 2
#define WALL 3
#define LIGHT 4
#define CYLINDER 5
#define BALL 6
#define TRACE_DEPTH 4

#define TRACE_DEPTH_PHOTON 5
#define PHOTON_RADIUS 0.5
#define N_PHOTONS_GLOBAL 1000000
#define N_PHOTONS_CAUSTIC 10000
#define CAUSTIC_PHOTON_RADIUS 0.5
#define TRACE_DEPTH_CAUSTIC_PHOTON 3

typedef glm::vec3 ColorRGB;


class KDTreeNode
{
    public:
	typedef double value_type;
	Photon p;
	size_t index;

	value_type operator[](size_t n) const
	{
		return p.pos[n];
	}

	double distance( const KDTreeNode &node)
	{
		return glm::length(node.p.pos - p.pos);
	}
};





#endif
