
#include "Level.hpp"

#include "../RAL/Log.hpp"

Level::Level(Renderer* renderer, Player* player) 
	: renderer(renderer), player(player)
{
}

Level::~Level() {

}

// Global Level setings
void Level::setClearColor(const Vec3& clearColor) {
	// TODO: Change clear color
}

Point2D* Level::addPoint2D(Point2D* point2D) {
	points2D.push_back(point2D);
	return point2D;
}

Line2D* Level::addLine2D(Line2D* line2D) {
	lines2D.push_back(line2D);
	return line2D;
}

Sprite2D* Level::addSprite2D(Sprite2D* sprite2D) {
	sprites2D.push_back(sprite2D);
	return sprite2D;
}

Point3D* Level::addPoint3D(Point3D* point3D) {
	points3D.push_back(point3D);
	return point3D;
}

Line3D* Level::addLine3D(Line3D* line3D) {
	lines3D.push_back(line3D);
	return line3D;
}

Sprite3D* Level::addSprite3D(Sprite3D* sprite3D) {
	sprites3D.push_back(sprite3D);
	return sprite3D;
}

Mesh2D* Level::addMesh2D(Mesh2D* mesh2D) {
	meshes2D.push_back(mesh2D);
	return mesh2D;
}

Mesh3D* Level::addMesh3D(Mesh3D* mesh3D, bool dynamic) {
	if(dynamic)
		dynamicMeshes3D.push_back(mesh3D);
	else
		staticMeshes3D.push_back(mesh3D);
	if (mesh3D->physics)
		physicsPrimitives.push_back(mesh3D);
	return mesh3D;
}

Mesh3D* Level::addLightedMesh3D(Mesh3D* mesh3D){
	lightedMeshes3D.push_back(mesh3D);
	if(mesh3D->physics)
		physicsPrimitives.push_back(mesh3D);
	return mesh3D;
}

Mesh3D* Level::addDeferredLightedMesh3D(Mesh3D *mesh3D){
	deferredLightedMeshes3D.push_back(mesh3D);
	if(mesh3D->physics)
		physicsPrimitives.push_back(mesh3D);
	return mesh3D;
}

Mesh3D* Level::addAnimatedMesh3D(Mesh3D *mesh3D){
	animatedMeshes3D.push_back(mesh3D);
	if(mesh3D->physics)
		physicsPrimitives.push_back(mesh3D);
	return mesh3D;
}

Mesh3D* Level::addAABB(Mesh3D* aabb) {
	aabbs.push_back(aabb);
	physicsPrimitives.push_back(aabb);
	return aabb;
}

Mesh3D* Level::addSphere(Mesh3D* sphere) {
	spheres.push_back(sphere);
	physicsPrimitives.push_back(sphere);
	return sphere;
}

Mesh3D* Level::addPlane(Mesh3D* plane) {
	planes.push_back(plane);
	physicsPrimitives.push_back(plane);
	return plane;
}

Mesh3D* Level::addOBB(Mesh3D* obb) {
	obbs.push_back(obb);
	physicsPrimitives.push_back(obb);
	return obb;
}

Mesh3D* Level::addRay(Mesh3D* ray) {
	rays.push_back(ray);
	physicsPrimitives.push_back(ray);
	return ray;
}

Mesh3D* Level::addLine(Mesh3D* line) {
	lines.push_back(line);
	physicsPrimitives.push_back(line);
	return line;
}

Point2D* Level::addNamedPoint2D(const std::string& name, Point2D* point2D) {
	points2D.push_back(point2D);
	mapPoints2D.insert(std::pair<std::string, Point2D*>(name, point2D));
	return point2D;
}

Line2D* Level::addNamedLine2D(const std::string& name, Line2D* line2D) {
	lines2D.push_back(line2D);
	mapLines2D.insert(std::pair<std::string, Line2D*>(name, line2D));
	return line2D;
}

Sprite2D* Level::addNamedSprite2D(const std::string& name, Sprite2D* sprite2D) {
	sprites2D.push_back(sprite2D);
	mapSprites2D.insert(std::pair<std::string, Sprite2D*>(name, sprite2D));
	return sprite2D;
}

Point3D* Level::addNamedPoint3D(const std::string& name, Point3D* point3D) {
	points3D.push_back(point3D);
	mapPoints3D.insert(std::pair<std::string, Point3D*>(name, point3D));
	return point3D;
}

