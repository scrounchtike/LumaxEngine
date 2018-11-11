
#include "Renderer.hpp"

#include "../GL/Level.hpp"

#include "../RAL/RenderingContext.hpp"

#include "../core/ECS.hpp"
#include "../core/RenderComponentStorage.hpp"
#include "../main.hpp"
#include "../lumax.hpp"

#include "Mesh2D.hpp"
#include "Mesh3D.hpp"
#include "GBuffer.hpp"
#include "../GL/Player.hpp"
#include "Shader.hpp"
#include "../RML/ResourceManager.hpp"

Model2D* Renderer::point2D;
Model2D* Renderer::line2D;
Model2D* Renderer::square;

Model3D* Renderer::cube;
Model3D* Renderer::sphere;
Model3D* Renderer::plane;
Model3D* Renderer::line3D;

// Shaders
Shader* Renderer::shader2D;
Shader* Renderer::shader2Dline;
Shader* Renderer::shader3D;
Shader* Renderer::shader3Dline;

// UBOs
// ONLY for OpenGL right now
// TODO: Make abstract class ShaderConstantBuffer for DX11 support
unsigned int Renderer::uboTransform2D;
unsigned int Renderer::uboInstancedTransforms2D;
unsigned int Renderer::uboColor;
unsigned int Renderer::uboInstancedColors;
//unsigned int Renderer::uboTexture;
unsigned int Renderer::uboMVP3D;
unsigned int Renderer::uboInstancedTransforms3D;
unsigned int Renderer::uboLights;
unsigned int Renderer::uboBones;
unsigned int Renderer::uboDirectionalLight;
unsigned int Renderer::uboPointLight;
unsigned int Renderer::uboSpotLight;
unsigned int Renderer::uboLightsVertex;
unsigned int Renderer::uboWater;

std::map<std::string, unsigned> Renderer::mapUBOs;

Renderer::Renderer(Player* player) : camera(getCamera()), player(player) {
	// Player control
	player->registerAxisControl(new InputControl(LMX_KEY_W, 1.0), Player::FORWARD);
	player->registerAxisControl(new InputControl(LMX_KEY_S, -1.0), Player::FORWARD);
	player->registerAxisControl(new InputControl(LMX_KEY_A, -1.0), Player::RIGHT);
	player->registerAxisControl(new InputControl(LMX_KEY_D, 1.0), Player::RIGHT);
	player->registerAxisControl(new InputControl(LMX_KEY_SPACE, 1.0), Player::UP);
	player->registerAxisControl(new InputControl(LMX_KEY_LSHIFT, -1.0), Player::UP);
	
	initializeGeometries();
	initializeShaders();
}

Renderer::~Renderer() {
	// TODO: Clean up
}

// Individual rendering methods
void Renderer::renderPoint2D(const Point2D& point) const {
	shader2D->bind();
	shader2D->setUniformMatrix3f("transform", Mat3().initTranslation(point.position));
	shader2D->setUniform3f("color", point.color);
	shader2D->prepareUniforms();

	lmx::setRenderingTopology(LMX_TRIANGLES);
	square->render();
}

void Renderer::renderLine2D(const Line2D& line) const {
	shader2Dline->bind();
	shader2Dline->setUniform2f("transform", line.A);
	shader2Dline->setUniform2f("end", line.B);
	shader2Dline->setUniform3f("color", line.color);
	shader2Dline->prepareUniforms();

	lmx::setRenderingTopology(LMX_LINES);
	line2D->render();
}

void Renderer::renderSprite2D(const Sprite2D& sprite) const {
	shader2D->bind();
	Mat3 transform = Mat3().initTranslation(sprite.position) * Mat3().initScale(sprite.extents);
	shader2D->setUniformMatrix3f("transform", transform);
	shader2D->setUniform3f("color", sprite.color);
	shader2D->prepareUniforms();

	lmx::setRenderingTopology(LMX_TRIANGLES);
  square->render();
}

void Renderer::renderPoint3D(const Point3D& point) const {
	shader3D->bind();
	shader3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader3D->setUniformMatrix4f("view", camera->getViewMatrix());
	shader3D->setUniformMatrix4f("transform", Mat4().initTranslation(point.position));
	shader3D->setUniform3f("color", point.color);
	shader3D->prepareUniforms();

	lmx::setRenderingTopology(LMX_TRIANGLES);
	cube->render();
}

void Renderer::renderLine3D(const Line3D& line) const {
	shader3Dline->bind();
	shader3Dline->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader3Dline->setUniformMatrix4f("view", camera->getViewMatrix());
	shader3Dline->setUniform3f("start", line.start);
	shader3Dline->setUniform3f("end", line.end);
	shader3Dline->setUniform3f("color", line.color);
	shader3Dline->prepareUniforms();

	lmx::setRenderingTopology(LMX_LINES);
	line3D->render();
}

