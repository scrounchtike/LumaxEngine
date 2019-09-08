
#ifndef TERRAIN_RENDER_SYSTEMS_HPP
#define TERRAIN_RENDER_SYSTEMS_HPP

template <typename ECS>
struct RenderSystemForwardTerrainTransform3D :
	public ForwardRenderSystem<ECS, ShaderPipeline, Material, Terrain, Transform3D, ForwardRender>
{
	using Parent = ForwardRenderSystem<ECS, ShaderPipeline, Material, Terrain, Transform3D, ForwardRender>;
	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 const std::vector<Terrain*>& terrains,
										 const std::vector<Transform3D*>& transforms,
										 Camera* camera)
	{
		Parent::newFrame = true;

		for(int i = 0; i < pipelines.size(); ++i)
		{
			Parent::impl_update_pipeline(pipelines[i], camera);
			Parent::impl_update_material(pipelines[i], materials[i], camera);
			Parent::impl_render_forward(pipelines[i], materials[i], terrains[i]->model, transforms[i], camera);

			Parent::newFrame = false;
		}
	}
};

template <typename ECS>
struct RenderSystemForwardTerrainLight3D :
	public ForwardRenderSystem<ECS, ShaderPipeline, Material, Terrain, Transform3D, SimpleLightComponent, ForwardRender>
{
	using Parent = ForwardRenderSystem<ECS, ShaderPipeline, Material, Terrain, Transform3D, SimpleLightComponent, ForwardRender>;
	static void update(const std::vector<ShaderPipeline*>& pipelines,
										 const std::vector<Material*>& materials,
										 const std::vector<Terrain*>& terrains,
										 const std::vector<Transform3D*>& transforms,
										 const std::vector<SimpleLightComponent*>& lights,
										 Camera* camera)
	{
		Parent::newFrame = true;

		for(int i = 0; i < pipelines.size(); ++i)
		{
			Parent::impl_update_pipeline(pipelines[i], camera);
			Parent::impl_update_material(pipelines[i], materials[i], camera);
			Parent::impl_update_lights(pipelines[i], lights[i], camera);
			Parent::impl_render_forward(pipelines[i], materials[i], terrains[i]->model, transforms[i], camera);

			Parent::newFrame = false;
		}
	}
};

#endif
