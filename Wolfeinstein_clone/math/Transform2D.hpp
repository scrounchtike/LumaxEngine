
#ifndef TRANSFORM_2D_HPP
#define TRANSFORM_2D_HPP

#include <memory>

#include "../RAL/buildDesc.hpp"

#include "../math.hpp"

class Transform2D {
public:
	Transform2D(Vec2 position = Vec2(0, 0), float rotation = 0, Vec2 scale = Vec2(1, 1), Vec2 pivot = Vec2(0, 0), float pivot_rotation = 0);
	~Transform2D();
	
	Mat4* getTransformation() const { return matrix; }
	void updateMatrix(Mat4* newMatrix) { matrix = newMatrix; }
protected:
	Mat4* matrix;
};

class DynamicTransform2D : public Transform2D {
public:
	DynamicTransform2D(Vec2 position = Vec2(0, 0), float rotation = 0, Vec2 scale = Vec2(1, 1));
	~DynamicTransform2D();

	Vec2 translation;
	float rotation;
	Vec2 scale;

	void updateMatrix();
private:
};

class PivotTransform2D : public DynamicTransform2D {
public:
	PivotTransform2D(Vec2 position = Vec2(0, 0), float rotation = 0, Vec2 scale = Vec2(1, 1), Vec2 pivot = Vec2(0,0), float pivot_rotation = 0);
	~PivotTransform2D();

	Vec2 pivot;
	float pivot_rotation;
	
	void updateMatrix();
private:
};

#endif
