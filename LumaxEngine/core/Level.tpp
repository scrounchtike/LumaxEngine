
#ifndef LEVEL_TPP
#define LEVEL_TPP

#include "../RL/Renderer.hpp"
#include "../utils/Timing.hpp"

//
// Level constructor Level()
//
// Initializes a new player and renderer with a new camera
//
// Initializes all systems in SystemList, RenderingSystem32List and
// RenderingSystem3DList
// 
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::Level() :
	renderer(new Renderer(new Player(new Camera()))),
	physics_world(PhysicsWorld())
{
	// Initialize all systems : instances unique to this level
	// Execute constructors of all the systems
	for_each<SystemList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			std::get<T>(systems) = T();
			smpl::call_if_exists<T, smpl::has_construct<T>>(&std::get<T>(systems));
		});
	for_each<RenderingSystem2DList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			std::get<T>(renderSystems2D) = T();
				smpl::call_if_exists<T, smpl::has_construct<T>>(&std::get<T>(renderSystems2D));
		});
	for_each<RenderingSystem3DList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			std::get<T>(renderSystems3D) = T();
			smpl::call_if_exists<T, smpl::has_construct<T>>(&std::get<T>(renderSystems3D));
		});
}

//
// Level getCamera() function
//
// Returns a pointer to the unique camera
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
Camera* Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::getCamera() const
{
	return renderer->getCamera();
}

//
// Level createEntity function
//
// Pushes back a new Entity in entities vector
// Returns the id of the created entity
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
uint32 Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::createEntity()
{
	entities.push_back(Entity<ECSDesc>());
	return entities.size() - 1;
}

//
// Level remoteEntity function
//
// Removes the entity by its ID
// *Not yet implemented*
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::removeEntity(uint32 entity)
{
	// TODO: Remove entity
}

//
// Level registerEntity function
//
// After an entity has been created and configured,
// it needs to be manually registered into all systems
// that match the entity signature (subset of the system signature)
//
// This is NOT done at entity creation since the created entity is not ready per se
//
// Creation and Registering are different actions that need to be done for an
// entity to be updated/rendered by our systems
//
// For rendering systems, the entities are sorted when registered into a render
// system by the flags, shader, material and model ids.
// This way, similar models will be rendered in batches, hopefully reducing the
// amount of state changes between shaders, textures and meshes.
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::registerEntity(uint32 entity)
{
	// Register entity into non-rendering systems
	for_each<typename TSystemList::type>::execute([this, entity](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			T& system = std::get<T>(systems);
			if((system.bitset & entities[entity].bitset) == system.bitset){
				system.entities.push_back(entity);
			}
		});

	//
	// NOTE: I've decided to NOT register entities into rendering system by default.
	//       Entities have to be added to rendering systems manually.
	//       This makes it just SO much easier since entites should only
	//       be added to one render system. So let the user choose which one.
	//

	/*
	// Register entity into Rendering Systems 2D
	for_each<typename TRenderingSystem2DList::type>::execute([this, entity](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			T& system = std::get<T>(renderSystems2D);
			if((system.bitset & entities[entity].bitset) == system.bitset){
				// Calculate entity hint value
				uint16 modelID = 0;
				uint16 materialID = 0;
				uint16 pipelineID = 0;
				if(hasComponent<Model2D>(entity))
					modelID = 0; // Add support for model2D IDs
					//modelID = getComponent<Model2D>(entity)->ID;
				if(hasComponent<Material>(entity))
					materialID = getComponent<Material>(entity)->ID;
				if(hasComponent<ShaderPipeline>(entity))
					pipelineID = getComponent<ShaderPipeline>(entity)->ID;
				uint16 flags = 0;
				
				uint64 entityHint = ((uint64)flags << 48) + ((uint64)pipelineID << 32) + ((uint64)materialID << 16) + (uint64)modelID;
				system.entities.insert(std::pair<uint64, uint32>(entityHint, entity));
			}
		});
	std::cout << "** Trying to register the entity with 3D rendering systems **" << std::endl;
	// Register entity into Rendering Systems 3D
	for_each<typename TRenderingSystem3DList::type>::execute([this, entity](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			T& system = std::get<T>(renderSystems3D);
			std::cout << "Checking entity against bitset" << std::endl;
			std::cout << std::hex << system.bitset << std::endl;
			std::cout << std::hex << entities[entity].bitset << std::endl;
			std::cout << ECSDesc::template getComponentID<Model3D>() << std::endl;
			std::cout << ECSDesc::template getComponentID<ShaderPipeline>() << std::endl;
			std::cout << ECSDesc::template getComponentID<Material>() << std::endl;
			std::cout << ECSDesc::template getGroupID<ForwardRender>() << std::endl;
			if((system.bitset & entities[entity].bitset) == system.bitset){
				// Calculate entity hint value
				uint16 modelID = 0;
				uint16 materialID = 0;
				uint16 pipelineID = 0;
				if(hasComponent<Model3D>(entity))
					modelID = getComponent<Model3D>(entity)->ID;
				if(hasComponent<Material>(entity))
					materialID = getComponent<Material>(entity)->ID;
				if(hasComponent<ShaderPipeline>(entity))
					pipelineID = getComponent<ShaderPipeline>(entity)->ID;
				
				uint16 flags = 0;
				
				// Check for translucency
				bool translucent_material = getComponent<Material>(entity)->translucent;
				flags |= (translucent_material << 8);
				
				uint64 entityHint = ((uint64)flags << 48) + ((uint64)pipelineID << 32) + ((uint64)materialID << 16) + (uint64)modelID;

				// The system performs metadata gathering of the entities when they
				// are inserted with the hint
				system.addEntity(entityHint, entity);
				//system.entities.insert(std::pair<uint64, uint32>(entityHint, entity));
				
				// Entity is now valid
				entities[entity].isValid = true;
			}
		});
	*/
}


