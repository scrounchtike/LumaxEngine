
shader3Dcolor = {
	file = "GLSL/3D/shader3Dcolor"
}

texture1 = {
	file = "res/textures/test.png"
}

material1 = {
	baseColor = {1,0,0},
	hitColor = {0,1,0}
}

default = {
	shader = shader3Dcolor,
	material = material1,
	updateMaterial = "copy"
}
updateDefaultVariables(default)

--Load Intersection primitives
aabb = {
	physics = { type = "AABB", position = {0,0,-20}, extents = {1,1,1} }, rendergroup = "AABB",
	movement = { translation = {0,0,"cos<0.05,0.3>"}}
}
sphere = {
	physics = { type = "Sphere", position = {6,0,-10}, radius = 1.0	}, rendergroup = "Sphere",
	movement = { translation = {0,0,"cos<0.05,0.2>"}}
}
plane = {
	physics = { type = "Plane", position = {0,0,0}, normal = {0,0,-1}}, rendergroup = "Plane"
}
addModel3D(plane)

for i = 0,5 do
	addModel3D(aabb)
	aabb.physics.position[1] = aabb.physics.position[1] + 6;
end
for i = 0,4 do
	addModel3D(sphere)
	sphere.physics.position[1] = sphere.physics.position[1] + 6;
end

aabb_contact = {
	physics = { type = "AABB", position = {0,0,-18}, extents = {0.6,0.6,1}}, rendergroup = "AABB"
}
addModel3D(aabb_contact)
sphere_contact = {
	physics = { type = "Sphere", position = {6.5,0,-18.5}, radius = 1.2}, rendergroup = "Sphere"
}
addModel3D(sphere_contact)
sphere_contact.physics.position = {6.5,0,-8.5}
addModel3D(sphere_contact)

obb_contact = {
	physics = { type = "OBB", position = {12.5,0,-18.5}, u1 = {0,0.707,0.707}, u2 = {1,0,0}, extents = {0.6,0.7,1}},
	rendergroup = "OBB", movement = { rotation = {0.001,0.001,"sin<0.1,0.1>"}}
}
for i = 0,1 do
	addModel3D(obb_contact)
	obb_contact.physics.position[3] = obb_contact.physics.position[3] + 10;
end

print("Done loading lua level")