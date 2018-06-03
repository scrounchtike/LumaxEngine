
-- Player and Camera stuff
inputs = {
	 {"KEY_W", "walk", 1.0},
	 {"KEY_S", "walk", -1.0},
	 {"KEY_A", "right", -1.0},
	 {"KEY_D", "right", 1.0},
	 {"KEY_SPACE", "up", 1.0},
	 {"KEY_LSHIFT", "up", -1.0}
}
inputs2 = {
	 forward = "KEY_W",
	 backward = "KEY_S",
	 right = "KEY_D",
	 left = "KEY_A",
	 up = "KEY_SPACE",
	 down = "KEY_LSHIFT"
}

inputs3 = {
	 forward = "KEY_E",
	 backward = "KEY_D",
	 right = "KEY_F",
	 left = "KEY_S"
}

player = {
	 controls = inputs2,
	 speed = 0.15
}
registerPlayer(player)

shader3Dcolor = {
	file = "3D/shader3Dcolor"
}
shader3Danimation = {
	 file = "animation/basicAnimation"
}
shader3Dlight = {
	 file = "3D/shader3Dlight"
}
gShaderLight = {
	 file = "deferred/gShader"
}
gShaderColor = {
	 file = "deferred/gShaderColor"
}

texture1 = {
	file = "gold.png"
}

material1 = {
	baseColor = {1,0,0},
	hitColor = {0,1,0}
}
material2 = {
	 color = { 1,0,0 }
}
material_texture = {
	 texture = texture1
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

-- Lighted models
lighted_cube = {
	 fullmesh = { file = "cube.obj"},
	 material = material_texture,
	 --shader = shader3Dlight,
	 shaders = { shader3Dlight, shader3Dcolor },
	 transform = {translation = {2,0,6}},
	 rendergroup = "lighted"
}
addModel3D(lighted_cube)

lighted_cube2 = {
	 fullmesh = { file = "cube.obj" },
	 material = material_texture,
	 shader = gShaderLight,
	 transform = { translation = {5,0,6}, scale = {1,1,1}},
	 rendergroup = "deferredlighted"
}
addModel3D(lighted_cube2)

lighted_sphere = {
	 fullmesh = { file = "sphere.obj" },
	 material = material2,
	 shader = gShaderColor,
	 transform = { translation = { 0,0,0 }, scale = { 0.4,0.4,0.4 }},
	 rendergroup = "deferredlighted"
}
-- deferred lights
attenuation1 = {
	 constant = 0.7, linear = 0.7, exponent = 0.9
}
d_pointlight = {
	 position = {10,2,14},
	 color = {1,1,0},
	 attenuation = attenuation1
}

for x = 0,10 do
	 for z = 0,10 do
			lighted_sphere.material.color = { 1,1,0 }
			lighted_sphere.transform.translation = { x+7,0,z+10 }
			addModel3D(lighted_sphere)
	 end
end

for x = 0,2 do
	 for z = 0,2 do
			d_pointlight.position = {x*1.5+10, 1, z*1.4+10}
			d_pointlight.color = { x/9.0,z/9.0,0.3 }
			addDeferredPointLight(d_pointlight)
	 end
end

d_pointlight.attenuation = { constant = 0.05, linear = 0.03, exponent = 0.3 }
d_pointlight.position = { 12,1,12 }
addDeferredPointLight(d_pointlight)

cube_pointlight = {
	 fullmesh = { file = "cube.obj" },
	 shader = shader3Dcolor,
	 material = material1,
	 transform = { translation = d_pointlight.position, scale = { 0.02,0.02,0.02 }}
}
addModel3D(cube_pointlight)

-- Lights
directional1 = {
	 direction = {1,1,-1},
	 color = {0.5,1,0.2},
	 intensity = 1.0
}
addDirectionalLight(directional1)

pointlight1 = {
	 position = {0.5,1.5,4.5},
	 color = {0,0.95,0.65},
	 attenuation = { constant = 0.005, linear = 0.1, exponent = 0.01 }
}
addPointLight(pointlight1)

spotlight1 = {
	 pointlight = { position = {2.5,1.1,7.5}, color = {0.7,0.3,1}},
	 direction = {0,0,1},
	 cutoff = 0.8
}
addSpotLight(spotlight1)

cube_spot = {
	 fullmesh = { file = "cube.obj" },
	 shader = shader3Dcolor,
	 material = material1,
	 transform = { translation = spotlight1.pointlight.position, scale = { 0.02,0.02,0.02 }}
}
addModel3D(cube_spot)

-- animated models
astroboy = {
	 fullmesh = { file = "astroBoy_walk.dae" },
	 shader = shader3Danimation,
	 material = { texture = { file = "astroBoyTexture.png"}},
	 transform = { translation = { -5,1,9 }},
	 rendergroup = "animated"
}
addAnimatedModel3D(astroboy)

print("Done loading lua level")
