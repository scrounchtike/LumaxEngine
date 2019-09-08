
#ifndef FORWARD_RENDERER_TPP
#define FORWARD_RENDERER_TPP

#include "../lumax.hpp"

template <typename ECS, typename TRenderingSystem3DList, typename Level, typename Function, typename... Systems>
void RenderPassWater3D<ECS, TRenderingSystem3DList, Level, Function, Systems...>::update(const std::vector<ShaderPipeline*>& pipelines,
		const std::vector<Material*>& materials,
		Camera* camera)
{
	// Static stuff needed for rendering a unique scene
	// (Unique scene = unique camera = unique POV = unique FBO)
	static RenderIntoTexture fbo_reflection{};
	static RenderIntoTexture fbo_refraction{};
	static Texture* reflection_texture = nullptr;
	static Texture* refraction_texture = nullptr;

	//
	// Refraction rendering
	//
		
	fbo_refraction.bind();
	fbo_refraction.clear();

	// Render with clip plane at water level facing down
	lmx::getStaticLevel()->template renderPass<typename ThisType::System3DListToRender::type>();
	//lmx::getStaticLevel()->template renderPass<typename smpl::type_list_null>();
		
	fbo_refraction.unbind();
	//if(refraction_texture)
	//	delete refraction_texture;
	refraction_texture = fbo_refraction.resolve();
	
	fbo_refraction.blit();

	for(Material* material : materials)
	{
		material->addTexture("refraction_texture", refraction_texture);
	}

	//
	// Reflection rendering
	//
		
	fbo_reflection.bind();
	fbo_reflection.clear();

	// Camera pitch
	
  // Render with clip plane at water level facing up
	lmx::getStaticLevel()->template renderPass<typename ThisType::System3DListToRender::type>();
	//lmx::getStaticLevel()->template renderPass<typename smpl::type_list_null>();
	// Camera unpitch

	fbo_reflection.unbind();
	//if(reflection_texture)
	//	delete reflection_texture;
	reflection_texture = fbo_reflection.resolve();

	for(Material* material : materials)
	{
		material->addTexture("reflection_texture", reflection_texture);
	}
}

#endif
