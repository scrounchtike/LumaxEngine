
shader3Dcolor = {
	 --file = "GLSL/3D/shader3Dcolor"
	 file = "HLSL/3D/shader3Dcolor"
}
material1 = {
	 color = {1,0,1}
}
cube = {
	 fullmesh = { file = "res/models/cube.obj"},
	 transform = { translation = { 0,0,-2 }},
	 material = material1,
	 shader = shader3Dcolor
}
cube2 = {
	fullmesh = { file = "res/models/cube.obj"},
	transform = { translation = { 0,0,2 }},
	material = material1,
	shader = shader3Dcolor
}
addModel3D(cube)
addModel3D(cube2)

print("Done loading lua level")
