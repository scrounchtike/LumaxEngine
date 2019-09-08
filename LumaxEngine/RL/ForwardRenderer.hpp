
#ifndef FORWARD_RENDERER_HPP
#define FORWARD_RENDERER_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "../RL/RenderIntoTexture.hpp"
#include "../CL/LightComponent.hpp"

#include "../RL/ShaderPipeline.hpp"
#include "../RL/Material.hpp"
#include "../RL/Model2D.hpp"
#include "../RL/Model3D.hpp"
#include "../RL/Renderer.hpp"
#include "../CL/Movement.hpp"
#include "../CL/LightComponent.hpp"
#include "../CL/DOOMComponents.hpp"
#include "../TL/Terrain.hpp"
#include "../TL/TerrainClipmap.hpp"

//-----------------------------------------------------------------------//
//
// Basic 3D forward renderer with Transform 3D
//
// Assumes that all entities passed have a Transform3D component from which
// to get the transformation matrix the shader uses
//
// Forward in one pass for all lights
//
// Lighting considerations (all lights are per-pixel)
//
// 1 Directional Light
// upto 4 point lights
// upto 4 spot lights
//
// Lights are uploaded to the shader (renderer) prior to using the lights UBO
//
// Shaders are custom and passed as a ShaderPipeline component for all entities
//
//-----------------------------------------------------------------------//

template <typename ECS, typename... Signature>
class ForwardRenderSystem : public RenderingSystem<ECS, Signature...>{
public:
	template <typename Level>
	void updateEntityVector(const Level& level,
													std::vector<uint32>& entities,
													const Camera* camera)
	{
		// ---------------------------------------------- //
		//
		// If option is selected, sort entities
		//
		// ---------------------------------------------- //
		
		//
		// Translucency sorting, back to front (painter's)
		//
		
		// No entities to sort
		return;
		if(entities.size() == 0)
			return;
		
		const bool translucencySorting = true;
		if(translucencySorting)
		{
			// Iterate over all entities, and find out the distance to the camera
			std::vector<float> distances(entities.size() - transparency_border_index);
			for(int ti = 0; ti < (entities.size() - transparency_border_index); ++ti)
			{
				// Index of the translucent object
				int index = transparency_border_index + ti;
				
				// TODO: We need to iterate over transforms of only translucent objects
				// Translucent objects are PACKED together since the vectors are sorted
				// and translucency is a sorted flag !
				
				// Get position of entity through matrix
				Vec3 transform_position = level.template getComponent<Transform3D>(entities[index])->getTranslation();
				float sqDist = (transform_position - camera->getPosition()).getSqLength();
				distances[ti] = 1.0 / sqDist;
			}

			// Sort the entities using the distances
			// Using Insertion sort since hopefully data is almost sorted
			// from frame to frame.
			for (auto it = distances.begin(); it != distances.end(); it++) 
			{
				uint32 index = it - distances.begin();
				
        // Searching the upper bound, i.e., first
        // element greater than *it from beginning
				auto const insertion_point =  
					std::upper_bound(distances.begin(), it, *it);
				uint32 insertion_index = insertion_point - distances.begin();
				
				auto const it_entities = entities.begin() + transparency_border_index + index;
				
				//auto const entities_point =
				//	std::upper_bound(entities.begin(), it_entities, *it_entities);
				auto const entities_point = entities.begin() + transparency_border_index + (insertion_point - distances.begin());
				
        // Shifting the unsorted part
        std::rotate(insertion_point, it, it+1);
				std::rotate(entities_point, it_entities, it_entities+1);
			}
		}
		
		//
		// Opaque sorting, front to back (maximize early z-cull)
		//
		
		const bool opaqueSorting = false;
		if(opaqueSorting)
		{
			// TODO: Pretty much the same thing, but for opaque entities
		}
	}

	void addEntity(uint64 hint, uint32 entity)
	{
	  // Gather where translucent entities begin and end
		// At the same time, keep track of where opaque objects start and end

		// wait is this for all rendering systems ? I dont think so..
		// Need to specialize this function based on the system
		
		bool translucent = ((hint >> 48) >> 8) & 0x1;
		
		// If entity is translucent, then it will be sorted in the back, thus index remains the same
		// Else, if opaque, then entity is inserted in front and the index increments
		transparency_border_index += (uint32)(!translucent);
		
		this->entities.insert(std::pair<uint64, uint32>(hint, entity));
	}

	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 const std::vector<Model3D*>& meshes,
										 const std::vector<Transform3D*>& transforms,
										 Camera* camera)
	{
		newFrame = true;
		
		//std::cout << "num pipelines = " << pipelines.size() << std::endl;
		for(int i = 0; i < pipelines.size(); ++i)
		{
			//std::cout << "new render" << std::endl;
			impl_update_pipeline(pipelines[i], camera);
			impl_update_material(pipelines[i], materials[i], camera);
			impl_render_forward(pipelines[i], materials[i], meshes[i], transforms[i], camera);
		}
	}

