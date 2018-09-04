
#ifndef ECS_TPP
#define ECS_TPP

template <typename ECS, typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Signature>
void ECSManager<ECS, TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::addRenderComponent2D(uint32 entity, ShaderPipeline* pipeline, Material* material, Model2D* model, const std::string& name){
	// Add Model Component to entity
	addComponent<Model2D>(entity, model);
	addComponent<Material>(entity, material);
	
	// Check if pipeline2D already exist with given name
	auto it = mapPipelines2D.find(name);
	if(it != mapPipelines2D.end()){
		// Found a pipeline
		PipelineGroup2D<ECS>* group = it->second;
		
		// Check if material already has a MaterialGroup in PipelineGroup2D
		for(MaterialGroup2D* materialGroup : group->materials){
			if(materialGroup->material->getTexture()->getTextureID() == material->getTexture()->getTextureID()){
				// Found a material
				
				// Check to see if geometry group already exists
				for(GeometryGroup2D* geometryGroup : materialGroup->geometries){
					if(geometryGroup->mesh->getGeometryID() == model->getGeometryID()){
						// Found a matching geometry group
						geometryGroup->entities.push_back(entity);
						return;
					}
				}
				// Create a new GeometryGroup2D
				GeometryGroup2D* geometryGroup = new GeometryGroup2D(model);
				materialGroup->geometries.push_back(geometryGroup);
				geometryGroup->entities.push_back(entity);
				return;
			}
		}
		// Create a new MaterialGroup2D + GeometryGroup2D
		MaterialGroup2D* materialGroup = new MaterialGroup2D(material);
		group->materials.push_back(materialGroup);
		GeometryGroup2D* geometryGroup = new GeometryGroup2D(model);
		materialGroup->geometries.push_back(geometryGroup);
		geometryGroup->entities.push_back(entity);
		return;
	}
	// Create a new PipelineGroup2D + MaterialGroup2D + GeometryGroup2D
	PipelineGroup2D<ECS>* group = new PipelineGroup2D<ECS>(pipeline);
	// Init signature bitset from list of supported components
	init_pipeline_bitset<ECS, Signature>::execute(group);
	
	MaterialGroup2D* materialGroup = new MaterialGroup2D(material);
	group->materials.push_back(materialGroup);
	GeometryGroup2D* geometryGroup = new GeometryGroup2D(model);
	
	materialGroup->geometries.push_back(geometryGroup); // Crashes
	
	geometryGroup->entities.push_back(entity);
	
	// Add new group to map
	mapPipelines2D.insert(std::pair<std::string, PipelineGroup2D<ECS>*>(name, group));
	
	// Register new group through rendering systems
	registerPipeline2D(group);
}

template <typename ECS, typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList>
template <typename Signature>
void ECSManager<ECS, TSystemList, TRenderingSystem2DList, TRenderingSystem3DList>::addRenderComponent3D(uint32 entity, ShaderPipeline* pipeline, Material* material, Model3D* model, const std::string& name){
	// Add Model Component to entity
	addComponent<Model3D>(entity, model);
	addComponent<Material>(entity, material);
	
	// Check if pipeline2D already exist with given name
	auto it = mapPipelines3D.find(name);
	if(it != mapPipelines3D.end()){
		// Found a pipeline
		PipelineGroup3D<ECS>* group = it->second;
		
		// Check if material already has a MaterialGroup in PipelineGroup2D
		for(MaterialGroup3D* materialGroup : group->materials){
			//if(!materialGroup->material->getTexture() || (materialGroup->material->getTexture()->getTextureID() == material->getTexture()->getTextureID())){
			if(materialGroup->material == material){
				// Found a material
				
				// Check to see if geometry group already exists
				for(GeometryGroup3D* geometryGroup : materialGroup->geometries){
					if(geometryGroup->mesh->getGeometryID() == model->getGeometryID()){
						// Found a matching geometry group
						geometryGroup->entities.push_back(entity);
						return;
					}
				}
				// Create a new GeometryGroup2D
				GeometryGroup3D* geometryGroup = new GeometryGroup3D(model);
				materialGroup->geometries.push_back(geometryGroup);
				geometryGroup->entities.push_back(entity);
				return;
			}
		}
		// Create a new MaterialGroup2D + GeometryGroup2D
		MaterialGroup3D* materialGroup = new MaterialGroup3D(material);
		group->materials.push_back(materialGroup);
		GeometryGroup3D* geometryGroup = new GeometryGroup3D(model);
		materialGroup->geometries.push_back(geometryGroup);
		geometryGroup->entities.push_back(entity);
		return;
	}
	// Create a new PipelineGroup2D + MaterialGroup2D + GeometryGroup2D
	PipelineGroup3D<ECS>* group = new PipelineGroup3D<ECS>(pipeline);
	// Init signature bitset from list of supported components
	init_pipeline_bitset<ECS, Signature>::execute(group);
	
	MaterialGroup3D* materialGroup = new MaterialGroup3D(material);
	group->materials.push_back(materialGroup);
	GeometryGroup3D* geometryGroup = new GeometryGroup3D(model);
	
	materialGroup->geometries.push_back(geometryGroup); // Crashes
	
	geometryGroup->entities.push_back(entity);
	
	// Add new group to map
	mapPipelines3D.insert(std::pair<std::string, PipelineGroup3D<ECS>*>(name, group));
	
	// Register new group through rendering systems
	registerPipeline3D(group);
}

#endif
