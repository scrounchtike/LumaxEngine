
#include "RendererGL.hpp"

#include "../RAL/Log.hpp"
#include "../main.hpp"

#ifdef _USE_OPENGL

#include <cassert>
#include "../loaders/ShaderLoader.hpp"

RendererGL::RendererGL(Camera* camera) : camera(camera) {
	bool success = initializeGeometries();
	success &= initializeShaders();
	assert(success);
}

RendererGL::~RendererGL() {
	bool success = cleanUpGeometries();
	success &= cleanUpShaders();
	assert(success);
}

/*
* Rendering of individual primitives
*/

void RendererGL::renderPoint2D(const Point2D& point) const {
	shaderPoint2D->bind();
	shaderPoint2D->setUniform2f("position", point.position);
	shaderPoint2D->setUniform3f("color", point.color);

	glBindVertexArray(vaoIDpoint2D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLine2D(const Line2D& line) const {
	shaderLine2D->bind();
	shaderLine2D->setUniform2f("A", line.A);
	shaderLine2D->setUniform2f("B", line.B);
	shaderLine2D->setUniform3f("color", line.color);

	glBindVertexArray(vaoIDline2D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprite2D(const Sprite2D& sprite) const {
	shaderSprite2D->bind();
	shaderSprite2D->setUniform2f("position", sprite.position);
	shaderSprite2D->setUniform2f("extents", sprite.extents);
	shaderSprite2D->setUniform3f("color", sprite.color);

	glBindVertexArray(vaoIDsprite2D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderPoint3D(const Point3D& point) const {
	assert(camera);
	shaderPoint3D->bind();
	shaderPoint3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderPoint3D->setUniformMatrix("view", camera->getViewMatrix());
	shaderPoint3D->setUniform3f("position", point.position);
	shaderPoint3D->setUniform3f("color", point.color);

	glBindVertexArray(vaoIDpoint3D);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDpoint3D);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLine3D(const Line3D& line) const {
	assert(camera);
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix("view", camera->getViewMatrix());
	shaderLine3D->setUniform3f("start", line.start);
	shaderLine3D->setUniform3f("end", line.end);
	shaderLine3D->setUniform3f("color", line.color);

	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprite3D(const Sprite3D& sprite) const {
	assert(false);
}

void RendererGL::renderMesh2D(const Mesh2D& mesh2D) const {
	mesh2D.render();
}

void RendererGL::renderMesh3D(const Mesh3D& mesh3D) const {
	mesh3D.render(camera);
}

void RendererGL::renderAABB(const Mesh3D& aabb) const {
	assert(false);
}

void RendererGL::renderSphere(const Mesh3D& sphere) const {
	assert(false);
}

void RendererGL::renderPlane(const Mesh3D& plane) const {
	assert(false);
}

void RendererGL::renderOBB(const Mesh3D& obb) const {
	assert(false);
}

void RendererGL::renderRay(const Mesh3D& ray) const {
	assert(false);
}

void RendererGL::renderLine(const Mesh3D& line) const {
	assert(false);
}

/* 
* Rendering of primitive vectors 
*/

void RendererGL::renderPoints2D(const std::vector<Point2D*>& points) const {
	shaderPoint2D->bind();
	glBindVertexArray(vaoIDpoint2D);
	glEnableVertexAttribArray(0);
	for (Point2D* point : points) {
		shaderPoint2D->setUniform2f("position", point->position);
		shaderPoint2D->setUniform3f("color", point->color);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLines2D(const std::vector<Line2D*>& lines) const {
	shaderLine2D->bind();
	glBindVertexArray(vaoIDline2D);
	glEnableVertexAttribArray(0);
	for (Line2D* line : lines) {
		shaderLine2D->setUniform2f("A", line->A);
		shaderLine2D->setUniform2f("B", line->B);
		shaderLine2D->setUniform3f("color", line->color);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprites2D(const std::vector<Sprite2D*>& sprites) const {
	shaderSprite2D->bind();
	glBindVertexArray(vaoIDsprite2D);
	glEnableVertexAttribArray(0);
	for (Sprite2D* sprite : sprites) {
		shaderSprite2D->setUniform2f("position", sprite->position);
		shaderSprite2D->setUniform2f("extents", sprite->extents);
		shaderSprite2D->setUniform3f("color", sprite->color);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderPoints3D(const std::vector<Point3D*>& points) const {
	assert(camera);
	//glDisable(GL_CULL_FACE);
	shaderPoint3D->bind();
	shaderPoint3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderPoint3D->setUniformMatrix("view", camera->getViewMatrix());
	glBindVertexArray(vaoIDpoint3D);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDpoint3D);
	for (Point3D* point : points) {
		shaderPoint3D->setUniform3f("position", point->position);
		shaderPoint3D->setUniform3f("color", point->color);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLines3D(const std::vector<Line3D*>& lines) const {
	assert(camera);
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	for (Line3D* line : lines) {
		shaderLine3D->setUniform3f("start", line->start);
		shaderLine3D->setUniform3f("end", line->end);
		shaderLine3D->setUniform3f("color", line->color);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprites3D(const std::vector<Sprite3D*>& sprites) const {
	
}

void RendererGL::renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const {
	if (!meshes2D.size())
		return;
	// NOT WORKING : Could be different gometries &/+ materials &/+ shaders..
	//meshes2D[0]->bindForRender();
	//for (Mesh2D* mesh : meshes2D)
	//	mesh->renderBuffersOnly();
	//meshes2D[0]->unbindForRender();

	for (Mesh2D* mesh : meshes2D) {
		mesh->render();
	}
}

void RendererGL::renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const {
	assert(camera);
	if (!meshes3D.size())
		return;
	for (Mesh3D* mesh : meshes3D) {
		mesh->render(camera);
	}
}

void RendererGL::renderAABBs(const std::vector<Mesh3D*>& aabbs) const {
	shaderAABB3D->bind();
	shaderAABB3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderAABB3D->setUniformMatrix("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDcube3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : aabbs) {
		AABB* aabb = (AABB*)mesh->getPhysics();
		shaderAABB3D->setUniform3f("position", aabb->position);
		shaderAABB3D->setUniform3f("extents", aabb->extents);
		mesh->getMaterial()->setShaderUniforms(shaderAABB3D);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSpheres(const std::vector<Mesh3D*>& spheres) const {
	shaderSphere3D->bind();
	shaderSphere3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderSphere3D->setUniformMatrix("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDsphere3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : spheres) {
		Sphere* sphere = (Sphere*)mesh->getPhysics();
		shaderSphere3D->setUniform3f("position", sphere->position);
		shaderSphere3D->setUniform1f("radius", sphere->radius);
		mesh->getMaterial()->setShaderUniforms(shaderSphere3D);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDsphere3D);
		glDrawElements(GL_TRIANGLES, numIndicesSphere, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderPlanes(const std::vector<Mesh3D*>& planes) const {
	shaderPlane3D->bind();
	shaderPlane3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderPlane3D->setUniformMatrix("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDplane3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : planes) {
		if (mesh->getMaterial()->getTexture()) {
			glEnableVertexAttribArray(1);
			shaderPlane3D->setUniform1f("isTextured", 1.0);
		}
		else {
			shaderPlane3D->setUniform1f("isTextured", 0.0);
		}

		Plane* plane = (Plane*)mesh->getPhysics();
		// Calculate rotation matrix
		Vec3 u = Vec3(0, 0, 1);
		if (u.equals(plane->normal))
			u = Vec3(1, 0, 0);
		Vec3 v1 = cross(plane->normal, u).normalize();
		Vec3 v2 = cross(v1, plane->normal).normalize();

		//Mat4 rotation = Mat4().initRotationTargetNormal(Vec3(0,0,1), plane->normal);
		Vec3 bitangent = -cross(plane->tangent, plane->normal);
		Mat4 rotation = Mat4().initSpecialRotation2(plane->tangent, bitangent, plane->normal);
		//
		shaderPlane3D->setUniformMatrix("rotation", rotation);
		shaderPlane3D->setUniform3f("position", plane->position);
		shaderPlane3D->setUniform1f("scale", plane->renderingScale);
		mesh->getMaterial()->setShaderUniforms(shaderPlane3D);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (mesh->getMaterial()->getTexture())
			glDisableVertexAttribArray(1);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderOBBs(const std::vector<Mesh3D*>& obbs) const {
	shaderOBB3D->bind();
	shaderOBB3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderOBB3D->setUniformMatrix("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDcube3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : obbs) {
		OBB* obb = (OBB*)mesh->getPhysics();
		Vec3 u2 = cross(obb->u[0], obb->u[1]);
		Mat4 translation = Mat4().initTranslation(obb->position);
		Mat4 rotation = Mat4().initSpecialRotation2(obb->u[0], obb->u[1], u2);
		Mat4 scale = Mat4().initScale(obb->e);
		Mat4 transform = translation.mul(rotation.mul(scale));
		shaderOBB3D->setUniformMatrix("transform", transform);

		mesh->getMaterial()->setShaderUniforms(shaderOBB3D);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderRays(const std::vector<Mesh3D*>& rays) const {
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : rays) {
		Ray* ray = (Ray*)mesh->getPhysics();
		shaderLine3D->setUniform3f("start", ray->position);
		shaderLine3D->setUniform3f("end", ray->position + ray->direction * 10000.0f);

		mesh->getMaterial()->setShaderUniforms(shaderLine3D);

		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLines(const std::vector<Mesh3D*>& lines) const {
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix("projection", camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : lines) {
		Line* ray = (Line*)mesh->getPhysics();
		shaderLine3D->setUniform3f("start", ray->start);
		shaderLine3D->setUniform3f("end", ray->end);

		mesh->getMaterial()->setShaderUniforms(shaderLine3D);

		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

bool RendererGL::initializeGeometries() {
	// Point2D initialization
	{
		const float sizePoint = 0.025;
		float ar = getStaticWindowHeight() / getStaticWindowWidth();
		float offset = sizePoint / 2.0f;
		float verticesPoints2D[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };

		glGenVertexArrays(1, &vaoIDpoint2D);
		glBindVertexArray(vaoIDpoint2D);

		glGenBuffers(1, &vboIDpoint2D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDpoint2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPoints2D), verticesPoints2D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	}

	// Line2D initialization
	{
		float verticesLines2D[4] = { 0,0,1,1 };

		glGenVertexArrays(1, &vaoIDline2D);
		glBindVertexArray(vaoIDline2D);

		glGenBuffers(1, &vboIDline2D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDline2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLines2D), verticesLines2D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	}

	// Sprite2D initialization
	{
		const float sizeSprite = 0.5f;
		float ar = getStaticWindowHeight() / getStaticWindowWidth();
		float offset = sizeSprite / 2.0f;
		float verticesSprites2D[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };
		float texCoordsSprites2D[12] = {0,0,0,1,1,1,1,1,1,0,0,0};

		glGenVertexArrays(1, &vaoIDsprite2D);
		glBindVertexArray(vaoIDsprite2D);

		glGenBuffers(1, &vboIDsprite2D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDsprite2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSprites2D), verticesSprites2D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &vbotIDsprite2D);
		glBindBuffer(GL_ARRAY_BUFFER, vbotIDsprite2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordsSprites2D), texCoordsSprites2D, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	}

	// Point3D initialization
	{
		const float pointSize = 0.25f;
		float verticesPoints3D[24] = { -pointSize, pointSize, pointSize,
									    pointSize, pointSize, pointSize,
										pointSize, -pointSize, pointSize,
									   -pointSize, -pointSize, pointSize,
									   -pointSize, pointSize, -pointSize,
										pointSize, pointSize, -pointSize,
										pointSize, -pointSize, -pointSize,
									   -pointSize, -pointSize, -pointSize};
		int indicesPoints3D[36] = {	0,1,2,2,3,0,
									1,5,6,6,2,1,
									4,5,1,1,0,4,
									4,0,3,3,7,4,
									6,7,3,3,2,6,
									5,4,7,7,6,5};

		glGenVertexArrays(1, &vaoIDpoint3D);
		glBindVertexArray(vaoIDpoint3D);

		glGenBuffers(1, &vboIDpoint3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDpoint3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPoints3D), verticesPoints3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &iboIDpoint3D);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDpoint3D);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPoints3D), indicesPoints3D, GL_STATIC_DRAW);
	}

	// Cube3D initialization
	{
		const float pointSize = 1; // 2x2x2 cube
		float verticesCubes3D[24] = { -pointSize, pointSize, pointSize,
			pointSize, pointSize, pointSize,
			pointSize, -pointSize, pointSize,
			-pointSize, -pointSize, pointSize,
			-pointSize, pointSize, -pointSize,
			pointSize, pointSize, -pointSize,
			pointSize, -pointSize, -pointSize,
			-pointSize, -pointSize, -pointSize };
		int indicesCubes3D[36] = { 0,1,2,2,3,0,
			1,5,6,6,2,1,
			4,5,1,1,0,4,
			4,0,3,3,7,4,
			6,7,3,3,2,6,
			5,4,7,7,6,5 };

		glGenVertexArrays(1, &vaoIDcube3D);
		glBindVertexArray(vaoIDcube3D);

		glGenBuffers(1, &vboIDcube3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDcube3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubes3D), verticesCubes3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &iboIDcube3D);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesCubes3D), indicesCubes3D, GL_STATIC_DRAW);
	}

	// Line3D initialization
	{
		float verticesLines3D[] = { 0,0,0,1,1,1 };

		glGenVertexArrays(1, &vaoIDline3D);
		glBindVertexArray(vaoIDline3D);

		glGenBuffers(1, &vboIDline3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDline3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLines3D), verticesLines3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	}

	// Sprite3D initialization
	{

	}

	// Sphere 3D initialization
	{
		float stackCount = 20;
		float sliceCount = 20;
		float radius = 1;

		std::vector<Vec3> vertices;
		std::vector<int> indices;
		std::vector<Vec2> texCoords;
		std::vector<Vec3> normals;
		std::vector<Vec3> tangents;
		vertices.push_back(Vec3(1, 0, 0));
		normals.push_back(Vec3(0, 1, 0));
		tangents.push_back(Vec3(0, radius, 0));
		texCoords.push_back(Vec2(0, 0));

		float phiStep = PI / stackCount;
		float thetaStep = 2.0f*PI / sliceCount;

		for (int i = 1; i <= stackCount - 1; i++) {
			float phi = i*phiStep;
			for (int j = 0; j <= sliceCount; j++) {
				float theta = j*thetaStep;
				Vec3 t = Vec3(
					(radius*sin(phi)*cos(theta)),
					(radius*cos(phi)),
					(radius*sin(phi)*sin(theta))
				);

				Vec3 p = Vec3(-radius*sin(phi)*sin(theta), 0, radius*sin(phi)*cos(theta));
				t.normalize();
				Vec3 n = p;
				n.normalize();
				Vec2 uv = Vec2(theta / (PI * 2), phi / PI);
				vertices.push_back(t), normals.push_back(n), tangents.push_back(p), texCoords.push_back(uv);
			}
		}

		vertices.push_back(Vec3(1, 0, 0));
		normals.push_back(Vec3(0, -1, 0));
		tangents.push_back(Vec3(0, -radius, 0));
		texCoords.push_back(Vec2(0, 1));


		for (int i = 1; i <= sliceCount; i++) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(0);
		}
		float baseIndex = 1;
		float ringVertexCount = sliceCount + 1;
		for (int i = 0; i < stackCount - 2; i++) {
			for (int j = 0; j < sliceCount; j++) {
				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
				indices.push_back(baseIndex + i*ringVertexCount + j + 1);
				indices.push_back(baseIndex + i*ringVertexCount + j);

				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
				indices.push_back(baseIndex + i*ringVertexCount + j + 1);
				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			}
		}
		float southPoleIndex = vertices.size() - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (int i = 0; i < sliceCount; i++) {
			indices.push_back(baseIndex + i + 1);
			indices.push_back(baseIndex + i);
			indices.push_back(southPoleIndex);
		}

		numIndicesSphere = indices.size();

		glGenVertexArrays(1, &vaoIDsphere3D);
		glBindVertexArray(vaoIDsphere3D);

		glGenBuffers(1, &vboIDsphere3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDsphere3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &vbotIDsphere3D);
		glBindBuffer(GL_ARRAY_BUFFER, vbotIDsphere3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords.size() * 2, &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &iboIDsphere3D);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDsphere3D);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Plane 3D initialization
	{
		float verticesPlanes3D[18] = { -0.5f,-0.5f,0,-0.5f,0.5f,0,0.5f,0.5f,0,0.5f,0.5f,0,0.5f,-0.5f,0,-0.5f,-0.5f,0 };
		float texCoordsPlanes3D[12] = { 0,0,0,1,1,1,1,1,1,0,0,0 };

		glGenVertexArrays(1, &vaoIDplane3D);
		glBindVertexArray(vaoIDplane3D);

		glGenBuffers(1, &vboIDplane3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDplane3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlanes3D), verticesPlanes3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &vbotIDplane3D);
		glBindBuffer(GL_ARRAY_BUFFER, vbotIDplane3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordsPlanes3D), texCoordsPlanes3D, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	}

	// Unbind buffers
	glBindVertexArray(0);

	return true;
}

bool RendererGL::initializeShaders() {
	shaderPoint2D = ShaderLoader::loadShaderGL("shaders/GLSL/renderer/shader2Dpoint");
	shaderLine2D = ShaderLoader::loadShaderGL("shaders/GLSL/renderer/shader2Dline");
	shaderSprite2D = ShaderLoader::loadShaderGL("shaders/GLSL/renderer/shader2Dsprite");

	shaderPoint3D = ShaderLoader::loadShaderGL("shaders/GLSL/renderer/shader3Dpoint");
	shaderLine3D = ShaderLoader::loadShaderGL("shaders/GLSL/renderer/shader3Dline");
	//shaderSprite3D = ShaderLoader::loadShaderGL("shaders/GLSL/renderer/shader3Dsprite");

	shaderAABB3D = ShaderLoader::loadShaderGL("shaders/GLSL/physics/shader3DAABB");
	shaderSphere3D = ShaderLoader::loadShaderGL("shaders/GLSL/physics/shader3Dsphere");
	shaderPlane3D = ShaderLoader::loadShaderGL("shaders/GLSL/physics/shader3Dplane");
	shaderOBB3D = ShaderLoader::loadShaderGL("shaders/GLSL/physics/shader3DOBB");

	return true;
}

bool RendererGL::cleanUpGeometries() {
	// TODO: destroy OpenGL buffers

	return true;
}

bool RendererGL::cleanUpShaders() {
	delete shaderPoint2D;
	delete shaderLine2D;
	delete shaderSprite2D;

	return true;
}

#endif