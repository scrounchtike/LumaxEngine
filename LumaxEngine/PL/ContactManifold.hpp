
#ifndef CONTACT_MANIFOLD_HPP
#define CONTACT_MANIFOLD_HPP

#include "../math.hpp"

class ContactManifold {
public:
	ContactManifold();

	bool isIntersecting;
	Vec3 normal;
	float penetration;
};

#endif
