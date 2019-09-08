
#ifndef RENDER_COMPONENT_STORAGE_HPP
#define RENDER_COMPONENT_STORAGE_HPP

#include <iostream>
#include <vector>
#include <map>

#include "../math/types.hpp"
#include "../utils/SMPL.hpp"

class ShaderPipeline;
class Model3D;
class Model2D;
class Material;

class GeometryGroup3D {
public:
	GeometryGroup3D(Model3D* mesh) : mesh(mesh) { }
	Model3D* mesh;
	bool isInstanced = false;
	
	unsigned int getGeometryID();
	
	std::vector<uint32> entities;
};

class MaterialGroup3D {
public:
	MaterialGroup3D(Material* material) : material(material) { }
	Material* material;
	bool hasTexture = false;
	bool hasColor = false;
	
	std::vector<GeometryGroup3D*> geometries;
};

struct GeometryGroup2D {
	GeometryGroup2D(Model2D* mesh) : mesh(mesh) { }
	Model2D* mesh;
	bool isInstanced = false;
	
	unsigned int getGeometryID();
	
	std::vector<uint32> entities;
};

struct MaterialGroup2D {
	MaterialGroup2D(Material* material) : material(material) { }
	Material* material;
	
	std::vector<GeometryGroup2D*> geometries;
};

template <typename ECS, typename B, typename T, int index>
struct BitsetSetter{
	static constexpr void execute(B& bitset) noexcept{
		bitset.set(index);
	}
};
template <typename ECS, typename B, typename T>
struct BitsetSetter<ECS, B, T, -1>{
	static constexpr void execute(B& bitset) noexcept{
	}
};

// Initialize bitsets from list of components and flags
template <typename ECS, typename B, typename F, typename T>
struct init_bitsets;
template <typename ECS, typename B, typename F, typename T, typename U>
struct init_bitsets<ECS, B, F, smpl::type_list_impl<T, U>> {
	static constexpr void execute(B& bitset, F& flags) noexcept {
		// Get Component T ID and update bitset
		BitsetSetter<ECS, B, T, ECS::template getComponentID<T>()>::execute(bitset);
		BitsetSetter<ECS, B, T, ECS::template getGroupID<T>()>::execute(bitset);
		BitsetSetter<ECS, F, T, ECS::template getFlagID<T>()>::execute(flags);
		
		// Recursively update bitset with all components
		init_bitsets<ECS, B, F, U>::execute(bitset, flags);
	}
};
template <typename ECS, typename B, typename F>
struct init_bitsets<ECS, B, F, smpl::type_list_null> {
	static constexpr void execute(B& bitset, F& flags) noexcept {}
};

// Initialize bitset from list of components
template <typename ECS, typename B, typename T>
struct init_bitset;
template <typename ECS, typename B, typename T, typename U>
struct init_bitset<ECS, B, smpl::type_list_impl<T, U>>{
	static constexpr void execute(B& bitset) noexcept {
		BitsetSetter<ECS, B, T, ECS::template getComponentID<T>()>::execute(bitset);
		BitsetSetter<ECS, B, T, ECS::template getGroupID<T>()>::execute(bitset);

		// Recursively update bitset with all components
		init_bitset<ECS, B, U>::execute(bitset);
	}
};
template <typename ECS, typename B>
struct init_bitset<ECS, B, smpl::type_list_null>{
	static constexpr void execute(B& bitset) noexcept {}
};

template <typename ECS>
class PipelineGroup3D {
public:
	PipelineGroup3D(ShaderPipeline* pipeline) : pipeline(pipeline) { }
	ShaderPipeline* pipeline = nullptr;
	bool isInstanced = false;
	bool hasLights = false;
	bool hasBones = false;
	
	std::vector<MaterialGroup3D*> materials;

	// Signature
	using Bitset = typename ECS::Bitset;
	Bitset signature;
};

template <typename ECS>
struct PipelineGroup2D {
	PipelineGroup2D(ShaderPipeline* pipeline) : pipeline(pipeline) {}
	ShaderPipeline* pipeline = nullptr;

	std::vector<MaterialGroup2D*> materials;
	
	bool isInstanced;
	
	// Signature
	using Bitset = typename ECS::Bitset;
	Bitset signature;
};

#endif
