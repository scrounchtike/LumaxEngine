
#ifndef GAME1_HPP
#define GAME1_HPP

#include "../core/Level.hpp"

// Renderers and render passes defintions
#include "../RL/ForwardRenderer.hpp"

#include "../lumax.hpp"

// Include all components
#include "../RL/lights.hpp"
#include "../CL/LightComponent.hpp"

#include "../TL/Terrain.hpp"
#include "../TL/FBmTerrain.hpp"

class Game1
{
public:
	  template <typename TSystems,                                               
						  typename TRenderingSystems2D,
	            typename TRenderingSystems3D,
							typename TRenderPasses2D,
							typename TRenderPasses3D>                                    
		static void loadLevel(Level<TSystems, TRenderingSystems2D, TRenderingSystems3D, TRenderPasses2D, TRenderPasses3D>* level)
	{
		
		// SCENE 1 lighting
		DirectionalLight* dlight = new DirectionalLight(Vec3(-1,1,-1).normalize(),
																										Vec3(1,1,1),
																										1.0f);
		PointLight* plight1 = new PointLight(Vec3(2,2,13), Vec3(1,0,0.5), Vec3(0.1,0.1,0.5));
		PointLight* plight2 = new PointLight(Vec3(-1.5,-1.5,13), Vec3(1,1,0), Vec3(0.1,0.1,0.1));
		SpotLight* slight1 = new SpotLight(Vec3(12,2,13), Vec3(0.5,0.2,1), Vec3(0.1,0.05,0.05), Vec3(-1,0,-0.5).normalize(), 0.99f);
		SimpleLightComponent* lcomponent = new SimpleLightComponent();
		lcomponent->dlight = dlight;
		
		lcomponent->plights[0] = plight1;
		lcomponent->plights[1] = plight2;
		
		lcomponent->slights[0] = slight1;
		
		//
		// Lambda functions useful for loading assets and creating entities
		//
		// Creating materials from textures
		auto createTextureMaterial = [](const std::string& name,
																		const std::string& filename,
																		Vec3 color = Vec3(0,0,0),
																		float blend = 1.0f)
			{
				Texture* texture = lmx::resManager->getTexture(filename);
				Material* material = new Material();
				material->setTexture(texture);
				if(blend != 1.0f)
				{
					material->setColor(color);
					material->setBlend(blend);
				}
				lmx::resManager->addMaterial(name, material);
			};
		auto createColorMaterial3 = [](const std::string& name,
																	 Vec3 color)
			{
				Material* material = new Material();
				material->setColor(color);
				lmx::resManager->addMaterial(name, material);
			};
		auto createColorMaterial4 = [](const std::string& name,
																	 Vec4 color)
			{
				Material* material = new Material();
				material->setColor(color);
				lmx::resManager->addMaterial(name, material);
			};
		// Creating shader pipelines from shader programs (.vs and .fs)
		auto createShaderPipeline = [](const std::string& name,
																	 const std::string& filename)->ShaderPipeline*
			{
				Shader* shader = lmx::resManager->getShader(filename);
				ShaderPipeline* shader_pipeline = new ShaderPipeline(shader);
				//shader_pipeline->addSampler2D("texture0", 0);
				lmx::resManager->addShaderPipeline(name, shader_pipeline);
				return shader_pipeline;
			};
		auto createTransparentCube = [level](Vec3 position,
																				 Vec3 color,
																				 SimpleLightComponent* lcomponent)->uint32
			{
				uint32 entity = level->template createEntity();
				level->template addToGroup<ForwardRender>(entity);
				level->template addComponent<Model3D>(entity, lmx::resManager->getModel3D("cube"));
				level->template addComponent<Material>(entity, new Material(Vec4(color, 0.5)));
				level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3Dlighting"));
				level->template addComponent<Transform3D>(entity, new Transform3D(position));
				level->template addComponent<SimpleLightComponent>(entity, lcomponent);
				level->template registerEntity(entity);
				level->template registerEntityForRender3D<TypeRenderSystemForwardLight3D>(entity);
				return entity;
			};
		// Terrain creation lambda
		auto createSimpleTerrain = [level, lcomponent](Vec3 position,
																									 float xi, float zi,
																									 TerrainGenDesc* gen,
																									 int lod)->Terrain*
			{
				Model3D* terrain_model;
				Terrain* terrain = new Terrain(gen, xi, zi, 100.0f);
				//terrain->generate(position.x, position.z, lod);
				terrain->generate(lod);
				terrain_model = terrain->model;
				
				uint32 entity = level->template createEntity();
				level->template addToGroup<ForwardRender>(entity);
				level->template addComponent<Transform3D>(entity, new Transform3D(position,
																																					Vec3(0,0,0),
																																					Vec3(1,10,1)));
				level->template addComponent<Terrain>(entity, terrain);
				level->template addComponent<SimpleLightComponent>(entity, lcomponent);
				Material* terrain_material = new Material();
				terrain_material->addTexture("grass", lmx::resManager->getTexture("grass.png"));
				terrain_material->addTexture("dirt", lmx::resManager->getTexture("dirt.png"));
				terrain_material->addTexture("rocks", lmx::resManager->getTexture("rocks.png"));
				terrain_material->addTexture("sand", lmx::resManager->getTexture("sand.png"));
				level->template addComponent<Material>(entity, terrain_material);
				level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3DterrainLight"));

				level->template registerEntity(entity);
				level->template registerEntityForRender3D<TypeRenderSystemForwardTerrainLight3D>(entity);

				return terrain;
			};

		// DEBUG
		// Print all systems ids
		std::cout << "TypeRenderSystemForwardTransform3D id = " <<
			typeid(*(TypeRenderSystemForwardTransform3D*)(nullptr)).name() << std::endl;
		// DEBUG
		
		// ****
		// Create level entities inside this function
		// ****

		//
		// Create models and meshes here !
		//
		lmx::resManager->addModel3D("cube", "cube.obj");
		lmx::resManager->addModel3D("sphere", "sphere.obj");
		lmx::resManager->addModel3D("dragon", "dragon.obj");
		{
			std::vector<float> vertices = {
				-1,0,-1,
				-1,0,1,
				1,0,1,
				1,0,-1
			};
			std::vector<int> indices = { 2,1,0,0,3,2 };
			std::vector<float> texCoords = {
				0,0,
				0,1,
				1,1,
				1,0
			};
			std::vector<float> normals = {
				0,1,0,
				0,1,0,
				0,1,0,
				0,1,0
			};
			std::vector<float> tangents = {
				1,0,0,
				1,0,0,
				1,0,0,
				1,0,0
			};
			Model3D* model = new Model3D(vertices, indices, texCoords, normals, tangents);
			lmx::resManager->addModel3D("quad_3D", model);
		}
		
		//
		// Create materials and textures here !
		//
		createTextureMaterial("test_png", "test.png");
		createTextureMaterial("gold", "gold.png");
		createTextureMaterial("grass", "grass.png");
		createColorMaterial3("blue", Vec3(0,0,1));
		createTextureMaterial("red_test_png", "test.png", Vec3(1,1,0), 0.5f);
		
		
		Texture* texture_only = lmx::resManager->getTexture("test.png");
		Material* test_texture = new Material(texture_only);
		lmx::resManager->addMaterial("test_texture", test_texture);
		
		createColorMaterial4("transparent_green", Vec4(0,1,0,0.5));
		{
			Texture* bricks = lmx::resManager->getTexture("bricks2.png");
			Texture* nmBricks = lmx::resManager->getTexture("bricks2_normal.png");
			Material* material = new Material();
			//material->setNormalTexture(nmBricks);
			material->addTexture("texSample", bricks);
			material->addTexture("texNormal", nmBricks);
			lmx::resManager->addMaterial("bricks_normal_map", material);
		}
		// Loading the cubemap for the skybox
		{
			std::vector<std::string> faces =
			{
				"right.png",
				"left.png",
				"top.png",
				"bottom.png",
				"front.png",
				"back.png"
			};
			CubemapTexture* cubemap = lmx::resManager->getCubemap("sky", faces.data());
		}
		
		//
		// Create shaders and pipelines here !
		//
		ShaderPipeline* pipeline;
		pipeline = createShaderPipeline("basic3Dtexture", "renderer/baseShaderForwardTexture");
		pipeline->addSampler2D("texSample", 0);
		pipeline = createShaderPipeline("basic3Dcolor", "renderer/baseShaderForwardColor");
		pipeline = createShaderPipeline("basic3D", "renderer/baseShaderForwardBoth");
		pipeline->addSampler2D("texSample", 0);
		pipeline = createShaderPipeline("forward3Dlighting", "renderer/shader3Dforward");
		pipeline->addSampler2D("texture0", 0);
		pipeline->addSubroutine("getFinalColor");
		pipeline = createShaderPipeline("forward3DlightingNM", "renderer/shader3DforwardNM");
		pipeline->addSampler2D("texSample", 0);
		pipeline->addSampler2D("texNormal", 1);
		pipeline->addSubroutine("getFinalColor");
		
		pipeline = createShaderPipeline("forward3Dwater", "renderer/water_shader");
		pipeline->addSampler2D("texSample", 0);
		pipeline->addSampler2D("reflection_texture", 1);
		pipeline->addSampler2D("refraction_texture", 2);
		pipeline->addSubroutine("getFinalColor");

		pipeline = createShaderPipeline("test_texture_samplers", "renderer/shader3Dtest_texture");
		pipeline->addSampler2D("texSample", 0);
		pipeline->addSampler2D("texSample2", 1);
		pipeline->addSampler2D("texSample3", 2);
		//pipeline->addSubroutine("getFinalColor");

		pipeline = createShaderPipeline("forward3Dsimple", "renderer/shader3Dforward_simple");
		pipeline->addSampler2D("texSample", 0);
		pipeline->addSubroutine("getFinalColor");

		pipeline = createShaderPipeline("forward3DterrainLight", "terrain/terrainShader");
		pipeline->addSampler2D("grass", 0);
		pipeline->addSampler2D("dirt", 1);
		pipeline->addSampler2D("rocks", 2);
		pipeline->addSampler2D("sand", 3);

		pipeline = createShaderPipeline("terrain_clipmap_shader", "terrain/terrain_clipmap");
		pipeline->addSampler2D("texture0", 0);
		pipeline->addSampler2D("heightmap", 1);
		pipeline->addSampler2D("normalMap", 2);
		
		//
		// Create lighting entities here
		//
		
		//
		// Create entities here
		//
		
		// Wireframe rendering
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Terrains
		FBmTerrain* fbm = new FBmTerrain(6, 0.4f, 0.9f, 5.0f);
		//Terrain* terrain1 = createSimpleTerrain(Vec3(50,-40,0), 0, 0, fbm, LOD3);
		//Terrain* terrain2 = createSimpleTerrain(Vec3(-50,-40, 0), -256, 0, fbm, LOD3);
		//Terrain* terrain3 = createSimpleTerrain(Vec3(-50,-40,-100), -256, -256, fbm, LOD3);
		//Terrain* terrain4 = createSimpleTerrain(Vec3(50,-40,-100), 0, -256, fbm, LOD3);
		//Terrain* terrain5 = createSimpleTerrain(Vec3(150, -40, 0), 256, 0, fbm, LOD3);

		//terrain1->upgrade_to_LOD0();
		//terrain1->upgrade_to_LOD0();
		
		//terrain3->downgrade_to_LOD3();
		//terrain2->downgrade_to_LOD1();
		//terrain1->downgrade_to_LOD2();
		//terrain3->downgrade_to_LOD3();

		// Infinite Clipmap Terrain
		{
			uint32 entity = level->createEntity();
			level->template addToGroup<ForwardRender>(entity);
			
			level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("terrain_clipmap_shader"));
			
			// Terrain heightmap to upload
			Texture* island_texture = lmx::resManager->getTexture("island_heightmap.png");
			Texture* texture0 = lmx::resManager->getTexture("blank_checker.png");
			Material* material = new Material();
			material->addTexture("texture0", texture0);
			material->addTexture("heightmap", island_texture);
			material->addTexture("normalMap", texture0);
			
			TerrainClipmap* terrain = new TerrainClipmap();
			terrain->initialize();
			level->template addComponent<TerrainClipmap>(entity, terrain);
			level->template addComponent<SimpleLightComponent>(entity, lcomponent);
			level->template addComponent<Material>(entity, material);

			level->template registerEntity(entity);
			level->template registerEntityForRender3D<TypeRenderSystemForwardTerrainClipmap>(entity);
		}
		