protected:
	
	// TODO: Should be removed....
	static bool newFrame;

	// Keeping track of what is bound into the rendering pipeline
	static uint32 pipelineID;
	static uint32 materialID;
	static uint32 modelID;

	// Transparency and sorting variables
	static uint32 transparency_border_index;

	//
	// Spedific functions useful for multiple forward render systems
	//

	//
	// Shader pipeline update function
	//
	static void impl_update_pipeline(ShaderPipeline* pipeline,
													Camera* camera)
	{	
		// Switch shader pipeline if necessary and bind uniforms every time we switch to
		// a new Shader Pipeline (or if first frame)
		if(pipelineID != pipeline->ID || newFrame){
			pipelineID = pipeline->ID;
			pipeline->bind();
		
			// Set per pipeline uniformsi
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
		
			// Bind projection and view matrices
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, camera->getProjectionMatrix().getHeadPointer());
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, camera->getViewMatrix().getHeadPointer());
		}
	}

	//
	// Material update function
	//
	static void impl_update_material(ShaderPipeline* pipeline,
																	 Material* material,
																	 Camera* camera)
	{
		bool usingColorSubroutine = pipeline->usingColorSubroutine;
		//usingColorSubroutine = true;
		// This check will not work
		// TODO: Check if material OR pipeline have changed
		//if(materialID != material->ID)
		{
			materialID = material->ID;
			
			if(material->isTextured)
			{
				// Get samplers for shader pipeline
				unsigned num_samplers_2D = pipeline->getNumSamplers2D();
				
				for(int sampler_i = num_samplers_2D-1; sampler_i >= 0; --sampler_i)
				{
					std::string texture_name = pipeline->getSamplerName(sampler_i);
					Texture* texture = material->getTextureByName(texture_name);
					if(!texture)
					{
						std::cout << "WARNING : Texture name was not found in material = " << texture_name << std::endl;
						//assert(!"Missing texture in material needed for shader pipeline. Aborting.");
						return;
					}
					pipeline->setUniform1i(texture_name, sampler_i);
					glActiveTexture(GL_TEXTURE0 + sampler_i);
					texture->bind();
				}
				if(usingColorSubroutine)
					pipeline->setSubroutine("getFinalColor", GL_FRAGMENT_SHADER, "getTextureColor");
			}
			if(material->isColored)
			{
				glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboColor);
				Vec4 color = material->getColor();
				glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, &color.x, GL_DYNAMIC_DRAW);
				if(usingColorSubroutine)
					pipeline->setSubroutine("getFinalColor", GL_FRAGMENT_SHADER, "getColor");
			}
			if(material->isBlended)
			{
				pipeline->setUniform1f("blend", material->blend);
				if(usingColorSubroutine)
					pipeline->setSubroutine("getFinalColor", GL_FRAGMENT_SHADER, "getBlendedColor");
			}
			
			// Update subroutine uniforms
			pipeline->updateSubroutines();
		}
	}

	//
	// Update lighting components and uniforms function
	//
	static void impl_update_lights(ShaderPipeline* pipeline,
																 SimpleLightComponent* lighting,
																 Camera* camera)
	{
		static DirectionalLight* dlight = (DirectionalLight*)(-1);
		static PointLight* plights[4] = { (PointLight*)(-1) };
		static SpotLight* slights[4] = { (SpotLight*)(-1) };

		// Initialize all light UBOs to all zeros
		if(dlight == (DirectionalLight*)(-1))
		{
			dlight = nullptr;
			DirectionalLight dir_light = DirectionalLight(Vec3(0,0,0), Vec3(0,0,0), 0);
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboDirectionalLight);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight), &dir_light, GL_DYNAMIC_DRAW);
		}
		if(plights[0] == (PointLight*)(-1))
		{
			plights[0] = plights[1] = plights[2] = plights[3] = nullptr;
			PointLight pointlights[4];
			memset(pointlights, 0, sizeof(PointLight)*4);
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboPointLight);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight)*4, &pointlights, GL_DYNAMIC_DRAW);
		}
		if(slights[0] == (SpotLight*)(-1))
		{
			slights[0] = slights[1] = slights[2] = slights[3] = nullptr;
			SpotLight spotlights[4];
			memset(spotlights, 0, sizeof(SpotLight)*4);
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboSpotLight);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLight)*4, &spotlights, GL_DYNAMIC_DRAW);
		}

		// Update lighting UBOs
		// Check if any lights have changed between the entities
		if(dlight != lighting->dlight)
		{
			dlight = lighting->dlight;
			// Update Directional Light UBO
			glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboDirectionalLight);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight), dlight, GL_DYNAMIC_DRAW);
		}
		for(int i = 0; i < 4; ++i)
		{
			if(plights[i] != lighting->plights[i])
			{
				plights[i] = lighting->plights[i];
				// Update the point lights in UBO
				// (If one changed, change all of them, its not more costly at this point)
			  glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboPointLight);
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(PointLight)*i, sizeof(PointLight), plights[i]);
			}
		}
		for(int i = 0; i < 4; ++i)
		{
		  if(slights[i] != lighting->slights[i])
			{
				slights[i] = lighting->slights[i];
				// Update the point lights in UBO
				// (If one changed, change all of them, its not more costly at this point)
			  glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboSpotLight);
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(SpotLight)*i, sizeof(SpotLight), slights[i]);
			}
		}
	}

	//
	// Updating the clip planes function
	// 
	static void impl_update_clip_planes(ShaderPipeline* pipeline,
																			Material* material,
																			Model3D* model,
																			Transform3D* transform,
																			Camera* camera)
	{
		static unsigned int pipelineID = static_cast<unsigned int>(-1);

		// Bind the pipeline if not already selected
		if(pipeline->ID != pipelineID || newFrame)
		{
			pipeline->bind();
			pipelineID = pipeline->ID;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboClipPlanes);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(Vec4)*6, &Renderer::clip_planes, GL_DYNAMIC_DRAW);
	}
	
	//
	// Model render function
	// 
	static void impl_render_forward(ShaderPipeline* pipeline,
																	Material* material,
																	Model3D* mesh,
																	Transform3D* transform,
																	Camera* camera)
	{
		static uint32 modelID = static_cast<uint32>(-1);
		
		// Camera Position uniform
		pipeline->setUniform3f("cameraPos", camera->getPosition());
		
		// Bind 3D mesh if necessary
		if(modelID != mesh->ID || newFrame)
		{
			modelID = mesh->ID;
			mesh->bindForRender();
		}
		
		// Update per entity uniforms
		glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
		mesh->renderBuffersOnly();
	}
};

