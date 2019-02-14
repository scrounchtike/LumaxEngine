
#ifndef RENDER_SYSTEMS_HPP
#define RENDER_SYSTEMS_HPP

#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"

#include "../CL/LightComponent.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "GBuffer.hpp"

// Rendering Systems
template <typename ECS, typename... Ts>
class RenderingSystem;
template <typename ECS, typename... Ts>
class System;

/*

template <typename ECS>
struct RenderSystemTransform2D : public RenderingSystem<ECS, TYPE_2D, Transform2D, Model2D>{
	static void update(Transform2D* transform, Model2D* mesh){
		std::cout << "render mesh" << std::endl;
		//glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, (float*)&(Mat4().initIdentity()), GL_DYNAMIC_DRAW);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, (float*)(transform->getTransformation()), GL_DYNAMIC_DRAW);
		mesh->renderBuffersOnly();
	}
};
template <typename ECS>
struct RenderSystemDynamicTransform2D : public RenderingSystem<ECS, TYPE_2D, DynamicTransform2D, Model2D>{
	static void update(DynamicTransform2D* transform, Model2D* mesh){
		std::cout << "render mesh" << std::endl;
	  glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, (float*)(transform->getTransformation()), GL_DYNAMIC_DRAW);
		mesh->renderBuffersOnly();
	}
};
template <typename ECS>
struct RenderSystemPivotTransform2D : public RenderingSystem<ECS, TYPE_2D, PivotTransform2D, Model2D>{
	static void update(PivotTransform2D* transform, Model2D* mesh){
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, transform->getTransformation()->getHeadPointer(), GL_DYNAMIC_DRAW);
		mesh->renderBuffersOnly();
	}
};
*/

template <typename E>
struct UnityForwardRender{
	// Static data that both systems share
	static bool initialized;
	static bool newFrame;

	static ShaderPipeline* basePipeline;
	static ShaderPipeline* perpixelLightPipeline;

	static DirectionalLight dlight;
	
	// Functions that both systems share
	static void impl_construct(){
		initialized = true;
		
		basePipeline = lmx::getStaticResourceManager()->getShaderPipeline("renderer/shader3DunityBase");
		//perpixelLightPipeline = lmx::getStaticResourceManager()->getShaderPipeline("renderer/shader3DunityPerpixelLight");

		// Dir light for testing (very temp)
		dlight = DirectionalLight(Vec3(-1,1,-1).normalize(), Vec3(1,0.6f,0.6f), 0.7f);

		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboDirectionalLight);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight), &dlight, GL_DYNAMIC_DRAW);
	}
	
	static void impl_update(const std::vector<Material*>& materials,
										 const std::vector<Model3D*>& meshes,
										 const std::vector<Transform3D*>& transforms,
										 const std::vector<LightComponent*>& lightDescs,
										 Camera* camera)
	{
		newFrame = true;
		
		static uint32 materialID = (uint32)(-1);
		static uint32 modelID = (uint32)(-1);
		
		// First, we begin with the base pass for directional light and vertex / baked lights
		// no baked lights for now though
		ShaderPipeline* pipeline = basePipeline;

		// We bind the base pass pipeline and set the camera uniforms
		pipeline->bind();
		// Update projection and view matrices
		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
		// Update camera position
		pipeline->setUniform3f("cameraPos", camera->getPosition());
		
		// IMPORTANT !!
		// This assumes that the meshes have vertex, texcoord and normal information! 

		// Loop over all entities and forward draw with first base pipeline
		for(int i = 0; i < meshes.size(); ++i){
			Material* material = materials[i];
			Model3D* mesh = meshes[i];
			Transform3D* transform = transforms[i];
			LightComponent* lights = lightDescs[i];
			
			// Material Settings
			// This sets the subroutines necessary for texturing and/or
			// coloring
			if(materialID != material->ID || newFrame){
			//if(newFrame){
				newFrame = false;
				materialID = material->ID;
				if(material->isTextured){
					// Set texture
					glActiveTexture(GL_TEXTURE0);
					material->getTexture()->bind();
					pipeline->setSubroutine("getAlbedo", GL_FRAGMENT_SHADER, "getTextureColor");
				}
				if(material->isColored){
					// Set color UBO
					glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboColor);
					Vec3 color = material->getColor();
					Vec4 color4 = Vec4(color.x, color.y, color.z, 1.0);
					glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, &color4.x, GL_DYNAMIC_DRAW);
					pipeline->setSubroutine("getAlbedo", GL_FRAGMENT_SHADER, "getColor");
				}
				if(material->isBlended){
					// Cant really blend if we are missing a blend source
					assert(material->isTextured && material->isColored);
					pipeline->setUniform1f("blend", material->blend); 
					pipeline->setSubroutine("getAlbedo", GL_FRAGMENT_SHADER, "getBlendedColor");
				}
				pipeline->updateSubroutines();
			}

			// Light information
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboLightsVertex);
		  glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight)*4 + sizeof(SpotLight)*4, &(lights->vertexPointLights[0]), GL_DYNAMIC_DRAW);
			
			// Mesh settings
			if(modelID != mesh->ID){
				modelID = mesh->ID;
				mesh->bindForRender();
			}
			
			// Base Pass Render
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
			mesh->renderBuffersOnly();
			
			// Turn on additive blending
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_ONE, GL_ONE);
			//lmx::setDepthClip(false);
			
			// For all point lights and spot lights, render the mesh with additive blending enabled
			
			// Point Lights
			/*
			pointLightPipeline->bind();
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
			// For each light
			for(int p = 0; p < 4; ++p){
				if(!lights->pointLights[p])
					continue;
				// For each light, bind appropriate light uniforms
				glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboPointLight);
				glBufferData(GL_UNIFORM_BUFFER, sizeof(Vec3)*4, lights->pointLights[p], GL_DYNAMIC_DRAW);
				mesh->renderBuffersOnly();
			}

			// Spot Lights
			for(int s = 0; s < 4; ++s){
				if(!lights->spotLights[s])
					continue;
				mesh->renderBuffersOnly();
			}
			*/
			

			// Turn off additive blending
			//glDisable(GL_BLEND);
			//lmx::setDepthClip(true);
		}
	}
		
	template <typename ECS>
	struct RenderingSystemTransform3DUnity : public RenderingSystem<ECS, Material, Model3D, Transform3D, LightComponent, ForwardRender>
	{
		static void construct(){
			if(!initialized)
				impl_construct();
		}
		static void update(const std::vector<Material*>& materials,
											 const std::vector<Model3D*>& meshes,
											 const std::vector<Transform3D*>& transforms,
											 const std::vector<LightComponent*>& lightDescs,
											 Camera* camera)
		{
			impl_update(materials, meshes, transforms, lightDescs, camera);
		}
	};
	
	template <typename ECS>
	struct RenderingSystemDynamicTransform3DUnity : public RenderingSystem<ECS, Material, Model3D, DynamicTransform3D, LightComponent, ForwardRender>
	{
		static void construct(){
			if(!initialized)
				impl_construct();
		}
		static void update(const std::vector<Material*>& materials,
											 const std::vector<Model3D*>& meshes,
											 const std::vector<DynamicTransform3D*>& transforms,
											 const std::vector<LightComponent*>& lightDescs,
											 Camera* camera)
		{
			impl_update(materials, meshes, std::vector<Transform3D*>(transforms.begin(), transforms.end()), lightDescs, camera);
		}
	};
};

