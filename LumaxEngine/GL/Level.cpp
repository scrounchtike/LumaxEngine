
#include "Level.hpp"

#include "../RAL/Log.hpp"

Level::Level(Renderer* renderer, Player* player, ResourceManager* resManager) 
	: renderer(renderer), player(player), resManager(resManager)
{
}

Level::~Level() {

}

void Level::addModel3D(Model3D* mesh, ShaderPipeline* pipeline, Material* material, Transform3D* transform, const std::string& shaderName){
	auto it = mapGroups3D.find(shaderName);
	if(it != mapGroups3D.end()){
		// Found an existing shader group
		PipelineGroup3D* group = it->second;
		unsigned int textureID = material->getTexture()->getTextureID();

		// Check for existing material
		int i = 0;
		for(; i < group->materials.size(); ++i){
			if(group->materials[i]->material->getTexture()->getTextureID() == textureID){
				// Found an already existing corresponding material
				MaterialGroup3D* materialGroup = group->materials[i];
				unsigned int geometryID = mesh->getGeometryID();
				// Search for existing geometry
				int j = 0;
				for(; j < materialGroup->geometries.size(); ++j){
					if(materialGroup->geometries[j]->getGeometryID() == geometryID){
						// Found existing matching geometry ID
						materialGroup->geometries[j]->transforms.push_back(transform);
						return;
					}
				}
				if(j == materialGroup->geometries.size()){
					// Create new geometry group
					GeometryGroup3D* geometryGroup = new GeometryGroup3D(mesh);
					materialGroup->geometries.push_back(geometryGroup);
					geometryGroup->transforms.push_back(transform);
				}
				return;
			}
		}
		if(i == group->materials.size()){
			// Create new material group
			MaterialGroup3D* materialGroup = new MaterialGroup3D(material);
			group->materials.push_back(materialGroup);
			GeometryGroup3D* geometryGroup = new GeometryGroup3D(mesh);
			materialGroup->geometries.push_back(geometryGroup);
			geometryGroup->transforms.push_back(transform);
		}
		return;
	}
	// Create new shader group
	PipelineGroup3D* group = new PipelineGroup3D(pipeline);
	MaterialGroup3D* materialGroup = new MaterialGroup3D(material);
	group->materials.push_back(materialGroup);
	GeometryGroup3D* geometryGroup = new GeometryGroup3D(mesh);
	materialGroup->geometries.push_back(geometryGroup);
	geometryGroup->transforms.push_back(transform);

	mapGroups3D.insert(std::pair<std::string, PipelineGroup3D*>(shaderName, group));
	groups3D.push_back(group);
}

void Level::addModel2D(Model2D *mesh, ShaderPipeline* pipeline, Material *material, Transform2D *transform, const std::string& shaderName){
	auto it = mapGroups2D.find(shaderName);
	if(it != mapGroups2D.end()){
		// Found an existing shader group
		PipelineGroup2D* group = it->second;
		unsigned int textureID = material->getTexture()->getTextureID();

		// Check for existing material
		int i = 0;
		for(; i < group->materials.size(); ++i){
			if(group->materials[i]->material->getTexture()->getTextureID() == textureID){
				// Found an already existing corresponding material
				MaterialGroup2D* materialGroup = group->materials[i];
				unsigned int geometryID = mesh->getGeometryID();
				// Search for existing geometry
				int j = 0;
				for(; j < materialGroup->geometries.size(); ++j){
					if(materialGroup->geometries[j]->getGeometryID() == geometryID){
						// Found existing matching geometry ID
						materialGroup->geometries[j]->transform = transform;
						return;
					}
				}
				if(j == materialGroup->geometries.size()){
					// Create new geometry group
					GeometryGroup2D* geometryGroup = new GeometryGroup2D(mesh);
					materialGroup->geometries.push_back(geometryGroup);
					geometryGroup->transform = transform;
				}
				return;
			}
		}
		if(i == group->materials.size()){
			// Create new material group
			MaterialGroup2D* materialGroup = new MaterialGroup2D(material);
			group->materials.push_back(materialGroup);
			GeometryGroup2D* geometryGroup = new GeometryGroup2D(mesh);
			materialGroup->geometries.push_back(geometryGroup);
			geometryGroup->transform = transform;
		}
		return;
	}
	// Create new shader group
	PipelineGroup2D* group = new PipelineGroup2D(pipeline);
	MaterialGroup2D* materialGroup = new MaterialGroup2D(material);
	group->materials.push_back(materialGroup);
	GeometryGroup2D* geometryGroup = new GeometryGroup2D(mesh);
	materialGroup->geometries.push_back(geometryGroup);
	geometryGroup->transform = transform;

	mapGroups2D.insert(std::pair<std::string, PipelineGroup2D*>(shaderName, group));
	groups2D.push_back(group);
}