// Out of line defintions
template <typename ECS, typename... S>
bool ForwardRenderSystem<ECS, S...>::newFrame = false;
template <typename ECS, typename... S>
uint32 ForwardRenderSystem<ECS, S...>::pipelineID = (uint32)(-1);
template <typename ECS, typename... S>
uint32 ForwardRenderSystem<ECS, S...>::materialID = (uint32)(-1);
template <typename ECS, typename... S>
uint32 ForwardRenderSystem<ECS, S...>::modelID    = (uint32)(-1);
template <typename ECS, typename... S>
uint32 ForwardRenderSystem<ECS, S...>::transparency_border_index;


template <typename ECS>
struct RenderSystemForwardTransform3D :
	public ForwardRenderSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, ForwardRender>
{
	using Parent = ForwardRenderSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, ForwardRender>;
	
	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 const std::vector<Model3D*>& meshes,
										 const std::vector<Transform3D*>& transforms,
										 Camera* camera)
	{
		Parent::newFrame = true;
		
		//std::cout << "num pipelines = " << pipelines.size() << std::endl;
		for(int i = 0; i < pipelines.size(); ++i)
		{
			//std::cout << "new render" << std::endl;
			Parent::impl_update_pipeline(pipelines[i], camera);
			Parent::impl_update_material(pipelines[i], materials[i], camera);
			Parent::impl_render_forward(pipelines[i], materials[i], meshes[i], transforms[i], camera);

			Parent::newFrame = false;
		}
	}
};

template <typename ECS>
struct RenderSystemForwardLight3D :
		public ForwardRenderSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, SimpleLightComponent, ForwardRender>
{
	using Parent = ForwardRenderSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, SimpleLightComponent, ForwardRender>;
	
	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 const std::vector<Model3D*>& meshes,
										 const std::vector<Transform3D*>& transforms,
										 const std::vector<SimpleLightComponent*>& lights,
										 Camera* camera)
	{
		Parent::newFrame = true;
		
		//std::cout << "num pipelines = " << pipelines.size() << std::endl;
		for(int i = 0; i < pipelines.size(); ++i)
		{
			//std::cout << "new render" << std::endl;
			Parent::impl_update_pipeline(pipelines[i], camera);
			Parent::impl_update_material(pipelines[i], materials[i], camera);
			Parent::impl_update_lights(pipelines[i], lights[i], camera);
			Parent::impl_render_forward(pipelines[i], materials[i], meshes[i], transforms[i], camera);

			Parent::newFrame = false;
		}
	}
};

