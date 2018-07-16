
#include "Transform2D.hpp"

#include "../main.hpp"

Transform2D::Transform2D(Vec2 position, float rotation, Vec2 scale, Vec2 pivot, float pivot_rotation) {
	static float ar = getStaticWindowHeight() / getStaticWindowWidth();
	matrix = new Mat4();
	*matrix = Mat4().initTranslation(position.x, position.y, 0.0) * Mat4().initSkewedRotationZ(rotation * PI / 180.0f, ar) * Mat4().initScale(scale.x, scale.y, 1);
	if (pivot_rotation) {
		// TODO: Add pivot rotation
	}
}

Transform2D::~Transform2D() {
	delete matrix;
}

DynamicTransform2D::DynamicTransform2D(Vec2 position, float rotation, Vec2 scale) : translation(position), rotation(rotation), scale(scale){
	static float ar = getStaticWindowHeight() / getStaticWindowWidth();
	*matrix = Mat4().initTranslation(position.x, position.y, 0.0) * Mat4().initSkewedRotationZ(rotation * PI / 180.0f, ar) * Mat4().initScale(scale.x, scale.y, 1);
}

DynamicTransform2D::~DynamicTransform2D() {
	delete matrix;
}

PivotTransform2D::PivotTransform2D(Vec2 position, float rotation, Vec2 scale, Vec2 pivot, float pivot_rotation) : pivot(pivot), pivot_rotation(pivot_rotation) {
	static float ar = getStaticWindowHeight() / getStaticWindowWidth();
	*matrix = Mat4().initTranslation(position.x, position.y, 0.0) * Mat4().initSkewedRotationZ(rotation * PI / 180.0f, ar) * Mat4().initScale(scale.x, scale.y, 1);
	if (pivot_rotation) {
		// TODO: Add pivot rotation
	}
	this->translation = position;
	this->rotation = rotation;
	this->scale = scale;
}

PivotTransform2D::~PivotTransform2D() {
	delete matrix;
}

void DynamicTransform2D::updateMatrix() {
	static float ar = getStaticWindowHeight() / getStaticWindowWidth();
	*matrix = Mat4().initTranslation(translation.x, translation.y, 0.0) * Mat4().initSkewedRotationZ(rotation * PI / 180.0f, ar) * Mat4().initScale(scale.x, scale.y, 1);
}

void PivotTransform2D::updateMatrix() {
	static float ar = getStaticWindowHeight() / getStaticWindowWidth();
	*matrix = Mat4().initTranslation(translation.x, translation.y, 0.0) * Mat4().initSkewedRotationZ(rotation * PI / 180.0f, ar) * Mat4().initScale(scale.x, scale.y, 1);
}
