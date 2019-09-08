
#ifndef RENDER_SYSTEMS_TPP
#define RENDER_SYSTEMS_TPP

#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"

#include "../CL/LightComponent.hpp"

// Rendering Systems
template <typename ECS, typename... Ts>
class RenderingSystem;
template <typename ECS, typename... Ts>
class System;

//-----------------------------------------------------------------------//
//
// 2D Rendering systems
//
//-----------------------------------------------------------------------//

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

//-----------------------------------------------------------------------//
//
// 3D Rendering systems
//
//-----------------------------------------------------------------------//



//-----------------------------------------------------------------------//
//
// UNITY like 3D rendering system
//
//
// https://docs.unity3d.com/Manual/RenderTech-ForwardRendering.html
//
// The system has upto N per-pixel lights
// Lighting is calculated inside the pixel shader for those lights
// There are often dynamic, or more important
//
// Support for 1 directional light in the pixel shader
//
// The system then has upto M per-vertex lights
// Lighting is calculated per vertex and extrapolated to fragments
//
// The system could also use baked lighting with Spherical harmonics for
// even more lights (although this isn't really a part of the system, and the
// baked texture can be added to the materials)
//
// ABOUT THE PASSES
//
// First pass renders the objects with baked light / SH and a per-pixel directional
// light and with all per-vertex lights since geometry does not change beyond this
// point.
//
// Additional passes are then used for each per-pixel light that influences the
// scene using Forward Rendering.
//
// ** IMPORTANT **
//
// Shader pipelines are static and cannot be modified
// The shaders are not customizable (same lighting for all)
// So do not add a ShaderPipeline component to any entity that will use this system
//
//-----------------------------------------------------------------------//

//3D
// Unity like Rendering System
template <typename ECS>
struct RenderSystemTransform3DUnity : public RenderingSystem<ECS, Material, Model3D, Transform3D, LightComponent, ForwardRender>{
	static bool newFrame;

	static ShaderPipeline* basePipelineColor;
	static ShaderPipeline* basePipelineTexture;
	static ShaderPipeline* basePipelineBoth;

	static ShaderPipeline* pointLightPipelineColor;
	static ShaderPipeline* pointLightPipelineTexture;
	static ShaderPipeline* pointLightPipelineBoth;

	static ShaderPipeline* spotLightPipelineColor;
	static ShaderPipeline* spotLightPipelineTexture;
	static ShaderPipeline* spotLightPipelineBoth;

	static ShaderPipeline* pipeline;

	static DirectionalLight dlight;
	