template <typename ECS>
struct RenderSystemForwardWater3D :
		public ForwardRenderSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, SimpleLightComponent, ForwardRender>
{
	using Parent = ForwardRenderSystem<ECS, ShaderPipeline, Material, Model3D, Transform3D, SimpleLightComponent, ForwardRender>;
	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 const std::vector<Model3D*>& meshes,
										 const std::vector<Transform3D*>& transforms,
										 const std::vector<SimpleLightComponent*>& lights,
										 Camera* camera)
	{
		Parent::newFrame = true;
		
		//std::cout << "num pipelines = " << pipelines.size() << std::endl;
		for(int i = 0; i < pipelines.size(); ++i)
		{
			Parent::impl_update_pipeline(pipelines[i], camera);
			Parent::impl_update_material(pipelines[i], materials[i], camera);
			Parent::impl_update_lights(pipelines[i], lights[i], camera);
			Parent::impl_render_forward(pipelines[i], materials[i], meshes[i], transforms[i], camera);
			
			Parent::newFrame = false;
		}
	}
};

template <typename ECS>
struct RenderSystemForwardTerrainClipmap :
	public ForwardRenderSystem<ECS, ShaderPipeline, Material, TerrainClipmap, SimpleLightComponent, ForwardRender>
{
	using Parent = ForwardRenderSystem<ECS, ShaderPipeline, Material, TerrainClipmap, SimpleLightComponent, ForwardRender>;
	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 const std::vector<TerrainClipmap*>& terrains,
										 const std::vector<SimpleLightComponent*>& lights,
										 Camera* camera)
	{
		Parent::newFrame = true;
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// For now, just assume one terrain and that multiple terrains is a mistake:
		assert(pipelines.size() == 1);

		//std::cout << "render terrain" << std::endl;
		
		for(int i = 0; i < pipelines.size(); ++i)
		{
			pipelines[i]->bind();
			Parent::impl_update_pipeline(pipelines[i], camera);

			Parent::impl_update_material(pipelines[i], materials[i], camera);

			// TODO : Update lights
			Parent::impl_update_lights(pipelines[i], lights[i], camera);

			// Camera Position uniform
			pipelines[i]->setUniform3f("cameraPos", camera->getPosition());
			
			// Mesh rendering
			uint32 num_levels = terrains[i]->num_levels;
			float tile_resolution = terrains[i]->tile_resolution;
			for(int l = 0; l < num_levels; ++l)
			{
				float scale = 1 << l;

				// Snap the camera position for each level
				Vec3 campos = camera->getPosition();
				Vec2 snapped_camera = Vec2(floor(campos.x / scale) * scale,
																	 floor(campos.z / scale) * scale);

				// Compute size and base location
				float tile_size = (float)((int)tile_resolution << l);
				Vec2 base = snapped_camera - Vec2(tile_size * 2);
				
				// Draw tile meshes
				terrains[i]->tile_mesh->bindForRender();
				for(int x = 0; x < 4; ++x)
				{
					for(int z = 0; z < 4; ++z)
					{
						if(l == 0 ||
							 (x*z) % 3 == 0)
						{
							// Draw square tile mesh
							static Transform3D* transform = new Transform3D();
							//float tile_pos_x = -2.0f * tile_resolution + x * tile_resolution;
							//float tile_pos_z = -2.0f * tile_resolution + z * tile_resolution;

							// Add filler space if necessary
							Vec2 fill = Vec2(x >= 2 ? 1 : 0, z >= 2 ? 1 : 0) * scale;
							Vec2 tile_pos = base + Vec2(x, z) * tile_size + fill;
							
							transform->translation = Vec3(tile_pos.x, 0, tile_pos.y);
							transform->scale = Vec3(scale, 1, scale);
							transform->updateMatrix();

							terrains[i]->tile_mesh->bindForRender();
							glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
							//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
							struct Buffer
							{
								Mat4 projection;
								Mat4 view;
								Mat4 transform;
							};
							static Buffer buffer;
							buffer.projection = camera->getProjectionMatrix();
							buffer.view = camera->getViewMatrix();
							buffer.transform = Mat4().initIdentity();
							//glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16*3, &buffer, GL_DYNAMIC_DRAW);
							pipelines[i]->setUniformMatrix4f("projection", camera->getProjectionMatrix());
							pipelines[i]->setUniformMatrix4f("view", camera->getViewMatrix());
							pipelines[i]->setUniformMatrix4f("transform", *transform->getTransformation());
							terrains[i]->tile_mesh->render();
						}	
					}
				}

				// Draw trim mesh (L-shape)
				if(l != num_levels - 1)
				{
					// TODO : Draw Seam

					// Draw Trim
					float next_scale = scale * 2.0f;
					Vec2 next_snapped_pos = Vec2(floor(campos.x / next_scale) * next_scale,
																			 floor(campos.z / next_scale) * next_scale);
					Vec2 tile_center = snapped_camera + Vec2(scale * 0.5f);

					// Compute rotation needed for L-shape
					Vec2 d = Vec2(campos.x, campos.z) - next_snapped_pos;
					uint32 r = 0;
					r |= d.x >= scale ? 0 : 2;
					r |= d.y >= scale ? 0 : 1;
					// Possible rotations
					Vec3 rotations[4] =
					{
						Vec3(0,0,0), Vec3(0,toRad(270),0), Vec3(0,toRad(90),0), Vec3(0,toRad(180),0)
					};
					Vec3 rotation = rotations[r];
					
					static Transform3D* transform = new Transform3D();
					transform->translation = Vec3(tile_center.x, 0, tile_center.y);
					transform->scale = Vec3(scale, 1, scale);
					transform->rotation = rotation;
					transform->updateMatrix();

					terrains[i]->trim_mesh->bindForRender();
					glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
					terrains[i]->trim_mesh->renderBuffersOnly();
				}
				
				// Draw filler meshes
				{
					static Transform3D* transform = new Transform3D();
					transform->translation = Vec3(snapped_camera.x, 0, snapped_camera.y);
					transform->scale = Vec3(scale, 1, scale);
					transform->updateMatrix();
					
					terrains[i]->filler_mesh->bindForRender();
					glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
					terrains[i]->filler_mesh->renderBuffersOnly();
				}
				
				// Draw cross mesh
				if(l == 0)
				{
					// Innermost level
					static Transform3D* transform = new Transform3D();
					float camx = (int)floor(camera->getPosition().x);
					float camz = (int)floor(camera->getPosition().z);
					transform->translation = Vec3(camx, 0, camz);
					transform->updateMatrix();

					terrains[i]->cross_mesh->bindForRender();
					glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
					terrains[i]->cross_mesh->renderBuffersOnly();
				}
				
				// Draw seams
				if(l != num_levels - 1)
				{
					float next_scale = scale * 2.0f;
					Vec2 next_snapped_pos = Vec2(floor(campos.x / next_scale) * next_scale,
																			 floor(campos.z / next_scale) * next_scale);

					// Draw the seam mesh
					Vec2 next_base = next_snapped_pos - Vec2((int)tile_resolution << (l + 1));

					static Transform3D* transform = new Transform3D();
					transform->translation = Vec3(next_base.x, 0, next_base.y);
					transform->scale = Vec3(scale, 1.0f, scale);
					transform->updateMatrix();
					
					terrains[i]->seam_mesh->bindForRender();
					glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboMVP3D);
					glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16*2, sizeof(float)*16, transform->getTransformation()->getHeadPointer());
					terrains[i]->seam_mesh->renderBuffersOnly();
				}
			}
				
			Parent::newFrame = false;
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
};