void Level::addInstancedModel3D(Model3D* mesh, ShaderPipeline* pipeline, Material* material, const std::vector<Vec2>& positions, const std::string& shaderName){
	
}

void Level::addInstancedModel2D(Model2D *mesh, ShaderPipeline *pipeline, Material *material, const std::vector<Vec2>& positions, const std::string &shaderName){
  auto it = mapGroups2D.find(shaderName);
	if(it != mapGroups2D.end()){
		// Found an existing shader group
		PipelineGroup2D* group = it->second;
		unsigned int textureID = material->getTexture()->getTextureID();

		// Check for existing material
		int i = 0;
		for(; i < group->materials.size(); ++i){
			if(group->materials[i]->material->getTexture()->getTextureID() == textureID){
				// Found an already existing corresponding material
				MaterialGroup2D* materialGroup = group->materials[i];
				unsigned int geometryID = mesh->getGeometryID();
				// Search for existing geometry
				int j = 0;
				for(; j < materialGroup->geometries.size(); ++j){
					if(materialGroup->geometries[j]->getGeometryID() == geometryID){
						// Found existing matching geometry ID
						for(int k = 0; k < positions.size(); ++k)
							materialGroup->geometries[j]->instancedTransforms.push_back(Vec4(positions[k].x, positions[k].y, 0.0, 1.0));
						return;
					}
				}
				if(j == materialGroup->geometries.size()){
					// Create new geometry group
					GeometryGroup2D* geometryGroup = new GeometryGroup2D(mesh);
					materialGroup->geometries.push_back(geometryGroup);
					geometryGroup->isInstanced = true;
					for(int k = 0; k < positions.size(); ++k)
						geometryGroup->instancedTransforms.push_back(Vec4(positions[k].x, positions[k].y, 0.0, 1.0));
					
				}
				return;
			}
		}
		if(i == group->materials.size()){
			// Create new material group
			MaterialGroup2D* materialGroup = new MaterialGroup2D(material);
			group->materials.push_back(materialGroup);
			GeometryGroup2D* geometryGroup = new GeometryGroup2D(mesh);
			materialGroup->geometries.push_back(geometryGroup);
			geometryGroup->isInstanced = true;
			for(int k = 0; k < positions.size(); ++k)
				geometryGroup->instancedTransforms.push_back(Vec4(positions[k].x, positions[k].y, 0.0, 1.0));
		}
		return;
	}
	// Create new shader group
	PipelineGroup2D* group = new PipelineGroup2D(pipeline);
	MaterialGroup2D* materialGroup = new MaterialGroup2D(material);
	group->materials.push_back(materialGroup);
	GeometryGroup2D* geometryGroup = new GeometryGroup2D(mesh);
	materialGroup->geometries.push_back(geometryGroup);
	geometryGroup->isInstanced = true;
	for(int k = 0; k < positions.size(); ++k)
		geometryGroup->instancedTransforms.push_back(Vec4(positions[k].x, positions[k].y, 0.0, 1.0));

	mapGroups2D.insert(std::pair<std::string, PipelineGroup2D*>(shaderName, group));
	groups2D.push_back(group);
}