void Renderer::renderSprite3D(const Sprite3D& sprite) const {
	assert(false);
}

void Renderer::renderAABB(const Mesh3D& aabb) const {
	shader3D->bind();
	shader3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader3D->setUniformMatrix4f("view", camera->getViewMatrix());
	shader3D->setUniformMatrix4f("transform", Mat4().initTranslation(((AABB*)(aabb.physics))->position) * Mat4().initScale(((AABB*)(aabb.physics))->extents));
	aabb.material->setShaderUniforms(shader3D);
	shader3D->prepareUniforms();

	lmx::setRenderingTopology(LMX_TRIANGLES);
	cube->render();
}

void Renderer::renderSphere(const Mesh3D& sphere) const {
	shader3D->bind();
	shader3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader3D->setUniformMatrix4f("view", camera->getViewMatrix());
	shader3D->setUniform3f("position", ((Sphere*)(sphere.physics))->position);
	shader3D->setUniform1f("radius", ((Sphere*)(sphere.physics))->radius);
	sphere.material->setShaderUniforms(shader3D);
	shader3D->prepareUniforms();

	lmx::setRenderingTopology(LMX_TRIANGLES);
	this->sphere->render();
}

void Renderer::renderPlane(const Mesh3D& plane) const {
	// TODO: Fix planes
	/*
	shaderPlane->bind();
	shaderPlane->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderPlane->setUniformMatrix4f("view", camera->getViewMatrix());
	if (plane.material->getTexture())
		shaderPlane->setUniform1f("isTextured", 1.0);
	else
		shaderPlane->setUniform1f("isTextured", 0.0);
	Vec3 bitangent = -cross(((Plane*)plane.physics)->tangent, ((Plane*)plane.physics)->normal);
	Mat4 rotation = Mat4().initSpecialRotation2(((Plane*)plane.physics)->tangent, bitangent, ((Plane*)plane.physics)->normal);
	shaderPlane->setUniformMatrix4f("rotation", rotation);
	shaderPlane->setUniform3f("position", ((Plane*)plane.physics)->position);
	shaderPlane->setUniform1f("scale", ((Plane*)plane.physics)->renderingScale);
	plane.material->setShaderUniforms(shaderPlane);
	shaderPlane->prepareUniforms();

	lmx::setRenderingTopology(LMX_TRIANGLES);
	plane->render();
	*/
}

void Renderer::renderOBB(const Mesh3D& obb) const {
	shader3D->bind();
	shader3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader3D->setUniformMatrix4f("view", camera->getViewMatrix());
	OBB* phys = (OBB*)obb.physics;
	Vec3 u2 = cross(phys->u[0], phys->u[1]);
	Mat4 translation = Mat4().initTranslation(phys->position);
	Mat4 rotation = Mat4().initSpecialRotation2(phys->u[0], phys->u[1], u2);
	Mat4 scale = Mat4().initScale(phys->e);
	Mat4 transform = translation.mul(rotation.mul(scale));
	shader3D->setUniformMatrix4f("transform", transform);
	obb.material->setShaderUniforms(shader3D);
	shader3D->prepareUniforms();

	lmx::setRenderingTopology(LMX_TRIANGLES);
	cube->render();
}

void Renderer::renderRay(const Mesh3D& ray) const {
	shader3Dline->bind();
	shader3Dline->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader3Dline->setUniformMatrix4f("view", camera->getViewMatrix());
	shader3Dline->setUniform3f("start", ((Ray*)ray.physics)->position);
	shader3Dline->setUniform3f("end", ((Ray*)ray.physics)->position + ((Ray*)ray.physics)->direction * 10000.0f);
	ray.material->setShaderUniforms(shader3Dline);
	shader3Dline->prepareUniforms();

	lmx::setRenderingTopology(LMX_LINES);
  line3D->render();
}

void Renderer::renderLine(const Mesh3D& line) const {
	shader3Dline->bind();
	shader3Dline->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shader3Dline->setUniformMatrix4f("view", camera->getViewMatrix());
	shader3Dline->setUniform3f("start", ((Line*)line.physics)->start);
	shader3Dline->setUniform3f("end", ((Line*)line.physics)->end);
	line.material->setShaderUniforms(shader3Dline);
	shader3Dline->prepareUniforms();

	lmx::setRenderingTopology(LMX_LINES);
	line3D->render();
}

Camera* Renderer::getCamera() const {
	return lmx::getStaticCamera();
}

