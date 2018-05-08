
#ifndef TRANSFORM_3D_HPP
#define TRANSFORM_3D_HPP

#include "../math.hpp"

class Transform3D {
public:
	Transform3D(Vec3 position = Vec3(0, 0, 0), Vec3 rotation = Vec3(0, 0, 0), Vec3 scale = Vec3(1, 1, 1), Vec3 pivot = Vec3(0, 0, 0), Vec3 pivot_rotation = Vec3(0, 0, 0));
	~Transform3D();

	Mat4* getTransformation() const { return matrix; }

	void updateMatrix(Mat4* newMatrix) { matrix = newMatrix; }
protected:
	Mat4* matrix;
};

class DynamicTransform3D : public Transform3D {
public:
	DynamicTransform3D(Vec3 position = Vec3(0, 0, 0), Vec3 rotation = Vec3(0, 0, 0), Vec3 scale = Vec3(1, 1, 1));
	~DynamicTransform3D();

	Vec3 translation;
	Vec3 rotation;
	Vec3 scale;

	void updateMatrix();
private:
};

class PivotTransform3D : public DynamicTransform3D {
public:
	PivotTransform3D(Vec3 position = Vec3(0, 0, 0), Vec3 rotation = Vec3(0, 0, 0), Vec3 scale = Vec3(1, 1, 1), Vec3 pivot = Vec3(0, 0, 0), Vec3 pivot_rotation = Vec3(0, 0, 0));
	~PivotTransform3D();

	Vec3 pivot;
	Vec3 pivot_rotation;

	void updateMatrix();
private:

};

#endif