// #include other (forward) rendering systems
#include "TerrainRenderSystems.hpp"

//
// Forward render passes
//

template <typename ECS, typename TRenderingSystem3DList, typename Level, typename Function, typename... Systems>
struct RenderPassWater3D : public RenderPass<ECS, TRenderingSystem3DList, Level, Function, Systems...>
{
	using Parent = RenderPass<ECS, TRenderingSystem3DList, Level, Function, Systems...>;
	using ThisType = RenderPassWater3D<ECS, TRenderingSystem3DList, Level, Function, Systems...>;
	//using SystemsToRender = typename Parent::SystemsToRender;
	//using SystemsToOmit = typename Parent::SystemsToOmit;
	
	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 Camera* camera);
};

template <typename ECS, typename TRenderingSystem3DList, typename Level, typename Function, typename... Systems>
struct RenderPassForward3D : public RenderPass<ECS, TRenderingSystem3DList, Level, Function, Systems...>
{
	using Parent = RenderPass<ECS, TRenderingSystem3DList, Level, Function, Systems...>;
	using ThisType = RenderPassWater3D<ECS, TRenderingSystem3DList, Level, Function, Systems...>;

	static void update(Camera* camera)
	{
	}
};


#include "ForwardRenderer.tpp"

#endif // FORWARD_RENDERER_HPP