Line3D* Level::addNamedLine3D(const std::string& name, Line3D* line3D) {
	lines3D.push_back(line3D);
	mapLines3D.insert(std::pair<std::string, Line3D*>(name, line3D));
	return line3D;
}

Sprite3D* Level::addNamedSprite3D(const std::string& name, Sprite3D* sprite3D) {
	sprites3D.push_back(sprite3D);
	mapSprites3D.insert(std::pair<std::string, Sprite3D*>(name, sprite3D));
	return sprite3D;
}

Mesh2D* Level::addNamedMesh2D(const std::string& name, Mesh2D* mesh2D) {
	meshes2D.push_back(mesh2D);
	mapMeshes2D.insert(std::pair<std::string, Mesh2D*>(name, mesh2D));
	return mesh2D;
}

Mesh3D* Level::addNamedMesh3D(const std::string& name, Mesh3D* mesh3D, bool dynamic) {
	if(dynamic)
		dynamicMeshes3D.push_back(mesh3D);
	else
		staticMeshes3D.push_back(mesh3D);
	mapMeshes3D.insert(std::pair<std::string, Mesh3D*>(name, mesh3D));
	if (mesh3D->physics)
		physicsPrimitives.push_back(mesh3D);
	return mesh3D;
}

Mesh3D* Level::addNamedAABB(const std::string& name, Mesh3D* aabb) {
	physicsPrimitives.push_back(aabb);
	aabbs.push_back(aabb);
	mapAABBs.insert(std::pair<std::string, Mesh3D*>(name, aabb));
	return aabb;
}

Mesh3D* Level::addNamedSphere(const std::string& name, Mesh3D* sphere) {
	physicsPrimitives.push_back(sphere);
	spheres.push_back(sphere);
	mapSpheres.insert(std::pair<std::string, Mesh3D*>(name, sphere));
	return sphere;
}

Mesh3D* Level::addNamedPlane(const std::string& name, Mesh3D* plane) {
	physicsPrimitives.push_back(plane);
	planes.push_back(plane);
	mapPlanes.insert(std::pair<std::string, Mesh3D*>(name, plane));
	return plane;
}

Mesh3D* Level::addNamedOBB(const std::string& name, Mesh3D* obb) {
	physicsPrimitives.push_back(obb);
	obbs.push_back(obb);
	mapOBBs.insert(std::pair<std::string, Mesh3D*>(name, obb));
	return obb;
}

Mesh3D* Level::addNamedRay(const std::string& name, Mesh3D* ray) {
	physicsPrimitives.push_back(ray);
	rays.push_back(ray);
	mapRays.insert(std::pair<std::string, Mesh3D*>(name, ray));
	return ray;
}

Mesh3D* Level::addNamedLine(const std::string& name, Mesh3D* line) {
	physicsPrimitives.push_back(line);
	lines.push_back(line);
	mapLines.insert(std::pair<std::string, Mesh3D*>(name, line));
	return line;
}

// Lights

DirectionalLight* Level::addDirectionalLight(DirectionalLight* light) {
	lights.directionalLights.push_back(light);
	return light;
}

DirectionalLight* Level::addNamedDirectionalLight(const std::string& name, DirectionalLight* light) {
	lights.directionalLights.push_back(light);
	mapDirectionalLights.insert(std::pair<std::string, DirectionalLight*>(name, light));
	return light;
}

PointLight* Level::addPointLight(PointLight* light){
	lights.pointLights.push_back(light);
	return light;
}

PointLight* Level::addNamedPointLight(const std::string &name, PointLight *light){
	lights.pointLights.push_back(light);
	mapPointLights.insert(std::pair<std::string, PointLight*>(name, light));
	return light;
}

SpotLight* Level::addSpotLight(SpotLight* light){
	lights.spotLights.push_back(light);
	return light;
}

SpotLight* Level::addNamedSpotLight(const std::string& name, SpotLight* light){
	lights.spotLights.push_back(light);
	mapSpotLights.insert(std::pair<std::string, SpotLight*>(name, light));
	return light;
}

DirectionalLight* Level::addDeferredDirectionalLight(DirectionalLight *light){
	deferredLights.directionalLights.push_back(light);
	return light;
}

PointLight* Level::addDeferredPointLight(PointLight* light){
	deferredLights.pointLights.push_back(light);
	return light;
}

SpotLight* Level::addDeferredSpotLight(SpotLight *light){
	deferredLights.spotLights.push_back(light);
	return light;
}

Movement3D* Level::addMovement3D(Movement3D* movement) {
	movements3D.push_back(movement);
	return movement;
}

