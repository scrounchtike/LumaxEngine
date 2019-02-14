
print("Begin loading lua level file")

require "levels/lumax"

-- Materials
materials = {
	 ["material1"] = {
			texture = { file = "test.png" }
	 },
	 ["material2"] = {
			color = { 1,0,0 }
	 }
}
loadMaterials(materials)

-- Shaders
pipelines = {
	 ["pipeline3Dbasic"] = {
			shader = { file = "renderer/shader3D" }
	 }
}
loadPipelines(pipelines)

-- Meshes3D
meshes3D = {
	 ["cube"] = {
			file = "cube.obj"
	 },
	 ["sphere"] = {
			file = "sphere.obj"
	 }
}
loadMeshes3D(meshes3D)

-- Movements3D
movements3D = {
	 ["movementSphere"] = {
			Translation = { "sin,10,1", "none", "none" }
	 }
}
loadMovements3D(movements3D)

-- TODO: Lights
--[[
lights = {
	 dlight = {
			Light = {
				 
			},
			Transform3D = { translation = {10,10,0}}
	 },
	 plight1 = {
			Light = {

			},
			Movement3D = {  }
	 }
}
loadLights(lights)
--]]


entities = {
	 entity1 = {
			groups = { "ForwardRender" },
			Mesh3D = "cube",
			Material = "material1",
			ShaderPipeline = "pipeline3Dbasic",
			Transform3D = { translation = {0,0,10}, scale = {7,7,7}},
			LightComponent = {}
	 },
	 entity2 = {
			groups = { "ForwardRender" },
			Mesh3D = "sphere",
			Material = "material2",
			ShaderPipeline = "pipeline3Dbasic",
			DynamicTransform3D = { translation = {-12,10,0}},
			LightComponent = {},
			Movement3D = "movementSphere"
	 }
}
makeEntities(entities)
