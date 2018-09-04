
-- Creation and initialization of level specific variables (Cameras, renderers, etc..)

-- Initialization and registering of assets

print("Beginning of Lua level")

-- Materials
print("Adding materials")
materials = {
	 ["material1"] = {
			texture = { file = "test.png" }
	 },
	 --["material2"] = {
	--		colors = { {1,0,0}, {0,1,0} }
	 --},
	 --["material3"] = {
	--		color = {1,0,1}
	 --}
}
materialNames = {}
materialValues = {}
local n = 0
for k, v in pairs(materials) do
	 n = n + 1
	 materialNames[n] = k
	 materialValues[n] = v
end
loadMaterials(materialNames, materialValues);

-- Shaders
print("Adding pipelines")
pipelines = {
	 ["pipeline2Dcolor"] = {
			shader = { file = "renderer/shader2Dcolor" }
	 },
	 ["pipeline2Dtexture"] = {
			shader = { file = "renderer/shader2Dtexture" }
	 },
	 ["pipeline3Dcolor"] = {
			shader = { file = "renderer/shader3Dcolor" }
	 },
	 ["pipeline3Dtexture"] = {
			shader = { file = "renderer/shader3Dtexture" }
	 }
}
pipelineNames = {}
pipelineValues = {}
n = 0
for k,v in pairs(pipelines) do
	 n = n + 1
	 pipelineNames[n] = k
	 pipelineValues[n] = v
end
loadPipelines(pipelineNames, pipelineValues)

-- Meshes
print("Loading 2D meshes")
meshes2D = {
	 ["square2D"] = {
			vertices = { -0,5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5 },
			indices = { 0,1,2,2,3,0 },
			texCoords = { 0,0,0,1,1,1,1,0 }
	 }
}
meshes2Dnames = {}
meshes2Dvalues = {}
n = 0
for k,v in pairs(meshes2D) do
	 n = n + 1
	 meshes2Dnames[n] = k;
	 meshes2Dvalues[n] = v
end
--loadMeshes2D(meshes2Dnames, meshes2Dvalues) -- TODO: Support 2D

print("Loading 3D meshes");
meshes3D = {
	 ["square3D"] = {
			vertices = { -0,5,-0.5,0,-0.5,0.5,0,0.5,0.5,0,0.5,-0.5,0 },
			indices = { 0,1,2,2,3,0 }
	 },
	 ["cube"] = {
			file = "cube.obj"
	 }
}
meshes3Dnames = {}
meshes3Dvalues = {}
n = 0
for k,v in pairs(meshes3D) do
	 n = n + 1
	 meshes3Dnames[n] = k
	 meshes3Dvalues[n] = v
end
loadMeshes3D(meshes3Dnames, meshes3Dvalues);

-- Creation of entities and components

print("Loading entities")
entities = {
	 entity1 = {
			groups = { "ForwardRender" },
			Mesh3D = "cube",
			Material = "material1",
			ShaderPipeline = "pipeline3Dtexture",
			Transform3D = { translation = {0,0,10}}
	 }
}
n = 0
entitiesNames = {}
entitiesValues = {}
for k,v in pairs(entities) do
	 n = n + 1
	 entitiesNames[n] = k
	 
	 entitiesValues[n] = {}
	 entitiesValues[n][1] = {}
	 entitiesValues[n][2] = {}
	 local m = 0
	 for i,j in pairs(v) do
			m = m + 1
			entitiesValues[n][1][m] = i
			entitiesValues[n][2][m] = j
	 end
end
makeEntities(entitiesValues)

print("End of lua level")