//
// Level registerEntityForRender2D function
//
// Adds an entity to a rendering system provided as a template typename
//
// Makes it explicit how entities will be rendered
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename RenderingSystem>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::registerEntityForRender2D(uint32 entity)
{
	auto action = [this, entity](auto* t){
		typedef typename std::remove_reference<decltype(*t)>::type T;
		T& system = std::get<T>(renderSystems2D);
		if((system.bitset & entities[entity].bitset) == system.bitset){
			// Calculate entity hint value
			uint16 modelID = 0;
			uint16 materialID = 0;
			uint16 pipelineID = 0;
			if(hasComponent<Model2D>(entity))
				modelID = 0; // Add support for model2D IDs
			//modelID = getComponent<Model2D>(entity)->ID;
			if(hasComponent<Material>(entity))
				materialID = getComponent<Material>(entity)->ID;
			if(hasComponent<ShaderPipeline>(entity))
				pipelineID = getComponent<ShaderPipeline>(entity)->ID;
			uint16 flags = 0;
			
			uint64 entityHint = ((uint64)flags << 48) + ((uint64)pipelineID << 32) + ((uint64)materialID << 16) + (uint64)modelID;
			system.entities.insert(std::pair<uint64, uint32>(entityHint, entity));
		}
	};
	action((RenderingSystem*)nullptr);
}


//
// Level registerEntityForRender3D function
//
// Adds an entity to a rendering system provided as a template typename
//
// Makes it explicit how entities will be rendered
//
// WARNING : This does not (yet) check for entities added to multiple
//           rendering systems (possible TODO)
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename RenderingSystem>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::registerEntityForRender3D(uint32 entity)
{
	auto action = [this, entity](auto* t)
	{
		typedef typename std::remove_reference<decltype(*t)>::type T;
		T& system = std::get<T>(renderSystems3D);
		std::cout << "Checking entity against bitset" << std::endl;
		std::cout << std::hex << system.bitset << std::endl;
		std::cout << std::hex << entities[entity].bitset << std::endl;
		std::cout << ECSDesc::template getComponentID<Model3D>() << std::endl;
		std::cout << ECSDesc::template getComponentID<ShaderPipeline>() << std::endl;
		std::cout << ECSDesc::template getComponentID<Material>() << std::endl;
		std::cout << ECSDesc::template getGroupID<ForwardRender>() << std::endl;
		if((system.bitset & entities[entity].bitset) == system.bitset){
			// Calculate entity hint value
			uint16 modelID = 0;
			uint16 materialID = 0;
			uint16 pipelineID = 0;
			if(hasComponent<Model3D>(entity))
				modelID = getComponent<Model3D>(entity)->ID;
			if(hasComponent<Material>(entity))
				materialID = getComponent<Material>(entity)->ID;
			if(hasComponent<ShaderPipeline>(entity))
				pipelineID = getComponent<ShaderPipeline>(entity)->ID;
				
			uint16 flags = 0;
				
			// Check for translucency
			bool translucent_material = getComponent<Material>(entity)->translucent;
			flags |= (translucent_material << 8);
				
			uint64 entityHint = ((uint64)flags << 48) + ((uint64)pipelineID << 32) + ((uint64)materialID << 16) + (uint64)modelID;

			// The system performs metadata gathering of the entities when they
			// are inserted with the hint
			system.addEntity(entityHint, entity);
			//system.entities.insert(std::pair<uint64, uint32>(entityHint, entity));
				
			// Entity is now valid
			entities[entity].isValid = true;
		}
	};
	action((RenderingSystem*)nullptr);
}


