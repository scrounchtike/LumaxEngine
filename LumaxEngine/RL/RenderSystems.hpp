
#ifndef RENDER_SYSTEMS_TPP
#define RENDER_SYSTEMS_TPP

#include "../lumax.hpp"
#include "../RML/ResourceManager.hpp"

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

// Render Flags
template <typename ECS>
struct RenderSystemTest : public RenderingSystem<ECS, Model3D>{
	static void update(const std::vector<Model3D*>& models, Camera* camera){
		
	}
};

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
	}
	
	static void impl(ShaderPipeline* pipeline, Material* material, Model3D* mesh, Transform3D* transform, Camera* camera){
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