template <typename ECS>
bool UnityForwardRender<ECS>::newFrame = false;
template <typename ECS>
bool UnityForwardRender<ECS>::initialized = false;

template <typename ECS>
ShaderPipeline* UnityForwardRender<ECS>::basePipeline;
template <typename ECS>
ShaderPipeline* UnityForwardRender<ECS>::perpixelLightPipeline;

template <typename ECS>
DirectionalLight UnityForwardRender<ECS>::dlight;

// 3D
// Static entities rendering
template <typename ECS>
struct RenderSystemTransform3D : public RenderingSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, ForwardRender>{

	static bool newFrame;
	
	static void update(const std::vector<ShaderPipeline*>& pipelines, const std::vector<Material*>& materials, const std::vector<Model3D*>& meshes, const std::vector<Transform3D*>& transforms, Camera* camera){
		//std::cout << "forward rendering " << transforms.size() << " entities" << std::endl;
		// Reset new frame hint
		newFrame = true;
		// Forward render entities
		for(int i = 0; i < pipelines.size(); ++i){
			impl(pipelines[i], materials[i], meshes[i], transforms[i], camera);
		}
		// For each directional light

		// For each point light

		// For each spot light
	}
	
	static void impl(ShaderPipeline* pipeline, Material* material, Model3D* mesh, Transform3D* transform, Camera* camera){
		assert(pipeline);
		assert(material);
		assert(mesh);
		assert(transform);
		assert(camera);
		
		static uint32 pipelineID = (uint32)(-1);
		static uint32 materialID = (uint32)(-1);
		static uint32 modelID = (uint32)(-1);
	
		// Switch shader pipeline if necessary
		
		if(pipelineID != pipeline->ID || newFrame){
			newFrame = false;
			pipelineID = pipeline->ID;
			pipeline->bind();
			// Set per pipeline uniformsi
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
			// Bind projection and view matrices
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
		}

		// Update material / texture state if necessary
	
		if(materialID != material->ID){
			materialID  = material->ID;
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
				glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboColor);
				Vec3 color = material->getColor();
				Vec4 color4 = Vec4(color.x, color.y, color.z, 1.0);
				glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, &color4.x, GL_DYNAMIC_DRAW);
			}
		}

		// Bind mesh if necessary
		
		if(modelID != mesh->ID){
			modelID = mesh->ID;
			mesh->bindForRender();
		}

		// Update per entity uniforms
		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
		mesh->renderBuffersOnly();
	}
};
template <typename E>
bool RenderSystemTransform3D<E>::newFrame = false;