		// Infinite plane floor
		{
			uint32_t entity = level->createEntity();
			level->template addToGroup<ForwardRender>(entity);

			level->template addComponent<Model3D>(entity, lmx::resManager->getModel3D("cube"));
			Material* material = new Material(lmx::resManager->getTexture("test.png"));
			level->template addComponent<Material>(entity, material);
			level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3Dlighting"));
			level->template addComponent<Transform3D>(entity, new Transform3D(Vec3(0,-20,0), Vec3(0,0,0), Vec3(2000,1,2000)));
			
			// Physics
			// Plane floor
			rp3d::Transform transform(Vec3(0,-20,0), rp3d::Quaternion::identity());
			rp3d::RigidBody* rigid_body = level->template getPhysicsWorld()->makeStaticRigidBody(transform);
			rp3d::BoxShape* plane = new rp3d::BoxShape(Vec3(2000,1,2000));
			rigid_body->addCollisionShape(plane, rp3d::Transform::identity(), 1.0f);
			rigid_body->enableGravity(false);
			level->template addComponent<rp3d::RigidBody>(entity, rigid_body);
			//

			level->template addComponent<SimpleLightComponent>(entity, lcomponent);
			
			level->template registerEntity(entity);
			level->template registerEntityForRender3D<TypeRenderSystemForwardLight3D>(entity);
		}
		// Entity cube3D 1
		{
			uint32 entity = level->createEntity();
			level->template addToGroup<ForwardRender>(entity);
			level->template addComponent<Model3D>(entity, lmx::resManager->getModel3D("cube"));
			level->template addComponent<Material>(entity, lmx::resManager->getMaterial("test_texture"));
			level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3Dlighting"));
			level->template addComponent<Transform3D>(entity, new Transform3D(Vec3(0,0,15)));
			level->template addComponent<SimpleLightComponent>(entity, lcomponent);

			// ------------------- //
			// Physics
			//
			// RigidBody
			rp3d::Quaternion orientation = rp3d::Quaternion::identity();
			rp3d::Transform transform(Vec3(0,0,15), orientation);
			rp3d::RigidBody* rigid_body = level->template getPhysicsWorld()->makeRigidBody(transform);
			rigid_body->applyForceToCenterOfMass(Vec3(1500,1500,-300));
			// Box Shape
			rp3d::BoxShape* box = new rp3d::BoxShape(Vec3(1,1,1));
			rigid_body->addCollisionShape(box, rp3d::Transform::identity(), 3.0);
			level->template addComponent<rp3d::RigidBody>(entity, rigid_body);
			// ------------------- //
			
			level->template registerEntity(entity);
			level->template registerEntityForRender3D<TypeRenderSystemForwardLight3D>(entity);
		}
		// Entity sphere 1
		{
			uint32 entity = level->createEntity();
			level->template addToGroup<ForwardRender>(entity);
			level->template addComponent<Model3D>(entity, lmx::resManager->getModel3D("sphere"));
			level->template addComponent<Material>(entity, lmx::resManager->getMaterial("red_test_png"));
			level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3Dlighting"));
			level->template addComponent<Transform3D>(entity, new Transform3D(Vec3(10,2,12)));
			level->template addComponent<SimpleLightComponent>(entity, lcomponent);

			//
			// Physics
			//
			rp3d::Quaternion orientation = rp3d::Quaternion::identity();
			rp3d::Transform transform(Vec3(10,2,12), orientation);
			rp3d::RigidBody* rigid_body = level->template getPhysicsWorld()->makeRigidBody(transform);
			rigid_body->applyForceToCenterOfMass(Vec3(0,700,0));
			rp3d::SphereShape* sphere = new rp3d::SphereShape(1.0);
			rigid_body->addCollisionShape(sphere, rp3d::Transform::identity(), 2.0);
			//rigid_body->enableGravity(false);
			level->template addComponent<rp3d::RigidBody>(entity, rigid_body);
			
			level->template registerEntity(entity);
			level->template registerEntityForRender3D<TypeRenderSystemForwardLight3D>(entity);
		}
		