/*
void Renderer::renderGroups3D(const std::vector<PipelineGroup3D*>& groups) const {
	for(PipelineGroup3D* shaderGroup : groups){
		ShaderPipeline* pipeline = shaderGroup->pipeline;
		pipeline->bind();
		if(!shaderGroup->isInstanced){
			// Set constant uniforms
		}else{
			// Set constant uniform buffer instanced variables
		}

		// Check if shader group is lighted
		if(shaderGroup->hasLights){
			// Set lights in Lights UBO
		}
		// Check if shader group has bones
		if(shaderGroup->hasBones){
			// Set bones in Bones UBO
		}

		for(MaterialGroup3D* materialGroup : shaderGroup->materials){
			Material* material = materialGroup->material;
			if(materialGroup->hasTexture){
				// Texture needs to be set in Texture UBO
			}
			if(materialGroup->hasColor){
				// Color needs to be set in Color UBO
			}

			for(GeometryGroup3D* geometryGroup : materialGroup->geometries){
				Model3D* mesh = geometryGroup->mesh;
				// Set transforms UBO
				Mat4 mvp = camera->getProjectionMatrix() * (camera->getViewMatrix()); // + add transform matrix
			}
		}
	}
}
*/

/*
template <typename ECS>
void Renderer::renderGroups2D(const std::vector<PipelineGroup2D<ECS>*>& groups, std::function<void(const std::vector<uint32>&)> callback) const {
	// Prepare for 2D rendering
	lmx::setDepthClip(false);
	
	for(PipelineGroup2D<ECS>* shaderGroup : groups){
		ShaderPipeline* pipeline = shaderGroup->pipeline;

		// Find appropriate system for update of entities' render components
		uint32 entity0 = shaderGroup->materials[0]->geometries[0]->entities[0];
		
		pipeline->bind();
	  if(!shaderGroup->isInstanced){
			// Set constant uniforms
		}else{
			// Set constant uniform buffer instanced variables
		}
		
		for(MaterialGroup2D* materialGroup : shaderGroup->materials){
			Material* material = materialGroup->material;
			if(material->isBlended){
				pipeline->setUniform1f("blend", material->blend); // Normal uniform since blends are rare (could make a special UBO if uses increase)
				// TODO: Better way to upload subroutine uniforms
				pipeline->setSubroutine("getFinalColor", GL_FRAGMENT_SHADER, "getBlendedColor");
				pipeline->updateSubroutines();
			}
			if(material->isTextured){
				// Set texture
				material->getTexture()->bind();
			}
			if(material->isColored){
				// Set color UBO
				glBindBuffer(GL_UNIFORM_BUFFER, uboColor);
				Vec3 color = material->getColor();
				Vec4 color4 = Vec4(color.x, color.y, color.z, 1.0);
				glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, &color4.x, GL_DYNAMIC_DRAW);
			}
			
			for(GeometryGroup2D* geometryGroup : materialGroup->geometries){
				Model2D* mesh = geometryGroup->mesh;
				// Set transform UBO
				if(!geometryGroup->isInstanced){
					// Not instanced
					glBindBuffer(GL_UNIFORM_BUFFER, uboTransform2D);
					
					// TODO: Fix
					// Send transform component into UBO
					// ----
					//glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, (float*)geometryGroup->transform->getTransformation(), GL_DYNAMIC_DRAW);
					
					// ----
					
					mesh->bindForRender();
					
					// Call appropriate render system
					//pipeline->update();
					callback(geometryGroup->entities);
					
					mesh->unbindForRender();
				}else{
					// Instanced
					glBindBuffer(GL_UNIFORM_BUFFER, uboInstancedTransforms2D);
					// TODO: Fix
					//glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4*geometryGroup->instancedTransforms.size(), (float*)&geometryGroup->instancedTransforms[0].x, GL_DYNAMIC_DRAW);
					
					// TODO: Fix
					//mesh->renderInstanced(geometryGroup->instancedTransforms.size());
				}
			}
		}
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
*/