template <typename ECS>
struct DeferredRenderSystemTransform3D : public RenderingSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, DeferredRender>{
	static GBuffer gBuffer;
	static Model2D* quad;
	static ShaderPipeline* deferredPipeline;
	static void construct(){
	  // Initialize required geometry, shaders, etc..
		float vertices[12] = {
			-1,-1,-1,1,1,1,1,1,1,-1,-1,-1
		};
		float texCoords[12] = {
			0,0,0,1,1,1,1,1,1,0,0,0
		};
		quad = new Model2D(vertices, 6, texCoords);

		Shader* deferredShader = lmx::getStaticResourceManager()->getShader("renderer/deferredShader");
		deferredPipeline = new ShaderPipeline(deferredShader);
	}

	static void update(const std::vector<ShaderPipeline*>& pipelines, const std::vector<Material*>& materials, const std::vector<Model3D*>& meshes, const std::vector<Transform3D*>& transforms, Camera* camera){
		// GBuffer Pass 
		gBuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for(int i = 0; i < pipelines.size(); ++i){
			//impl(pipelines[i], materials[i], meshes[i], transforms[i], camera);
		}
		gBuffer.unbind();
		gBuffer.copyDepth();

		// Deferred Pass
		deferredPipeline->bind();

		glActiveTexture(GL_TEXTURE3);
		//gBuffer->bindDepthTexture();
		glActiveTexture(GL_TEXTURE2);
		//gBuffer->bindAlbedoTexture();
		glActiveTexture(GL_TEXTURE1);
		//gBuffer->bindNormalTexture();
		glActiveTexture(GL_TEXTURE0);
		//gBuffer->bindPositionTexture();

		deferredPipeline->setUniform1i("depth", 3);
		deferredPipeline->setUniform1i("albedo", 2);
		deferredPipeline->setUniform1i("normal", 1);
		deferredPipeline->setUniform1i("position", 0);
	
		quad->render();
	}
	
	typedef std::multimap<uint64, uint32>::iterator Iterator;
	static void dispatch(Iterator begin, Iterator end){
		
	}
};
template <typename ECS>
GBuffer DeferredRenderSystemTransform3D<ECS>::gBuffer;
template <typename ECS>
Model2D* DeferredRenderSystemTransform3D<ECS>::quad;
template <typename ECS>
ShaderPipeline* DeferredRenderSystemTransform3D<ECS>::deferredPipeline;

template <typename ECS>
struct RenderSystemWolf : public RenderingSystem<ECS, TYPE_3D, ShaderPipeline, Material, Model3D, DynamicTransform3D>{
	
	static bool newFrame;
	
	static void update(const std::vector<ShaderPipeline*>& pipelines, const std::vector<Material*>& materials, const std::vector<Model3D*>& models, const std::vector<DynamicTransform3D*>& transforms, Camera* camera){
		newFrame = true;
		
		static uint32 pipelineID = (uint32)(-1);
		static uint32 materialID = (uint32)(-1);
		static uint32 modelID = (uint32)(-1);
		
		for(int i = 0; i < pipelines.size(); ++i){
			ShaderPipeline* pipeline = pipelines[i];
			Material* material = materials[i];
			Model3D* mesh = models[i];
			DynamicTransform3D* transform = transforms[i];
			
			// Switch shader pipeline if necessary
			if(pipelineID != pipeline->ID || newFrame){
				pipelineID = pipeline->ID;
				pipeline->bind();
				// Set per pipeline uniforms
				glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
				// Bind projection and view matrices
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
			}
			
			// Update material / texture state if necessary
			
			if(materialID != material->ID || newFrame){
				//std::cout << "material" << std::endl;
				materialID  = material->ID;
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
					glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboColor);
					Vec3 color = material->getColor();
					Vec4 color4 = Vec4(color.x, color.y, color.z, 1.0);
					glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, &color4.x, GL_DYNAMIC_DRAW);
				}
			}
			
			// Bind mesh if necessary
			if(modelID != mesh->ID || newFrame){
				modelID = mesh->ID;
				mesh->bindForRender();
			}
			
			// Update per entity uniforms
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
			mesh->renderBuffersOnly();
			
			if(newFrame)
				newFrame = false;
		}
	}
};
template <typename E>
bool RenderSystemWolf<E>::newFrame = false;

/*
struct FlagRender1 {};
template <typename ECS>
struct RenderSystemDynamicTransform3D : public RenderingSystem<ECS, TYPE_3D, DynamicTransform3D, Model3D>{
	static void update(DynamicTransform3D* transform, Model3D* mesh, Camera* camera){
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
		mesh->renderBuffersOnly();
	}
};
template <typename ECS>
struct RenderSystemPivotTransform3D : public RenderingSystem<ECS, TYPE_3D, PivotTransform3D, Model3D>{
	static void update(PivotTransform3D* transform, Model3D* mesh, Camera* camera){
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
		mesh->renderBuffersOnly();
	}
};
*/

#endif
