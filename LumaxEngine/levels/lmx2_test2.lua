
print("Beginning of Lua level")

-- Materials
print("Adding materials")
materials = {
	 ["material1"] = {
			texture = { file = "test.png" }
	 },
	 ["material2"] = {
	 		colors = { {1,0,0}, {0,1,0} }
	 },
	 ["material3"] = {
	 		color = {1,0,1}
	 }
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

print("Loading 3D meshes");
meshes3D = {
	 ["square3D"] = {
			vertices = { -0,5,-0.5,0,-0.5,0.5,0,0.5,0.5,0,0.5,-0.5,0 },
			indices = { 0,1,2,2,3,0 }
	 },
	 ["cube"] = {
			file = "cube.obj"
	 },
	 ["sphere"] = {
			file = "sphere.obj"
	 },
	 --["doomWall"] = {
	--		vertices = { -1,-1,0,-1,1,0,1,1,0,1,-1,0,-1,-1,0 },
	--		texCoords = { 0,0,0,1,1,1,1,1,1,0,0,0 },
	--		normals = { 0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1 },
	--		indices = { 0,1,2,2,3,0 }
	 --}
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

print("Done loading lua level")
