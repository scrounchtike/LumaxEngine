
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "RenderPrimitives.hpp"
#include "../RAL/buildDesc.hpp"

#include "Mesh2D.hpp"
#include "Mesh3D.hpp"

#include "../GL/Player.hpp"
#include "Shader.hpp"
#include "ShaderPipeline.hpp"

#include <vector>
#include <map>
#include <functional>

#include "../RAL/RenderingContext.hpp"
#include "../core/RenderComponentStorage.hpp"

#include "../math/types.hpp"

#include "GBuffer.hpp"

template <typename T>
struct PipelineGroup2D;
template <typename T>
struct PipelineGroup3D;

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

	void input(){
		player->input();
	}
	void update(){
		player->update();
	}

	Camera* getCamera() const{
		return camera;
	}

	// Group rendering methods
	template <typename ECS>
	void renderGroups3D(const std::vector<PipelineGroup3D<ECS>*>& groups, std::function<void(const std::vector<uint32>&)> callback) const{
		lmx::setDepthClip(true);
		lmx::setFaceCulling(false);

		// TEMPORARY
		camera->update();

		for(PipelineGroup3D<ECS>* shaderGroup : groups){
			ShaderPipeline* pipeline = shaderGroup->pipeline;
			pipeline->bind();
			// Set shader constant uniform variables
			for(MaterialGroup3D* materialGroup : shaderGroup->materials){
				Material* material = materialGroup->material;
				// Set Material constant uniform variables
				
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
				

				for(GeometryGroup3D* geometryGroup : materialGroup->geometries){
					Model3D* mesh = geometryGroup->mesh;
					if(!geometryGroup->isInstanced){
						glBindBuffer(GL_UNIFORM_BUFFER, uboMVP3D);
						
						// Bind projection and view matrices
						glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
						glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
						
						mesh->bindForRender();
						
						callback(geometryGroup->entities);
						
						mesh->unbindForRender();
						
					}
				}
			}
		}
	}
	//template <typename Manager, typename... Ts>
	template <typename ECS>
	void renderGroups2D(const std::vector<PipelineGroup2D<ECS>*>& groups, std::function<void(const std::vector<uint32>&)> callback) const{
		// Prepare for 2D rendering
		lmx::setDepthClip(false);

		std::cout << "renderer render" << std::endl;
		for(PipelineGroup2D<ECS>* shaderGroup : groups){
			ShaderPipeline* pipeline = shaderGroup->pipeline;
			std::cout << "found a pipeline" << std::endl;
			// Find appropriate system for update of entities' render components
			
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
						//glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, (float*)&(Mat4().initIdentity()), GL_DYNAMIC_DRAW);
						//mesh->renderBuffersOnly();
						
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
	Player* player;
};

#endif
