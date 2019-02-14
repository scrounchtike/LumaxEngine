
-- Global variables

-- Global level settings
--game = {
--	 clearColor = { 1,0,0.1 },
--	 mouseSensitivity = 0.006,
--	 cameraSpeed = 0.05
--}
--loadGameSettings(game)

print("Beginning of lua level file")

--require "levels/lumax"



-- Materials
materials = {
	 ["material1"] = {
			color = { 1,0,0 }
	 }
}
--loadMaterials(materials)

-- Shaders
pipelines = {
	 ["pipeline3Dbasic"] = {
			shader = { file = "renderer/shader3D" }
	 }
}
--loadPipelines(pipelines)

-- Meshes3D
meshes3D = {
	 ["cube"] = {
			file = "cube.obj"
	 }
}
--loadMeshes3D(meshes3D)

-- TODO: Lights

entities = {
	 entity1 = {
			groups = { "ForwardRender" },
			Mesh3D = "cube",
			Material = "material1",
			ShaderPipeline = "pipeline3Dbasic",
			Transform3D = { translation = {0,0,10}},
			LightComponent = {}
	 }
}

--makeEntities(entities)