void Renderer::initializeUBOs(){
	// Initialize UBO Lookup table
	mapUBOs.insert(std::pair<std::string, unsigned>("uboTransform2D", 0));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboInstancedTransforms2D", 1));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboColor", 2));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboInstancedColors", 3));
	//mapUBOs.insert(std::pair<std::string, unsigned>("uboTexture", 4));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboMVP3D", 5));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboInstancedTransforms3D", 6));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboLights", 7));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboBones", 8));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboDirectionalLight", 9));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboPointLight", 10));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboSpotLight", 11));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboLightsVertex", 12));
	mapUBOs.insert(std::pair<std::string, unsigned>("uboWater", 13));
	
	// Create UBO buffers
	glGenBuffers(1, &uboTransform2D);
	glBindBuffer(GL_UNIFORM_BUFFER, uboTransform2D);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &uboInstancedTransforms2D);
	glBindBuffer(GL_UNIFORM_BUFFER, uboInstancedTransforms2D);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4*2000, NULL, GL_DYNAMIC_DRAW); // 1 vec4 (position + angle + scale) for 2000 objects

	glGenBuffers(1, &uboColor);
	glBindBuffer(GL_UNIFORM_BUFFER, uboColor);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &uboInstancedColors);
	glBindBuffer(GL_UNIFORM_BUFFER, uboInstancedColors);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*3*2000, NULL, GL_DYNAMIC_DRAW); // vec3 color for 2000 objects

	glGenBuffers(1, &uboMVP3D);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMVP3D);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16*3, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &uboInstancedTransforms3D);
	glBindBuffer(GL_UNIFORM_BUFFER, uboInstancedTransforms3D);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*8*2000, NULL, GL_DYNAMIC_DRAW); // 2 vec4 (position + scale + quaternion) for 2000 objects
	
	struct DirectionalLight{
		Vec3 direction;
		float padding;
		Vec3 color;
		float intensity;
	};
	struct PointLight{
		Vec3 position;
		Vec3 color;
		float constant, linear, exponent;
		Vec3 padding;
	};
	struct SpotLight{
		PointLight pointLight;
		Vec3 direction;
		float cutoff;
	};
	glGenBuffers(1, &uboLights);
	glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight)*1 + sizeof(PointLight)*4 + sizeof(SpotLight)*4, NULL, GL_DYNAMIC_DRAW); // Assuming maximum of 4 lights per type and 1 directional light

	glGenBuffers(1, &uboBones);
	glBindBuffer(GL_UNIFORM_BUFFER, uboBones);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16*64, NULL, GL_DYNAMIC_DRAW); // Assuming maximum of 64 bones (64 mat4 matrices effectively)

	glGenBuffers(1, &uboDirectionalLight);
	glBindBuffer(GL_UNIFORM_BUFFER, uboDirectionalLight);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &uboPointLight);
	glBindBuffer(GL_UNIFORM_BUFFER, uboPointLight);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &uboSpotLight);
	glBindBuffer(GL_UNIFORM_BUFFER, uboSpotLight);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLight), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &uboLightsVertex);
	glBindBuffer(GL_UNIFORM_BUFFER, uboLightsVertex);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight)*4 + sizeof(SpotLight)*4, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &uboWater);
	glBindBuffer(GL_UNIFORM_BUFFER, uboWater);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, NULL, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Bind UBO buffers to a binding point
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboTransform2D);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboInstancedTransforms2D);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboColor);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, uboInstancedColors);
	//glBindBufferBase(GL_UNIFORM_BUFFER, 4, uboTexture);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, uboMVP3D);
	glBindBufferBase(GL_UNIFORM_BUFFER, 6, uboInstancedTransforms3D);
	glBindBufferBase(GL_UNIFORM_BUFFER, 7, uboLights);
	glBindBufferBase(GL_UNIFORM_BUFFER, 8, uboBones);
	glBindBufferBase(GL_UNIFORM_BUFFER, 9, uboDirectionalLight);
	glBindBufferBase(GL_UNIFORM_BUFFER, 10, uboPointLight);
	glBindBufferBase(GL_UNIFORM_BUFFER, 11, uboSpotLight);
	glBindBufferBase(GL_UNIFORM_BUFFER, 12, uboLightsVertex);
	glBindBufferBase(GL_UNIFORM_BUFFER, 13, uboWater);
}

void Renderer::initializeGeometries(){
	// Point2D initialization
	{
		const float sizePoint = 0.025;
		float ar = lmx::getStaticWindowHeight() / lmx::getStaticWindowWidth();
		float offset = sizePoint / 2.0;
		float vertices[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };
		point2D = new Model2D(vertices, 12);
	}

	// Line2D initialization
	{
		float vertices[4] = { 0,0,1,1 };
		line2D = new Model2D(vertices, 4);
	}

	// Square initialization
	{
		float vertices[12] = {
			-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,0.5,0.5,-0.5,-0.5,-0.5
		};
		square = new Model2D(vertices, 12);
	}
	// 3D primitives initialization
	{
		cube = lmx::getStaticResourceManager()->getModel3D("cube.obj");
		sphere = lmx::getStaticResourceManager()->getModel3D("sphere.obj");
		// TODO: Plane
	}
	// Line3D initialization
	{
		float vertices[6] = {
			0,0,0,1,1,1
		};
		line3D = new Model3D(vertices, 6);
	}
}

void Renderer::initializeShaders(){
	shader2D = lmx::getStaticResourceManager()->getShader("renderer/shader2Dcolor");
	shader2Dline = lmx::getStaticResourceManager()->getShader("renderer/shader2Dline");
	shader3D = lmx::getStaticResourceManager()->getShader("renderer/shader3D");
	shader3Dline = lmx::getStaticResourceManager()->getShader("renderer/shader3Dline");
}