	static void construct(){
		std::cout << "Construct method called" << std::endl;
		basePipelineColor = new ShaderPipeline(lmx::getStaticResourceManager()->getShader("renderer/baseShaderForwardColor"));
		basePipelineTexture = new ShaderPipeline(lmx::getStaticResourceManager()->getShader("renderer/baseShaderForwardTexture"));
		//basePipelineBoth = new ShaderPipeline(lmx::getStaticResourceManager()->getShader("renderer/baseShaderForwardBoth"));

		pointLightPipelineTexture = new ShaderPipeline(lmx::getStaticResourceManager()->getShader("renderer/pointLightPipelineTexture"));

		pipeline = new ShaderPipeline(lmx::getStaticResourceManager()->getShader("renderer/shaderLight"));

		dlight = DirectionalLight(Vec3(-1,1,-1).normalize(), Vec3(1,0.6f,0.6f), 0.7f);
		
		// Light uniforms
		pipeline->bind();
		struct VertexLights{
			PointLight plights[4];
			SpotLight slights[4];
		};
		VertexLights vlights;
		//vlights.plights[0] = PointLight(Vec3(3,2,10), Vec3(1,1,0), Vec3(0.5,0.5,0.5));
		struct PixelLights{
			DirectionalLight dlight;
			PointLight plights[4];
			SpotLight slights[4];
		};
		PixelLights plights;
		//plights.plights[0] = PointLight(Vec3(3,2,10), Vec3(1,1,0), Vec3(0.2,0.2,0.2));
		plights.dlight = DirectionalLight(Vec3(1,-1,1).normalize(), Vec3(1,0,0), 0.5f);
		
		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboLightsVertex);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight)*4 + sizeof(SpotLight)*4, &vlights, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboLights);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight) + sizeof(PointLight)*4 + sizeof(SpotLight)*4, &plights, GL_DYNAMIC_DRAW);
		
		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboDirectionalLight);
		basePipelineColor->bind();
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight), &dlight, GL_DYNAMIC_DRAW);
		std::cout << "Unity like 3D rendering system construction done" << std::endl;
	}

	static void update(const std::vector<Material*>& materials, const std::vector<Model3D*>& meshes, const std::vector<Transform3D*>& transforms, const std::vector<LightComponent*>& lightDescs, Camera* camera){
		newFrame = true;
		
		static uint32 materialID = (uint32)(-1);
		static uint32 modelID = (uint32)(-1);
		for(int i = 0; i < meshes.size(); ++i){
			Material* material = materials[i];
			Model3D* mesh = meshes[i];
			Transform3D* transform = transforms[i];
			LightComponent* lights = lightDescs[i];

			// Bind color/texture light pipeline for forward rendering
			pipeline->bind();
			if(newFrame){
				newFrame = false;
				// Update projection and view matrices
				glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
				// Update camera position
				pipeline->setUniform3f("cameraPos", camera->getPosition());
			}
			
			//Switch shader pipeline if necessary
			/*
			static ShaderPipeline* pipeline = nullptr;
			static ShaderPipeline* pointLightPipeline = pointLightPipelineTexture;
			static ShaderPipeline* spotLightPipeline = spotLightPipelineTexture;
			if(materialDesc != newMaterialDesc || newFrame){
				newFrame = false;
				materialDesc = newMaterialDesc;
				if(materialDesc == (MAT_TEXTURED | MAT_COLORED)){
					pipeline = basePipelineBoth;
					//pointLightPipeline = pointLightPipelineBoth;
					//spotLightPipeline = spotLightPipelineBoth;
				}else if(materialDesc == MAT_COLORED){
					pipeline = basePipelineColor;
					//pointLightPipeline = pointLightPipelineColor;
					//spotLightPipeline = spotLightPipelineColor;
				}else if(materialDesc == MAT_TEXTURED){
					pipeline = basePipelineTexture;
					//pointLightPipeline = pointLightPipelineTexture;
					//spotLightPipeline = spotLightPipelineTexture;
				}
				// Bind projection and view matrices
				glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
				// Bind projection and view matrices for light pipelines
				pointLightPipeline->bind();
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
				//spotLightPipeline->bind();
				//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
				//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
				// Bind pipeline
				pipeline->bind();
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
			}
			assert(pipeline);
			*/
			
			// Material Settings
			if(materialID != material->ID){
				materialID = material->ID;
				if(material->isTextured){
					// Set texture
					material->getTexture()->bind();
					pipeline->setSubroutine("getAlbedo", GL_FRAGMENT_SHADER, "getTexturedColor");
				}
				if(material->isColored){
					// Set color UBO
					glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboColor);
					Vec3 color = material->getColor().xyz();
					Vec4 color4 = Vec4(color.x, color.y, color.z, 1.0);
					glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, &color4.x, GL_DYNAMIC_DRAW);
					pipeline->setSubroutine("getAlbedo", GL_FRAGMENT_SHADER, "getColor");
				}
				if(material->isBlended){
					pipeline->setUniform1f("blend", material->blend); 
					pipeline->setSubroutine("getAlbedo", GL_FRAGMENT_SHADER, "getBlendedColor");
				}
				pipeline->updateSubroutines();
			}

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
		
		
		// Light passes
	}
};
template <typename ECS>
bool RenderSystemTransform3DUnity<ECS>::newFrame = false;
template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::basePipelineColor = nullptr;
template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::basePipelineTexture = nullptr;
template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::basePipelineBoth = nullptr;

template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::pointLightPipelineColor = nullptr;
template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::pointLightPipelineTexture = nullptr;
template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::pointLightPipelineBoth = nullptr;

template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::spotLightPipelineColor = nullptr;
template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::spotLightPipelineTexture = nullptr;
template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::spotLightPipelineBoth = nullptr;

template <typename ECS>
ShaderPipeline* RenderSystemTransform3DUnity<ECS>::pipeline = nullptr;

template <typename ECS>
DirectionalLight RenderSystemTransform3DUnity<ECS>::dlight;


//-----------------------------------------------------------------------//
//
// Basic 3D Deferred renderer
//
//   --  WIP  --
//
//-----------------------------------------------------------------------//
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
