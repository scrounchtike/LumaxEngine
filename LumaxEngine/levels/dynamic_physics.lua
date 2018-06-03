
material_texture = {
	 texture = { file = "test.png" }
}

shader3Dtexture = {
	 file = "3D/shader3Dtexture"
}

cube1 = {
	 fullmesh = { file = "cube.obj" },
	 material = material_texture,
	 shader = shader3Dtexture,
	 transform = { translation = { -2,1,5 } }
}
addStaticModel3D(cube1);

print("Done loading lua level")
