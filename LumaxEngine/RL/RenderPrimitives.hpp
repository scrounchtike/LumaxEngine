
#ifndef RENDER_PRIMITIVES_HPP
#define RENDER_PRIMITIVES_HPP

#include "../math.hpp"

struct Point2D {
	Vec2 position;
	Vec3 color;
	Point2D(Vec2 position, Vec3 color) : position(position), color(color) { }
};

struct Line2D {
	Vec2 A;
	Vec2 B;
	Vec3 color;
	Line2D(Vec2 A, Vec2 B, Vec3 color) : A(A), B(B), color(color) { }
};

struct Sprite2D {
	Vec2 position;
	Vec2 extents;
	Vec3 color;
	Sprite2D(Vec2 position, Vec2 extents, Vec3 color) : position(position), extents(extents), color(color) { }
};

struct Point3D {
	Vec3 position;
	Vec3 color;
	Point3D(Vec3 position, Vec3 color) : position(position), color(color) { }
};

struct Line3D {
	Vec3 start;
	Vec3 end;
	Vec3 color;
	Line3D(Vec3 start, Vec3 end, Vec3 color) : start(start), end(end), color(color) { }
};

struct Sprite3D {
	Vec3 position;
	Vec3 extentU;
	Vec3 extentV;
	Vec3 color;
	Sprite3D(Vec3 position, Vec3 extentU, Vec3 extentV, Vec3 color) : position(position), extentU(extentU), extentV(extentV), color(color) { }
};

#endif