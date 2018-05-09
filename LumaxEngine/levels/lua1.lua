
-- Global variables
defaultBaseColor = {0,1,0}
defaultHitColor = {1,0,0}

-- Level global settings
game = {
	clearColor = { 0,0,0 }
}
loadLevelParameters(game)

-- Level components
shader3Dcolor = { file = "GLSL/3D/shader3Dcolor" }
shader3Dtexture = { file = "GLSL/3D/shader3Dtexture" }

material1 = {
	texture = { file = "res/textures/test.png" }
}

cube1 = {
	fullmesh = { file = "res/models/cube2.obj" },
	material = { texture = { file = "res/textures/test.png"}, color = { 0,1,1}},
	shader = { file = "GLSL/3D/shader3Dtexture"},
	physics = { type = "AABB", position = { 0,0,0 }, extents = { 1,1,1 }},
	transform = nil
}
addModel3D(cube1)

cube2 = {
	fullmesh = { file = "res/models/cube2.obj" },
	material = { color = { 0,1,0 }},
	shader = shader3Dcolor,
	physics = nil,
	transform = { translation = { 5,0,0 }, scale = { 1,1,1 }}
}
addModel3D(cube2)

aabb1 = {
	physics = { type = "AABB", position = { 0,1,-10}, extents = {1.2,1,1}},
	material = { color = { 0,1,0 }},
	shader = shader3Dcolor,
	fullmesh = "genFromPhysics",
	transform = "genFromPhysics",
	rendergroup = "AABB"
}
addModel3D(aabb1)

sphere1 = {
	physics = { type = "Sphere", position = { 5,1,-10 }, radius = 1.0},
	material = { color = { 0,1,0 }},
	shader = shader3Dcolor,
	rendergroup = "Sphere"
}
addModel3D(sphere1)

plane1 = {
	physics = { type = "Plane", position = { -5,1,-10 }, normal = { 0,0,1 }},
	material = { color = { 0,1,0 }},
	shader = shader3Dcolor,
	rendergroup = "Plane"
}
addModel3D(plane1)

obb1 = { 
	physics = { type = "OBB", position = { 10,1,-10 }, u1 = {0.707,0.707,0}, u2 = {0,1,0}, extents = {1,1,1}},
	material = { color = {0,1,0 }}, shader = shader3Dcolor, rendergroup = "OBB" 
}
addModel3D(obb1)

ray1 = { physics = { type = "Ray", position = {-15,1,-10}, direction = {1,1,1}}, material = { color = {0,1,0}}, shader = shader3Dcolor, rendergroup = "Ray" }
--addModel3D(ray1)

line1 = { physics = { type = "Line", start = {-10,1,-10}, end_ = {-11,2,-11}}, material = { color = { 0,1,0 }}, shader = shader3Dcolor, rendergroup = "Line" }
--addModel3D(line1)

-- Moving / Dynamic objects
sin = {
	type = "sin",
	amplitude = 1.0,
	frequency = 0.5
}

aabb2 = {
	physics = { type = "OBB", position = {0,0,10}, u1 = {1,0,0}, u2 = {0,1,0}, extents = {1,1,1} },
	material = material1, shader = shader3Dtexture, rendergroup = "OBB",
	movement = { translation = {0,0,"cos"}, rotation = {0.01,0.01,0} }
}
addModel3D(aabb2);

print("Done loading lua level.")