//
// Level addComponent function
//
// Adds a component to an entity dynamically (meaning the entity
// does not need to be removed, readded and re-registered
//
// ***
// IMPORTANT : Entity then needs to be registered again for updating its systems !
// After this call, the entity becomes useless (isValid = false) and will not
// be updated until re-registering.
// * It will be registered automatically at next update call *
// ***
// 
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename Component>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::addComponent(uint32 entity, Component* component)
{
	// Set entity as non-valid
	entities[entity].isValid = false;
	
	// Update bitset of entity
	entities[entity].bitset[ECSDesc::template getComponentID<Component>()] = true;
	// Add component into component storage
	std::vector<Component*>& componentStorage = std::get<std::vector<Component*>>(components);
	uint32 location = componentStorage.size();
	
	// Construct new Component at the end of Component array
	componentStorage.push_back(component);
	
	// Map the new component to location returned from ECS
	entities[entity].map.insert(std::pair<uint32, void*>(ECSDesc::template getComponentID<Component>(), (void*)component));
}

//
// Level removeComponent function
// 
// Removes a component dynamically from an entity
//
// IMPORTANT : Same as with addComponent
// The entity is now useless and not updated
// Entity needs to be re-registered through registerEntity() for updating its systems
// * It will be registered automatically at next update call *
//
// *Not yet implemented*
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename Component>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::removeComponent(uint32 entity)
{
	// TODO: Remove component
}

//
// Level getComponent function
//
// Iterates over the entity component map and tries to find a component with the ID
// The returns a Component* to this component
//
// Returns a nullptr if the component does not exist
// 
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename Component>
Component* Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::getComponent(uint32 entity) const
{
	typename ComponentMap::const_iterator it = entities[entity].map.find(ECSDesc::template getComponentID<Component>());
	
	// Did we get a hit ? If not, return nullptr, else return the component pointer
	if(it == entities[entity].map.end())
		return nullptr;
	return (Component*)it->second;
}

//
// Level hasComponent function
//
// Returns true if entity has component
// Returns false otherwise
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename Component>
bool Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::hasComponent(uint32 entity) const
{
	auto it = entities[entity].map.find(ECSDesc::template getComponentID<Component>());
		return it != entities[entity].map.end();
}

//
// Level addToGroup function
//
// Updates the bitset (signature) of the entity to be added or removed from a group
// according to the bool value passed in
//
// Entity becomes useless and needs to be re-registered again
// * It will be registered automatically at next update call *
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename Group>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::addToGroup(uint32 entity, bool value)
{
	// Set entity as non-valid
	entities[entity].isValid = false;
	
	entities[entity].bitset[ECSDesc::template getGroupID<Group>()] = value;
}

//
// Level setFlag
//
// Updates the bitset (signature) of an entity for a specific flag
//
// Entity becomes useless and needs to be re-registered again
// * It will be registered automatically at next update call *
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename Flag>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::setFlag(uint32 entity, bool value)
{
	// Set entity as non-valid
	entities[entity].isValid = false;
	
	entities[entity].flags[ECSDesc::template getFlagID<Flag>()] = value;
}

//
// Level input function
// 
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::input()
{
	renderer->input();
}