		// Entity cube 2 for testing samplers
		{
			uint32 entity = level->createEntity();
			level->template addToGroup<ForwardRender>(entity);
			level->template addComponent<Model3D>(entity, lmx::resManager->getModel3D("cube"));

			// Test material for testing multiple samplers
			Material* material_test = new Material();
			Texture* texture1 = lmx::resManager->getTexture("test.png");
			Texture* texture2 = lmx::resManager->getTexture("gold.png");
			Texture* texture3 = lmx::resManager->getTexture("grass.png");
			material_test->addTexture("texSample", texture1);
			material_test->addTexture("texSample2", texture2);
			material_test->addTexture("texSample3", texture3);
			
			level->template addComponent<Material>(entity, material_test);

			level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3Dsimple"));
			level->template addComponent<Transform3D>(entity, new Transform3D(Vec3(-10,-10,-10)));
			level->template registerEntity(entity);
			level->template registerEntityForRender3D<TypeRenderSystemForwardTransform3D>(entity);
		}
		
		
		uint32_t cube0 = createTransparentCube(Vec3(0,1,0), Vec3(3,2,6), lcomponent);
		uint32_t cube1 = createTransparentCube(Vec3(-2,3,9), Vec3(1,0,0), lcomponent);
		uint32_t cube2 = createTransparentCube(Vec3(3,-1,4), Vec3(1,1,0), lcomponent);
		uint32_t cube3 = createTransparentCube(Vec3(-3,1,2), Vec3(1,0,1), lcomponent);
		uint32_t cube4 = createTransparentCube(Vec3(0,1,5), Vec3(0,1,1), lcomponent);
		
