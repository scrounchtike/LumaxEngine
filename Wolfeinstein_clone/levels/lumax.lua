
function loadTable(tableData, func)
	 tableNames = {}
	 tableValues = {}
	 local n = 0
	 for k,v in pairs(tableData) do
			n = n + 1
			tableNames[n] = k
			tableValues[n] = v
	 end
	 func(tableNames, tableValues)
end

function loadMaterials(materials)
	 print("loading materials")
	 loadTable(materials, lmx_loadMaterials)
end

function loadMeshes2D(meshes2D)
	 print("loading meshes2D")
	 loadTable(meshes2D, lmx_loadMeshes2D)
end

function loadMeshes3D(meshes3D)
	 print("loading meshes3D")
	 loadTable(meshes3D, lmx_loadMeshes3D)
end

function loadPipelines(pipelines)
	 print("loading pipelines")
	 loadTable(pipelines, lmx_loadPipelines)
end

function loadDirectionalLight(dirLight)
	 print("loading directional light")
	 lmx_loadDirectionalLight(dirLight)
end

function loadPointLights(pointLights)
	 print("loading point lights")
	 lmx_loadPointLights(pointLights)
end

function loadSpotLights(spotLights)
	 print("loading spot lights")
	 lmx_loadSpotLights(spotLights)
end

function loadMovements3D(movements3D)
	 print("loading movements 3D")
	 loadTable(movements3D, lmx_loadMovements3D)
end

function makeEntities(entities)
	 print("loading entities")
	 local n = 0
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
				 entitiesValues[n][1][m] = i;
				 entitiesValues[n][2][m] = j;
			end
	 end
	 lmx_makeEntities(entitiesValues)
end