//
// Level update function
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::update()
{
	// Register all entities that have changed (or been added) in the last frame
	for(int i = 0; i < entities.size(); ++i){
		if(!entities[i].isValid)
			registerEntity(i), entities[i].isValid = true;
	}

	// DOOM demo specific calls (should be rewritten in a system !)
	/*
	// DOOM Collision detection with walls
	Vec3 position = renderer->getCamera()->getPosition();
	// TODO: Add getDeltaMovement function in camera before movement addition
	Vec3 newPos = position;// + deltaMovement();

	// Ray trace downwards
	for(int i = 0; i < floors.size(); ++i){
		uint32 floorID = floors[i];
		// 1. Find if 2D xz position intersects floor
		DOOM_Floor* floorComp = getComponent<DOOM_Floor>(floorID);
		float floorY = floorComp->y;
		Vec2 pos2D = Vec2(newPos.x, newPos.z);

		bool intersects = PrimitiveCollision::Point_intersects_Rectangle(floorComp->A, floorComp->B, floorComp->C, floorComp->D, pos2D);
		if(!intersects)
			continue;
		newPos.y = floorY;
		
		// 2. If so, calculate deltaY
		//float deltaY = std::abs(position.y - floorY);
	}
	*/
	
	//
	// Physics update
	//
	updatePhysics();
	
	/*
	for(int i = 0; i < physicsEntities.size(); ++i){
		setFlag<FlagCollided>(physicsEntities[i], false);
		setFlag<FlagNotCollided>(physicsEntities[i], true);
	}
	// Update physics simulation
	for(int i = 0; i < physicsEntities.size(); ++i){
		PhysicsPrimitive* primA = getComponent<PhysicsPrimitive>(physicsEntities[i]);
		for(int j = i+1; j < physicsEntities.size(); ++j){
			// Check for any collision
			//ContactManifold manifold = intersects<PhysicsTypeA, PhysicsTypeB>::execute(a, b);
			// Need to get the PhysicsPrimitive from entity
			PhysicsPrimitive* primB = getComponent<PhysicsPrimitive>(physicsEntities[j]);
			
			// Collision detection
			bool collides = primA->collidesWith(primB);
			if(collides){
				// Update collision flag
				setFlag<FlagCollided>(physicsEntities[i], true);
				setFlag<FlagCollided>(physicsEntities[j], true);
				setFlag<FlagNotCollided>(physicsEntities[i], false);
				setFlag<FlagNotCollided>(physicsEntities[j], false);
			}
		}
	}
	*/
	// Camera update
	renderer->update();

	// System list update
  for_each<SystemList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, UPDATE_TYPE, typename T::Signature::type> caller;
			// Loop over entities the system has to manage
			T& system = std::get<T>(systems);
			for(int i = 0; i < system.entities.size(); ++i){
				// Check if required flags match
				if((system.flags & this->entities[system.entities[i]].flags) == system.flags)
					caller.template call(this, system.entities[i], T::update);
			}
		});
}

//
// Level update physics function
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::updatePhysics()
{
	const float time_step = 1.0f / 60.0f;
	const float time_step_us = time_step * 1000000.0f;
	
	static double accumulator = 0.0;
	static double last_frame_time = lmx::getCurrentTimeMicroSeconds();
	double current_frame_time = lmx::getCurrentTimeMicroSeconds();

	double delta_time = current_frame_time - last_frame_time;
	last_frame_time = current_frame_time;

	accumulator += delta_time;

	while(accumulator >= time_step_us)
	{
		accumulator -= time_step_us;
		
		// Perform ReactPhysics3D dynamics world update:
		physics_world.update(time_step);
	}
}