// Lights

DirectionalLight* Level::addDirectionalLight(DirectionalLight* light) {
	lights.directionalLights.push_back(light);
	return light;
}

PointLight* Level::addPointLight(PointLight* light){
	lights.pointLights.push_back(light);
	return light;
}

SpotLight* Level::addSpotLight(SpotLight* light){
	lights.spotLights.push_back(light);
	return light;
}

DirectionalLight* Level::addDeferredDirectionalLight(DirectionalLight *light){
	deferredLights.directionalLights.push_back(light);
	return light;
}

PointLight* Level::addDeferredPointLight(PointLight* light){
	deferredLights.pointLights.push_back(light);
	return light;
}

SpotLight* Level::addDeferredSpotLight(SpotLight *light){
	deferredLights.spotLights.push_back(light);
	return light;
}

Movement3D* Level::addMovement3D(Movement3D* movement) {
	movements3D.push_back(movement);
	return movement;
}

void Level::update() {
	player->input();
	player->update();

	updateLevelTest();

	// Physics collision tests
	for(int i = 0; i < dynamicMeshes3D.size(); ++i){
		Mesh3D* dynamicMesh = dynamicMeshes3D[i];
		for(int j = 0; j < staticMeshes3D.size(); ++j){
			Mesh3D* staticMesh = staticMeshes3D[j];
			ContactManifold manifold;
			dynamicMesh->physics->dynamicCollidesWithStatic(staticMesh->physics, manifold);
			if(manifold.isIntersecting){
				// Resolve interpenetration
				dynamicMesh->addTranslation(manifold.normal * manifold.penetration);
			}
		}
	}

	// Physics update!
	/*
	for (int i = 0; i < physicsPrimitives.size(); ++i) {
		physicsPrimitives[i]->material->colorIndex = 0;
	}
	// Perform Collision Detection in O^2
	for (int i = 0; i < physicsPrimitives.size(); ++i) {
		PhysicsPrimitive* pFirst = physicsPrimitives[i]->physics;
		assert(pFirst);
		for (int j = i + 1; j < physicsPrimitives.size(); ++j) {
			PhysicsPrimitive* pSecond = physicsPrimitives[j]->physics;
			assert(pSecond);
			bool collided = pFirst->collidesWith(pSecond);
			if (collided) {
				physicsPrimitives[i]->material->colorIndex = 1;
				physicsPrimitives[j]->material->colorIndex = 1;
			}
		}
	}
	*/
}

void Level::updateLevelTest() {
	/*
	std::map<std::string, Mesh2D*>::const_iterator it = mapMeshes2D.find("mesh1");
	if (it == mapMeshes2D.end())
		return;
	Mesh2D* mesh1 = it->second;
	DynamicTransform2D* transform = (DynamicTransform2D*)mesh1->getTransform();
	transform->rotation += 1;
	transform->updateMatrix();
	*/

	//Mesh3D* aabb2 = mapAABBs.find("aabb2")->second;
	//AABB* aabb = (AABB*)aabb2->getPhysics();
	//aabb->position -= Vec3(0.01f, 0, 0);

	//Mesh3D* sphere2 = mapSpheres.find("sphere2")->second;
	//Sphere* sphere = (Sphere*)sphere2->getPhysics();
	//sphere->position -= Vec3(0.015f, 0, 0);

	//Mesh3D* meshOBB1 = mapOBBs.find("obb1")->second;
	//OBB* obb1 = (OBB*)meshOBB1->getPhysics();
	//obb1->position += Vec3(0, 0, 0.01f);

	//Mesh3D* meshOBB2 = mapOBBs.find("obb2")->second;
	//OBB* obb2 = (OBB*)meshOBB2->getPhysics();
	//obb2->position -= Vec3(0, 0.0001f, 0);

	for (Movement3D* movement : movements3D) {
		movement->update();
	}
}


void Level::render() const {
	//renderer->renderGroups3D(groups3D);
	renderer->renderGroups2D(groups2D);
}
