
shaderVertexPosition = {
	 file = "renderer/shader2Dposition"
}
shaderVertexTexcoord = {
	 file = "renderer/shader2Dtexcoord"
}

shader2Dcolor = {
	 name = "shader2Dcolor",
	 --file = "renderer/shader2Dcolor"
	 vertex = shaderVertexPosition,
	 fragment = { file = "renderer/shader2Dcolor" }
}
shader2Dtexture = {
	 name = "shader2Dtexture",
	 --file = "renderer/shader2Dtexture"
	 vertex = shaderVertexTexcoord,
	 fragment = { file = "renderer/shader2Dtexture" }
}
shader2D = {
	 name = "shader2D",
	 file = "renderer/shader2D"
	 --vertex = shaderVertexTexcoord,
	 --fragment = { file = "renderer/shader2D" }
}

material1 = {
	 color = { 1,1,0 }
}
material2 = {
	 texture = { file = "test.png" }
}
material3 = {
	 texture = { file = "test.png" },
	 color = { 0,1,1 },
	 blend = 0.5
}

square = {
	 vertices = { -0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5 },
	 texCoords = { 0,0,1,0,1,1,0,1 },
	 indices = { 0,3,2,2,1,0 }
}
smallSquare = {
	 vertices = { -0.02,-0.02,-0.02,0.02,0.02,0.02,0.02,-0.02 },
	 texCoords = { 0,0,1,0,1,1,0,1 },
	 indices = { 0,3,2,2,1,0 }
}

square1 = {
	 mesh = square,
	 material = material1,
	 shader = shader2Dcolor,
	 transform = { translation = { 0.5,0 }}
}
--addModel2D(square1)

square2 = {
	 mesh = square,
	 material = material2,
	 shader = shader2Dtexture,
	 transform = { translation = { -0.5,0 }}
}
--addModel2D(square2)

square3 = {
	 mesh = square,
	 material = material3,
	 shader = shader2D,
	 transform = { translation = { -0.75,-0.75 }, scale = { 0.5,0.5 }}
}
--addModel2D(square3)

-- Instanced model 2D
shader2Dinstanced = {
	 name = "shader2DinstancedPosition",
	 vertex = { file = "renderer/instShader2Dtexcoord" },
	 fragment = { file = "renderer/shader2Dtexture" }
}
squaresInstanced = {
	 mesh = smallSquare,
	 material = material2,
	 shader = shader2Dinstanced,
	 positions = {}
}
max = 32
for i = 1,max do
	 for j = 1,max do
			table.insert(squaresInstanced.positions, 2.0*i/max-1.0);
			table.insert(squaresInstanced.positions, 2.0*j/max-1.0);
	 end
end
addInstancedModel2D(squaresInstanced, max*max)

print("Done loading lua level")