//
// level render function
//
template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::render()
{
	// Go over all render passes
	for_each<RenderPass3DList>::execute([this](auto* t)
	{
		typedef typename std::remove_reference<decltype(*t)>::type T;
		
		// Call the update method of the render pass with all entities
		std::vector<uint32> entities;
		for_each<typename T::System3DListToOmit::type>::execute([this, &entities](auto* t)
		{
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, RENDER3D_TYPE, typename T::Signature::type> caller;
			// Loop over entities the system has to manage
			T& system = std::get<T>(renderSystems3D);
			// TODO: To consider :
			// Should flags be treated the same in render passes vs system updates/render ?
			for(auto it = system.entities.begin(); it != system.entities.end(); ++it)
			{
				// Check flags
				if((system.flags & this->entities[it->second].flags) == system.flags)
					entities.push_back(it->second);
			}
		});

		// For testing purposes, helper to see systems in both categories
		/*
		std::cout << "start" << std::endl;
		for_each<typename T::System3DListToOmit::type>::execute([](auto* t){
				typedef typename std::remove_reference<decltype(*t)>::type T;
				std::cout << "to omit = " << typeid(T).name() << std::endl;
			});
		for_each<typename T::System3DListToRender::type>::execute([](auto* t){
				typedef typename std::remove_reference<decltype(*t)>::type T;
				std::cout << "to render = " << typeid(T).name() << std::endl;
			});
		*/
		
		SystemCaller<ThisType, ECSDesc, RENDER3D_TYPE, typename T::ComponentList> caller;
		caller.template call(this, entities, T::update);
		
		// Perform the rendering of the entities
		for_each<typename T::System3DListToOmit::type>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, RENDER3D_TYPE, typename T::Signature::type> caller;
			
			// Loop over entities the system has to manage
			T& system = std::get<T>(renderSystems3D);
			std::vector<uint32> entities;
			for(auto it = system.entities.begin(); it != system.entities.end(); ++it){
				// Check flags
				if((system.flags & this->entities[it->second].flags) == system.flags)
					entities.push_back(it->second);
			}

			// Use polymorphism to do other processing to entities before rendering
			system.template updateEntityVector<ThisType>(*this, entities, getCamera());
			
			caller.template call(this, entities, T::update);
		});
	});
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename Targets>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::renderPass()
{
	// Update rendering systems 2D
	for_each<RenderingSystem2DList>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, RENDER2D_TYPE, typename T::Signature::type> caller;
			// Check that T is inside the targets
			if(!smpl::contains<Targets, T>())
				return;
			
			// Loop over entities the system has to manage
			T& system = std::get<T>(renderSystems2D);
			std::vector<uint32> entities;
			for(auto it = system.entities.begin(); it != system.entities.end(); ++it){
				// Check flags
				if((system.flags & this->entities[it->second].flags) == system.flags)
					entities.push_back(it->second);
			}
			caller.template call(this, entities, T::update);
		});
	
	// Update rendering systems 3D
	//
	//using TargetList = typename SystemList<Targets...>::type;
  for_each<Targets>::execute([this](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			SystemCaller<ThisType, ECSDesc, RENDER3D_TYPE, typename T::Signature::type> caller;
			// Check that T is inside the targets
			if(!smpl::contains<Targets, T>())
				return;
			
			// Loop over entities the system has to manage
			T& system = std::get<T>(renderSystems3D);
			std::vector<uint32> entities;
			for(auto it = system.entities.begin(); it != system.entities.end(); ++it){
				// Check flags
				if((system.flags & this->entities[it->second].flags) == system.flags)
					entities.push_back(it->second);
			}

			// Use polymorphism to do other processing to entities before rendering
			system.template updateEntityVector<ThisType>(*this, entities, getCamera());
			
			caller.template call(this, entities, T::update);
		});
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
template <typename CallerType>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::renderIntoFBO(Camera* camera)
{
	// Update rendering systems 3D
  for_each<RenderingSystem3DList>::execute([this, camera](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			
			// Since this function is called by systems that require FBO rendering,
			// discard the caller system
			if(typeid(T) == typeid(CallerType))
				return;
			
			SystemCaller<ThisType, ECSDesc, RENDER3D_TYPE, typename T::Signature::type> caller;
			
			// Loop over entities the system has to manage
			T& system = std::get<T>(renderSystems3D);
			std::vector<uint32> entities;
			for(auto it = system.entities.begin(); it != system.entities.end(); ++it){
				// Check flags
				if((system.flags & this->entities[it->second].flags) == system.flags)
					entities.push_back(it->second);
			}

			// Use polymorphism to do other processing to entities before rendering
			system.template updateEntityVector<ThisType>(*this, entities, camera);
			
			caller.template call(this, entities, T::update);
		});
}

template <typename TSystemList, typename TRenderingSystem2DList, typename TRenderingSystem3DList, typename TRenderPass2DList, typename TRenderPass3DList>
void Level<TSystemList, TRenderingSystem2DList, TRenderingSystem3DList, TRenderPass2DList, TRenderPass3DList>::doDebugPrint() const
{
	std::cout << " --------- ECS DEBUG ----------- " << "\n";
	std::cout << "ECS entity count = " << std::dec << entities.size() << "\n";
	for(int i = 0; i < entities.size(); ++i)
	{
		int total = 0;
		std::cout << "Bitset = " << entities[i].bitset.to_string() << "  " << entities[i].bitset.count() << "\n";
	}
	
	// Print all entities and systems in the system after initialization
	int i = 0;
	std::cout << " ----- SYSTEMS ----- " << "\n";
	std::cout << "System count = " << smpl::getSize<RenderingSystem3DList>() << "\n";
	std::cout << "System bitsets" << "\n";
	for_each<RenderingSystem3DList>::execute([this, &i](auto* t){
			typedef typename std::remove_reference<decltype(*t)>::type T;
			static T systemtest = T();
			
			std::cout << "system " << i << "\n";
			std::cout << "Bitset = " << systemtest.bitset.to_string() << "  " << systemtest.bitset.count() << "\n";
			
			++i;
		});
}

#endif
