
#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>
#include <map>

#include "../RAL/buildDesc.hpp"
#include "../RL/RenderPrimitives.hpp"

#include "../RL/Renderer.hpp"

#include "../RL/Camera.hpp"

// Lights
#include "../RL/lights.hpp"

#include "../PL/PhysicsPrimitives.hpp"
#include "../PL/Movement3D.hpp"

struct LightingDescription {
	std::vector<DirectionalLight*> directionalLights;
	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;
};

class Level {
public:
	Level(Renderer* renderer, Camera* camera);
	~Level();

	// Global state functions
	void setClearColor(const Vec3& clearColor);

	Point2D* addPoint2D(Point2D* point2D);
	Line2D* addLine2D(Line2D* line2D);
	Sprite2D* addSprite2D(Sprite2D* sprite2D);
	Point3D* addPoint3D(Point3D* point3D);
	Line3D* addLine3D(Line3D* line3D);
	Sprite3D* addSprite3D(Sprite3D* sprite3D);

	Mesh2D* addMesh2D(Mesh2D* mesh2D);
	Mesh3D* addMesh3D(Mesh3D* mesh3D);
	Mesh3D* addLightedMesh3D(Mesh3D* mesh3D);

	Mesh3D* addAABB(Mesh3D* aabb);
	Mesh3D* addSphere(Mesh3D* sphere);
	Mesh3D* addPlane(Mesh3D* plane);
	Mesh3D* addOBB(Mesh3D* obb);
	Mesh3D* addRay(Mesh3D* ray);
	Mesh3D* addLine(Mesh3D* line);

	Point2D* addNamedPoint2D(const std::string& name, Point2D* point2D);
	Line2D* addNamedLine2D(const std::string& name, Line2D* line2D);
	Sprite2D* addNamedSprite2D(const std::string& name, Sprite2D* sprite2D);
	Point3D* addNamedPoint3D(const std::string& name, Point3D* point3D);
	Line3D* addNamedLine3D(const std::string& name, Line3D* line3D);
	Sprite3D* addNamedSprite3D(const std::string& name, Sprite3D* sprite3D);

	Mesh2D* addNamedMesh2D(const std::string& name, Mesh2D* mesh2D);
	Mesh3D* addNamedMesh3D(const std::string& name, Mesh3D* mesh3D);
	// Mesh3D* addNamedLightedMesh3D(const std::string& name, Mesh3D* mesh3D); // temp ?

	Mesh3D* addNamedAABB(const std::string& name, Mesh3D* aabb);
	Mesh3D* addNamedSphere(const std::string& name, Mesh3D* sphere);
	Mesh3D* addNamedPlane(const std::string& name, Mesh3D* plane);
	Mesh3D* addNamedOBB(const std::string& name, Mesh3D* obb);
	Mesh3D* addNamedRay(const std::string& name, Mesh3D* ray);
	Mesh3D* addNamedLine(const std::string& name, Mesh3D* line);

	DirectionalLight* addDirectionalLight(DirectionalLight* light);
	DirectionalLight* addNamedDirectionalLight(const std::string& name, DirectionalLight* light);
	PointLight* addPointLight(PointLight* light);
	PointLight* addNamedPointLight(const std::string& name, PointLight* light);
	SpotLight* addSpotLight(SpotLight* light);
	SpotLight* addNamedSpotLight(const std::string& name, SpotLight* light);

	Movement3D* addMovement3D(Movement3D* movement);

	void update();
	void render() const;

	// Specific level update functions
	void updateLevelTest();
private:
	// Rendering
	std::vector<Point2D*> points2D;
	std::vector<Line2D*> lines2D;
	std::vector<Sprite2D*> sprites2D;
	std::vector<Point3D*> points3D;
	std::vector<Line3D*> lines3D;
	std::vector<Sprite3D*> sprites3D;

	std::vector<Mesh2D*> meshes2D;
	std::vector<Mesh3D*> meshes3D;
	std::vector<Mesh3D*> lightedMeshes3D;
	
	LightingDescription lights;

	// All primitives in a single container for physics
	std::vector<Mesh3D*> physicsPrimitives;

	// Specific primitive containers for rendering
	std::vector<Mesh3D*> aabbs;
	std::vector<Mesh3D*> spheres;
	std::vector<Mesh3D*> planes;
	std::vector<Mesh3D*> obbs;
	std::vector<Mesh3D*> rays;
	std::vector<Mesh3D*> lines;

	// Debug Movement
	std::vector<Movement3D*> movements3D;

	// Name references
	std::map<std::string, Point2D*> mapPoints2D;
	std::map<std::string, Line2D*> mapLines2D;
	std::map<std::string, Sprite2D*> mapSprites2D;
	std::map<std::string, Point3D*> mapPoints3D;
	std::map<std::string, Line3D*> mapLines3D;
	std::map<std::string, Sprite3D*> mapSprites3D;

	std::map<std::string, Mesh2D*> mapMeshes2D;
	std::map<std::string, Mesh3D*> mapMeshes3D;
	std::map<std::string, Mesh3D*> mapLightedMeshes3D;

	std::map<std::string, DirectionalLight*> mapDirectionalLights;
	std::map<std::string, PointLight*> mapPointLights;
	std::map<std::string, SpotLight*> mapSpotLights;

	std::map<std::string, Mesh3D*> mapAABBs;
	std::map<std::string, Mesh3D*> mapSpheres;
	std::map<std::string, Mesh3D*> mapPlanes;
	std::map<std::string, Mesh3D*> mapOBBs;
	std::map<std::string, Mesh3D*> mapRays;
	std::map<std::string, Mesh3D*> mapLines;

	Renderer* renderer;

	// Cameras
	Camera* camera;
};

#endif
