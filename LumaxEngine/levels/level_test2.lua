
shader3Dcolor = {
	 file = "GLSL/3D/shader3Dcolor"
}
material1 = {
	 color = {1,0,1}
}
cube = {
	 fullmesh = { file = "res/models/cube.obj"},
	 transform = { translation = { 0,0,0 }},
	 material = material1,
	 shader = shader3Dcolor
}
addModel3D(cube)

print("Done loading lua level")