		// Normal mapped quad
		{
			uint32_t entity = level->createEntity();
			level->template addToGroup<ForwardRender>(entity);
			level->template addComponent<Model3D>(entity, lmx::resManager->getModel3D("quad_3D"));
			level->template addComponent<Material>(entity, lmx::resManager->getMaterial("bricks_normal_map"));
			//ShaderPipeline* pipeline = lmx::resManager->getShaderPipeline("forward3DlightingNM");
			//pipeline->bind();
			//pipeline->addSampler2D("texSample", 0);
			//pipeline->addSampler2D("texNormal", 1);
			//pipeline->setUniform1i("texSample", 0);
			//pipeline->setUniform1i("texNormal", 1);
			level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3DlightingNM"));
			
			level->template addComponent<Transform3D>(entity,
																								new Transform3D(Vec3(-1,-6,11),
																																Vec3(0,0,0),
																																Vec3(4,4,4)));
			level->template addComponent<SimpleLightComponent>(entity, lcomponent);
			level->template registerEntity(entity);
			level->template registerEntityForRender3D<TypeRenderSystemForwardLight3D>(entity);
		}
		
		// Water Tile
		{
			uint32_t entity = level->createEntity();
			level->template addToGroup<ForwardRender>(entity);
			level->template addComponent<Model3D>(entity, lmx::resManager->getModel3D("quad_3D"));
			
			// Creating the water material
			Material* material = new Material();
			material->addTexture("texSample", lmx::resManager->getTexture("test.png"));
			material->addTexture("reflection_texture", lmx::resManager->getTexture("grass.png"));
			material->addTexture("refraction_texture", lmx::resManager->getTexture("gold.png"));
			lmx::resManager->addMaterial("water_material", material);
			//std::cout << "texture0 = " << lmx::resManager->getTexture("gold.png") << std::endl;
			//std::cout << "reflection texture = " << lmx::resManager->getTexture("grass.png") << std::endl;
			//std::cout << "refraction texture = " << lmx::resManager->getTexture("test.png") << std::endl;
			
			level->template addComponent<Material>(entity, lmx::resManager->getMaterial("water_material"));
			
			//ShaderPipeline* water_pipeline = lmx::resManager->getShaderPipeline("water_shader");
			//water_pipeline->bind();
			//water_pipeline->addUniform("texSample");
			//water_pipeline->addUniform("reflection_texture");
			//water_pipeline->addUniform("refraction_texture");
			//water_pipeline->setUniform1i("texSample", 0);
			//water_pipeline->setUniform1i("reflection_texture", 1);
			//water_pipeline->setUniform1i("refraction_texture", 2);
			//water_pipeline->addSampler2D("texSample", 0);
			//water_pipeline->addSampler2D("reflection_texture", 1);
			//water_pipeline->addSampler2D("refraction_texture", 2);
			//ShaderPipeline* water_pipeline = new ShaderPipeline("renderer/water_shader");
			level->template addComponent<ShaderPipeline>(entity, lmx::resManager->getShaderPipeline("forward3Dwater"));
			
			level->template addComponent<Transform3D>(entity,
																								new Transform3D(Vec3(-20,-10,13),
																																Vec3(0,0,0),
																																Vec3(15,15,15)));
			level->template addComponent<SimpleLightComponent>(entity, lcomponent);
			level->template registerEntity(entity);
			level->template registerEntityForRender3D<TypeRenderSystemForwardWater3D>(entity);
		}

		level->template doDebugPrint();
	}
		
	
private:
};

#endif
