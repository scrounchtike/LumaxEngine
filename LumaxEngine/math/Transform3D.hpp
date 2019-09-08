
#ifndef TRANSFORM_3D_HPP
#define TRANSFORM_3D_HPP

#include "../math.hpp"

class StaticTransform3D {
public:
	StaticTransform3D(Vec3 position = Vec3(0, 0, 0), Vec3 rotation = Vec3(0, 0, 0), Vec3 scale = Vec3(1, 1, 1), Vec3 pivot = Vec3(0, 0, 0), Vec3 pivot_rotation = Vec3(0, 0, 0));
	~StaticTransform3D();

	Mat4* getTransformation() const { return matrix; }

	void updateMatrix(Mat4* newMatrix) { matrix = newMatrix; }

	Vec3 getTranslation() const
	{
		return matrix->getTranslationComponent();
	}
	
protected:
	Mat4* matrix;
};

class DynamicTransform3D {
public:
	DynamicTransform3D(Vec3 position = Vec3(0, 0, 0), Vec3 rotation = Vec3(0, 0, 0), Vec3 scale = Vec3(1, 1, 1));
	~DynamicTransform3D();

	Vec3 translation;
	Vec3 rotation;
	Vec3 scale;

	void updateMatrix();
	Mat4* getTransformation() const { return matrix; }
	void updateMatrix(const Mat4& newMatrix) { *matrix = newMatrix; }

	Vec3 getTranslation() const
	{
		return matrix->getTranslationComponent();
	}
protected:
	Mat4* matrix;
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

using Transform3D = DynamicTransform3D;

#endif
