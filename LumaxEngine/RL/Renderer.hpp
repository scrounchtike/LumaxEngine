
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "RenderPrimitives.hpp"
#include "../RAL/buildDesc.hpp"
#include "../main.hpp"

#include "Mesh2D.hpp"
#include "Mesh3D.hpp"

#include "../GL/Player.hpp"
#include "Shader.hpp"
#include "ShaderPipeline.hpp"

#include <vector>
#include <map>

class GeometryGroup3D {
public:
	GeometryGroup3D(Model3D* mesh) : mesh(mesh) { }
	Model3D* mesh;
	bool isInstanced = false;
	
	unsigned int getGeometryID(){
		return mesh->getGeometryID();
	}
	//std::vector<Transform3D*> transforms;
};

class MaterialGroup3D {
public:
	MaterialGroup3D(Material* material) : material(material) { }
	Material* material;
	bool hasTexture = false;
	bool hasColor = false;
	
	std::vector<GeometryGroup3D*> geometries;
};

class PipelineGroup3D {
public:
	PipelineGroup3D(ShaderPipeline* pipeline) : pipeline(pipeline) { }
	ShaderPipeline* pipeline = nullptr;
	bool isInstanced = false;
	bool hasLights = false;
	bool hasBones = false;
	
	std::vector<MaterialGroup3D*> materials;
};

class GeometryGroup2D {
public:
	GeometryGroup2D(Model2D* mesh) : mesh(mesh) { }
	Model2D* mesh;
	bool isInstanced = false;

	unsigned int getGeometryID(){
		return mesh->getGeometryID();
	}
	Transform2D* transform;
	std::vector<Vec4> instancedTransforms;
};

class MaterialGroup2D {
public:
	MaterialGroup2D(Material* material) : material(material) { }
	Material* material;
	
	std::vector<GeometryGroup2D*> geometries;
};

class PipelineGroup2D {
public:
	PipelineGroup2D(ShaderPipeline* pipeline) : pipeline(pipeline) { }
	ShaderPipeline* pipeline = nullptr;
	bool isInstanced = false;

	std::vector<MaterialGroup2D*> materials;
};

struct LightingDescription;

class Renderer {
public:
	Renderer(Player* player);
	~Renderer();

	// Individual rendering methods
	void renderPoint2D(const Point2D& point) const;
	void renderLine2D(const Line2D& line) const;
	void renderSprite2D(const Sprite2D& model) const;

	void renderPoint3D(const Point3D& point) const;
	void renderLine3D(const Line3D& line) const;
	void renderSprite3D(const Sprite3D& sprite) const;

	void renderAABB(const Mesh3D& aabb) const;
	void renderSphere(const Mesh3D& sphere) const;
	void renderPlane(const Mesh3D& plane) const;
	void renderOBB(const Mesh3D& obb) const;
	void renderRay(const Mesh3D& ray) const;
	void renderLine(const Mesh3D& line) const;

	// Group rendering methods
	void renderGroups3D(const std::vector<PipelineGroup3D*>& groups) const;
	void renderGroups2D(const std::vector<PipelineGroup2D*>& groups) const;

	static void initializeUBOs();
	static void initializeGeometries();
	static void initializeShaders();
	
	// Geometry
	static Model2D* point2D;
	static Model2D* line2D;
	static Model2D* square;
	
	static Model3D* cube;
	static Model3D* sphere;
	static Model3D* plane;
	static Model3D* line3D;
	
	// Shaders
	static Shader* shader2D;
	static Shader* shader2Dline;
	static Shader* shader3D;
	static Shader* shader3Dline;

	// UBOs
	// ONLY for OpenGL right now
	// TODO: Make abstract class ShaderConstantBuffer for DX11 support
	static unsigned int uboTransform2D;
	static unsigned int uboInstancedTransforms2D;
	static unsigned int uboColor;
	static unsigned int uboInstancedColors;
	//static unsigned int uboTexture;
	static unsigned int uboMVP3D;
	static unsigned int uboInstancedTransforms3D;
	static unsigned int uboLights;
	static unsigned int uboBones;

	static std::map<std::string, unsigned> mapUBOs;
private:
	Camera* camera;
};

#endif
