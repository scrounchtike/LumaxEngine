
#include "Transform3D.hpp"

Transform3D::Transform3D(Vec3 position, Vec3 rotation, Vec3 scale, Vec3 pivot, Vec3 pivot_rotation) {
	matrix = new Mat4();
	*matrix = Mat4().initTranslation(position) * Mat4().initRotationDegrees(rotation) * Mat4().initScale(scale);
	//*matrix = Matrix4f::mul(Mat4().initTranslation(position), Mat4().initRotation(rotation));
	if (!pivot_rotation.isNull()) {
		// TODO: Add pivot rotation
	}
}

Transform3D::~Transform3D() {
	delete matrix;
}

DynamicTransform3D::DynamicTransform3D(Vec3 position, Vec3 rotation, Vec3 scale) : translation(translation), rotation(rotation), scale(scale) {
	*matrix = Mat4().initTranslation(position) * Mat4().initRotationDegrees(rotation) * Mat4().initScale(scale);
}

DynamicTransform3D::~DynamicTransform3D() {
	delete matrix;
}

PivotTransform3D::PivotTransform3D(Vec3 position, Vec3 rotation, Vec3 scale, Vec3 pivot, Vec3 pivot_rotation) : pivot(pivot), pivot_rotation(pivot_rotation) {
	*matrix = Mat4().initTranslation(position) * Mat4().initRotationDegrees(rotation) * Mat4().initScale(scale);
	if (!pivot_rotation.isNull()) {
		// Add pivot rotation
	}
	this->translation = position;
	this->rotation = rotation;
	this->scale = scale;
}

PivotTransform3D::~PivotTransform3D() {
	delete matrix;
}

void DynamicTransform3D::updateMatrix() {
	*matrix = Mat4().initTranslation(translation) * Mat4().initRotationDegrees(rotation) * Mat4().initScale(scale);
}

void PivotTransform3D::updateMatrix() {
	*matrix = Mat4().initTranslation(translation) * Mat4().initRotationDegrees(rotation) * Mat4().initScale(scale);
	// TODO: Add pivot rotation
}