void Level::update() {
	player->input();
	player->update();

	updateLevelTest();

	// Physics collision tests
	for(int i = 0; i < dynamicMeshes3D.size(); ++i){
		Mesh3D* dynamicMesh = dynamicMeshes3D[i];
		for(int j = 0; j < staticMeshes3D.size(); ++j){
			Mesh3D* staticMesh = staticMeshes3D[j];
			ContactManifold manifold;
			dynamicMesh->physics->dynamicCollidesWithStatic(staticMesh, manifold);
			if(manifold.isIntersecting){
				// Resolve interpenetration
				dynamicMesh->addTranslation(manifold.normal * manifold.penetration);
			}
		}
	}

	// Physics update!
	/*
	for (int i = 0; i < physicsPrimitives.size(); ++i) {
		physicsPrimitives[i]->material->colorIndex = 0;
	}
	// Perform Collision Detection in O^2
	for (int i = 0; i < physicsPrimitives.size(); ++i) {
		PhysicsPrimitive* pFirst = physicsPrimitives[i]->physics;
		assert(pFirst);
		for (int j = i + 1; j < physicsPrimitives.size(); ++j) {
			PhysicsPrimitive* pSecond = physicsPrimitives[j]->physics;
			assert(pSecond);
			bool collided = pFirst->collidesWith(pSecond);
			if (collided) {
				physicsPrimitives[i]->material->colorIndex = 1;
				physicsPrimitives[j]->material->colorIndex = 1;
			}
		}
	}
	*/
}

void Level::updateLevelTest() {
	/*
	std::map<std::string, Mesh2D*>::const_iterator it = mapMeshes2D.find("mesh1");
	if (it == mapMeshes2D.end())
		return;
	Mesh2D* mesh1 = it->second;
	DynamicTransform2D* transform = (DynamicTransform2D*)mesh1->getTransform();
	transform->rotation += 1;
	transform->updateMatrix();
	*/

	//Mesh3D* aabb2 = mapAABBs.find("aabb2")->second;
	//AABB* aabb = (AABB*)aabb2->getPhysics();
	//aabb->position -= Vec3(0.01f, 0, 0);

	//Mesh3D* sphere2 = mapSpheres.find("sphere2")->second;
	//Sphere* sphere = (Sphere*)sphere2->getPhysics();
	//sphere->position -= Vec3(0.015f, 0, 0);

	//Mesh3D* meshOBB1 = mapOBBs.find("obb1")->second;
	//OBB* obb1 = (OBB*)meshOBB1->getPhysics();
	//obb1->position += Vec3(0, 0, 0.01f);

	//Mesh3D* meshOBB2 = mapOBBs.find("obb2")->second;
	//OBB* obb2 = (OBB*)meshOBB2->getPhysics();
	//obb2->position -= Vec3(0, 0.0001f, 0);

	for (Movement3D* movement : movements3D) {
		movement->update();
	}
}


void Level::render() const {
	// Render deferred lighted meshes
	renderer->renderDeferredLightedMeshes3D(deferredLightedMeshes3D, deferredLights);

	// Points 3D
	renderer->renderPoints3D(points3D);

	// Lines 3D
	renderer->renderLines3D(lines3D);

	// Sprites 3D
	renderer->renderSprites3D(sprites3D);

	// Meshes 3D
	renderer->renderMeshes3D(staticMeshes3D);
	renderer->renderMeshes3D(dynamicMeshes3D);

	// Render lighted meshes
	renderer->renderLightedMeshes3D(lightedMeshes3D, lights);

	// Render animated meshes
	renderer->renderAnimatedMeshes3D(animatedMeshes3D);

	// Physics Primitives
	renderer->renderAABBs(aabbs);
	renderer->renderSpheres(spheres);
	renderer->renderPlanes(planes);
	renderer->renderOBBs(obbs);

	//renderer->renderRays(rays);
	renderer->renderLines(lines);

	//for (Mesh3D* obb : obbs)
	//	obb->physics->renderAxes(renderer);
	//for (Mesh3D* plane : planes)
	//	plane->physics->renderAxes(renderer);
	//for (Mesh3D* aabb : aabbs)
	//	aabb->physics->renderAxes(renderer);

	// Points 2D
	renderer->renderPoints2D(points2D);

	// Lines 2D
	renderer->renderLines2D(lines2D);

	// Sprites 2D
	renderer->renderSprites2D(sprites2D);

	// Meshes 2D
	renderer->renderMeshes2D(meshes2D);
}
