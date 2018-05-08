
#include "LevelLoader.hpp"

#include <cassert>
#include "../RAL/Log.hpp"

#include "OBJLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"

#include "ModelLoader.hpp"

Vec3 LevelLoader::defaultClearColor;
Vec3 LevelLoader::defaultBaseColor;
Vec3 LevelLoader::defaultHitColor;

// Static helper pointers
Level* LevelLoader::level;

// Registry maps
std::map<std::string, Model2D*>		LevelLoader::models2D;
std::map<std::string, Mesh2D*>		LevelLoader::meshes2D;
std::map<std::string, Transform2D*> LevelLoader::transforms2D;

std::map<std::string, FullModel3D*> LevelLoader::models3D;
std::map<std::string, Mesh3D*>		LevelLoader::meshes3D;
std::map<std::string, Transform3D*> LevelLoader::transforms3D;

std::map<std::string, Texture*>     LevelLoader::textures;
std::map<std::string, Material*>	LevelLoader::materials;
std::map<std::string, Shader*>		LevelLoader::shaders;

std::map<std::string, AABB*>        LevelLoader::aabbs;
std::map<std::string, Sphere*>      LevelLoader::spheres;
std::map<std::string, Plane*>       LevelLoader::planes;
std::map<std::string, OBB*>			LevelLoader::obbs;
std::map<std::string, Ray*>         LevelLoader::rays;
std::map<std::string, Line*>		LevelLoader::lines;

std::map<std::string, Movement3D*>  LevelLoader::movements3D;

// Static Models 3D
FullModel3D* LevelLoader::modelCube3D;

// Resource Manager
ResourceManager* LevelLoader::resManager;

Level* LevelLoader::loadLevel(const std::string& filename, ResourceManager* resManager) {
	// Set static Resource Manager
	LevelLoader::resManager = resManager;

	// Renderer creation
	Camera* camera = new Camera();

#ifdef _USE_DIRECTX11
	RendererDX11* renderer;
	renderer = new RendererDX11(camera);
#elif defined _USE_OPENGL
	RendererGL* renderer;
	renderer = new RendererGL(camera);
#endif

	// Level creation
	Level* newLevel = new Level(renderer, camera);
	level = newLevel;

	// Init static models
	const float pointSize = 1; // 2x2x2 cube
	float verticesCubes3D[24] = { -pointSize, pointSize, pointSize,
		pointSize, pointSize, pointSize,
		pointSize, -pointSize, pointSize,
		-pointSize, -pointSize, pointSize,
		-pointSize, pointSize, -pointSize,
		pointSize, pointSize, -pointSize,
		pointSize, -pointSize, -pointSize,
		-pointSize, -pointSize, -pointSize };
	int indicesCubes3D[36] = { 0,1,2,2,3,0,
		1,5,6,6,2,1,
		4,5,1,1,0,4,
		4,0,3,3,7,4,
		6,7,3,3,2,6,
		5,4,7,7,6,5 };
	std::vector<Model3DGL*> modelCube { new Model3DGL(verticesCubes3D, 24, indicesCubes3D, 36) };
	modelCube3D = new FullModel3D(modelCube);

	std::ifstream file;
	file.open("levels/" + filename);
	if (!file.is_open()) {
		Log::println("Fatal Error: Could not open level file " + filename);
		assert(false);
		return nullptr;
	}

	bool inComment = false;

	std::string line;
	while (std::getline(file, line)) {
		// Primary keyword
		std::vector<std::string> keywords = Utils::split(line);
		if (line == "*/")
			inComment = false;
		if (line == "/*")
			inComment = true;
		if (inComment) continue;
		if (keywords.size() <= 1)
			continue;
		if (keywords[ACTION] == "set") {
			// Set variable
			set(keywords);
		}
		else if (keywords[ACTION] == "add") {
			// Add element
			add(keywords);
		}
	}
	file.close();

	return newLevel;
}

void LevelLoader::set(const std::vector<std::string>& keywords) {

}

void LevelLoader::add(const std::vector<std::string>& keywords) {
	static std::string name = "";
	static Vec2 position(0, 0);
	static Vec3 baseColor = defaultBaseColor;
	static Vec3 hitColor = defaultHitColor;

	static Vec3 position3D(0, 0, 0);

	if (keywords[TYPE] == "Point2D") {
		for (int i = 2; i < keywords.size(); i += 2) {
			std::string keyword = keywords[i];
			keyword = keyword.substr(0, keyword.length() - 1);
			if (keyword == "name") {
				name = keywords[i + 1];
			}
			else if (keyword == "position") {
				position = loadVector2D(keywords[i + 1]);
			}
			else if (keyword == "baseColor" || keyword == "color") {
				baseColor = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "hitColor") {
				hitColor = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "transform") {
				// Get Transform by name
			}
		}
		// Create Point2D and add it to the rendering pipeline
		Point2D* point2D;
		if (name != "")
			level->addNamedPoint2D(name, new Point2D(position, baseColor));
		else
			level->addPoint2D(new Point2D(position, baseColor));
	}
	else if (keywords[TYPE] == "Line2D") {
		static Vec2 start(0, 0);
		static Vec2 end(1, 1);
		for (int i = 2; i < keywords.size(); i += 2) {
			std::string keyword = keywords[i];
			keyword = keyword.substr(0, keyword.length() - 1);
			if (keyword == "name") {
				name = keywords[i + 1];
			}
			else if (keyword == "start") {
				start = loadVector2D(keywords[i + 1]);
			}
			else if (keyword == "end") {
				end = loadVector2D(keywords[i + 1]);
			}
			else if (keyword == "baseColor" || keyword == "color") {
				baseColor = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "hitColor") {
				hitColor = loadVector3D(keywords[i + 1]);
			}
		}
		// Create Line2D and add it to the rendering pipeline
		Line2D* line2D;
		if (name != "")
			level->addNamedLine2D(name, new Line2D(start, end, baseColor));
		else
			level->addLine2D(new Line2D(start, end, baseColor));
	}
	else if (keywords[TYPE] == "Sprite2D") {
		static Vec2 extents(1, 1);
		for (int i = 2; i < keywords.size(); i += 2) {
			std::string keyword = keywords[i];
			keyword = keyword.substr(0, keyword.length() - 1);
			if (keyword == "name") {
				name = keywords[i + 1];
			}
			else if (keyword == "position") {
				position = loadVector2D(keywords[i + 1]);
			}
			else if (keyword == "extents") {
				extents = loadVector2D(keywords[i + 1]);
			}
			else if (keyword == "baseColor" || keyword == "color") {
				baseColor = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "hitColor") {
				hitColor = loadVector3D(keywords[i + 1]);
			}
		}
		// Create Sprite2D and add it to the rendering pipeline
		Sprite2D* sprite2D;
		if (name != "")
			level->addNamedSprite2D(name, new Sprite2D(position, extents, baseColor));
		else
			level->addSprite2D(new Sprite2D(position, extents, baseColor));
	}
	else if (keywords[TYPE] == "Point3D") {
		for (int i = 2; i < keywords.size(); i += 2) {
			std::string keyword = keywords[i];
			keyword = keyword.substr(0, keyword.length() - 1);
			if (keyword == "name") {
				name = keywords[i + 1];
			}
			else if (keyword == "position") {
				position3D = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "color" || keyword == "baseColor") {
				baseColor = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "hitColor") {
				hitColor = loadVector3D(keywords[i + 1]);
			}
		}
		// Create Point3D and add it to the rendering pipeline
		Point3D* point3D;
		if (name != "")
			level->addNamedPoint3D(name, new Point3D(position3D, baseColor));
		else
			level->addPoint3D(new Point3D(position3D, baseColor));
	}
	else if (keywords[TYPE] == "Line3D") {
		Vec3 start(0, 0, 0);
		Vec3 end(1, 1, 1);
		for (int i = 2; i < keywords.size(); i += 2) {
			std::string keyword = keywords[i];
			keyword = keyword.substr(0, keyword.length() - 1);
			if (keyword == "name") {
				name = keywords[i + 1];
			}
			else if (keyword == "start") {
				start = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "end") {
				end = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "baseColor" || keyword == "color") {
				baseColor = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "hitColor") {
				hitColor = loadVector3D(keywords[i + 1]);
			}
		}
		// Create Line3D and add it to the rendering pipeline
		if (name != "")
			level->addNamedLine3D(name, new Line3D(start, end, baseColor));
		else
			level->addLine3D(new Line3D(start, end, baseColor));
	}
	else if (keywords[TYPE] == "Sprite3D") {
		Vec3 extentU(1, 1, 1);
		Vec3 extentV(1, 1, 1);
		for (int i = 2; i < keywords.size(); i += 2) {
			std::string keyword = keywords[i];
			keyword = keyword.substr(0, keyword.length() - 1);
			if (keyword == "name") {
				name = keywords[i + 1];
			}
			else if (keyword == "position") {
				position3D = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "extentU") {
				extentU = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "extentV") {
				extentV = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "baseColor" || keyword == "color") {
				baseColor = loadVector3D(keywords[i + 1]);
			}
			else if (keyword == "hitColor") {
				hitColor = loadVector3D(keywords[i + 1]);
			}
		}
		// Create Sprite3D and add it to the rendering pipeline
		if (name != "")
			level->addNamedSprite3D(name, new Sprite3D(position3D, extentU, extentV, baseColor));
		else
			level->addSprite3D(new Sprite3D(position3D, extentU, extentV, baseColor));
	}
	else if (keywords[TYPE] == "Transform2D") {
		addTransform2D(keywords);
	}
	else if (keywords[TYPE] == "Model2D") {
		addModel2D(keywords);
	}
	else if (keywords[TYPE] == "Material") {
		addMaterial(keywords);
	}
	else if (keywords[TYPE] == "Shader") {
		addShader(keywords);
	}
	else if (keywords[TYPE] == "Mesh2D") {
		addMesh2D(keywords);
	}
	else if (keywords[TYPE] == "Texture") {
		addTexture(keywords);
	}
	else if (keywords[TYPE] == "Model3D") {
		addModel3D(keywords);
	}
	else if (keywords[TYPE] == "Mesh3D") {
		addMesh3D(keywords);
	}
	else if (keywords[TYPE] == "Transform3D") {
		addTransform3D(keywords);
	}
	else if (keywords[TYPE] == "AABB") {
		addAABB3D(keywords);
	}
	else if (keywords[TYPE] == "Sphere") {
		addSphere(keywords);
	}
	else if (keywords[TYPE] == "Plane") {
		addPlane(keywords);
	}
	else if (keywords[TYPE] == "OBB") {
		addOBB(keywords);
	}
	else if (keywords[TYPE] == "Movement3D") {
		addMovement3D(keywords);
	}
	else {
		Log::println("Warning: Unregconized keyword " + keywords[TYPE] + " in Level loading");
	}
}

Model2D* LevelLoader::addModel2D(const std::vector<std::string>& keywords) {
	bool isIndexed = false;
	bool isTextured = false;
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	float* vertices = nullptr;
	int* indices = nullptr;
	float* texCoords = nullptr;

	std::string name;

	for (int i = 2; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "vertices") {
			std::vector<std::string> verts = Utils::splitByRegex(keywords[i + 1], ',');
			numVertices = verts.size() / 2;
			vertices = new float[verts.size()];
			for (int i = 0; i < verts.size(); ++i)
				vertices[i] = std::stof(verts[i]);
		}
		else if (keyword == "indices") {
			isIndexed = true;
			std::vector<std::string> inds = Utils::splitByRegex(keywords[i + 1], ',');
			numIndices = inds.size();
			indices = new int[inds.size()];
			for (int i = 0; i < inds.size(); ++i)
				indices[i] = std::stoi(inds[i]);
		}
		else if (keyword == "texCoords") {
			isTextured = true;
			std::vector<std::string> texs = Utils::splitByRegex(keywords[i + 1], ',');
			texCoords = new float[numVertices * 2];
			for (int i = 0; i < numVertices * 2; ++i)
				texCoords[i] = std::stof(texs[i]);
		}
	}

	Model2D* model2D = nullptr;
	if (isIndexed && isTextured) {
#ifdef _USE_OPENGL
		model2D = new Model2DGL(vertices, numVertices, indices, numIndices, texCoords);
#elif defined _USE_DIRECTX11
		model2D = new Model2DDX11(vertices, numVertices, indices, numIndices, texCoords);
#endif
	}
	else if (isIndexed && !isTextured) {
#ifdef _USE_OPENGL
		model2D = new Model2DGL(vertices, numVertices, indices, numIndices);
#elif defined _USE_DIRECTX11
		model2D = new Model2DDX11(vertices, numVertices, indices, numIndices);
#endif
	}
	else if (!isIndexed && isTextured) {
#ifdef _USE_OPENGL
		model2D = new Model2DGL(vertices, numVertices, texCoords);
#elif defined _USE_DIRECTX11
		model2D = new Model2DDX11(vertices, numVertices, texCoords);
#endif
	}
	else {
#ifdef _USE_OPENGL
		model2D = new Model2DGL(vertices, numVertices);
#elif defined _USE_DIRECTX11
		model2D = new Model2DDX11(vertices, numVertices);
#endif
	}
	if (name != "")
		models2D.insert(std::pair<std::string, Model2D*>(name, model2D));
	else
		Log::println("Warning: Model2D created in LevelLoader not associated with any name, making it unusable!");
	return model2D;
}

FullModel3D* LevelLoader::addModel3D(const std::vector<std::string>& keywords, bool skipFirst) {
	bool isIndexed = false;
	bool isTextured = false;
	bool hasNormals = false;
	bool hasTangents = false;
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	float* vertices = nullptr;
	int* indices = nullptr;
	float* texCoords = nullptr;
	float* normals = nullptr;
	float* tangents = nullptr;

	std::string name;

	bool fromFile = false;
	std::string file;

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "vertices") {
			std::vector<std::string> verts = Utils::splitByRegex(keywords[i + 1], ',');
			numVertices = verts.size() / 3;
			vertices = new float[verts.size()];
			for (int i = 0; i < verts.size(); ++i)
				vertices[i] = std::stof(verts[i]);
		}
		else if (keyword == "indices") {
			isIndexed = true;
			std::vector<std::string> inds = Utils::splitByRegex(keywords[i + 1], ',');
			numIndices = inds.size();
			indices = new int[numIndices];
			for (int i = 0; i < numIndices; ++i)
				indices[i] = std::stoi(inds[i]);
		}
		else if (keyword == "texcoords" || keyword == "texCoords") {
			isTextured = true;
			std::vector<std::string> texs = Utils::splitByRegex(keywords[i + 1], ',');
			assert((texs.size() / 2) == numVertices);
			texCoords = new float[texs.size()];
			for (int i = 0; i < texs.size(); ++i)
				texCoords[i] = std::stof(texs[i]);
		}
		else if (keyword == "normals") {
			hasNormals = true;
			std::vector<std::string> norms = Utils::splitByRegex(keywords[i + 1], ',');
			assert((norms.size() / 3) == numVertices);
			normals = new float[norms.size()];
			for (int i = 0; i < norms.size(); ++i)
				normals[i] = std::stof(norms[i]);
		}
		else if (keyword == "tangents") {
			hasTangents = true;
			std::vector<std::string> tangs = Utils::splitByRegex(keywords[i + 1], ',');
			assert((tangs.size() / 3) == numVertices);
			tangents = new float[tangs.size()];
			for (int i = 0; i < tangs.size(); ++i)
				tangents[i] = std::stof(tangs[i]);
		}
		else if (keyword == "file") {
			fromFile = true;
			file = keywords[i + 1];
		}
	}
	if (!fromFile && !numVertices) {
		Log::println("Error: Not enough information provided to load Model3D in LevelLoader. Requiring at least vertices or a file");
		assert(false);
		return nullptr;
	}

	FullModel3D* fullModel = nullptr;

	if (fromFile) {
		// From file
#ifdef _USE_OPENGL
		std::vector<Model3DGL*> models = ModelLoader::loadModel(file);
#elif defined _USE_DIRECTX11
		std::vector<Model3DDX11*> models = ModelLoader::loadModel(file);
#endif
		fullModel = new FullModel3D(models);
	}
	else {
#ifdef _USE_OPENGL
		Model3DGL* model;
#elif defined _USE_DIRECTX11
		Model3DDX11* model;
#endif
		// From provided geometry
		if (!isIndexed) {
			// Not indexed
#ifdef _USE_OPENGL
			if (hasTangents)
				model = new Model3DGL(vertices, numVertices, texCoords, normals, tangents);
			else if (hasNormals)
				model = new Model3DGL(vertices, numVertices, texCoords, normals);
			else if (isTextured)
				model = new Model3DGL(vertices, numVertices, texCoords);
			else
				model = new Model3DGL(vertices, numVertices);
#elif defined _USE_DIRECTX11
			if (hasTangents)
				model = new Model3DDX11(vertices, numVertices, texCoords, normals, tangents);
			else if (hasNormals)
				model = new Model3DDX11(vertices, numVertices, texCoords, normals);
			else if (isTextured)
				model = new Model3DDX11(vertices, numVertices, texCoords);
			else
				model = new Model3DDX11(vertices, numVertices);
#endif
		}
		else {
			// Indexed
#ifdef _USE_OPENGL
			if (hasTangents)
				model = new Model3DGL(vertices, numVertices, indices, numIndices, texCoords, normals, tangents);
			else if (hasNormals)
				model = new Model3DGL(vertices, numVertices, indices, numIndices, texCoords, normals);
			else if (isTextured)
				model = new Model3DGL(vertices, numVertices, indices, numIndices, texCoords);
			else
				model = new Model3DGL(vertices, numVertices, indices, numIndices);
#elif defined _USE_DIRECTX11
			if (hasTangents)
				model = new Model3DDX11(vertices, numVertices, indices, numIndices, texCoords, normals, tangents);
			else if (hasNormals)
				model = new Model3DDX11(vertices, numVertices, indices, numIndices, texCoords, normals);
			else if (isTextured)
				model = new Model3DDX11(vertices, numVertices, indices, numIndices, texCoords);
			else
				model = new Model3DDX11(vertices, numVertices, indices, numIndices);
#endif
		}
		// Generate FullModel3D from single model
		fullModel = new FullModel3D();
		fullModel->models.push_back(model);
	}
	if (fullModel == nullptr)
		Log::println("Full Model 3D was nullptr");
	assert(fullModel);

	if (name != "")
		models3D.insert(std::pair<std::string, FullModel3D*>(name, fullModel));

	return fullModel;
}

Material* LevelLoader::addMaterial(const std::vector<std::string>& keywords) {
	std::string name;
	Texture* texture = nullptr;
	std::vector<Vec3> colors;

	for (int i = 2; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "texture") {
			//texture = textures.find(keywords[i + 1])->second;
			std::map<std::string, Texture*>::const_iterator it = textures.find(keywords[i + 1]);
			if (it == textures.end()) {
				Log::println("Error: Could not find texture " + keywords[i + 1] + " in Material loading for Level creation");
				assert(false);
				return nullptr;
			}
			texture = it->second;
		}
		else if (keyword == "baseColor" || keyword == "color" || keyword == "color_1") {
			colors.push_back(loadVector3D(keywords[i + 1]));
		}
		else if (keyword == "hitColor" || keyword == "color_2") {
			colors.push_back(loadVector3D(keywords[i + 1]));
		}
		else if (keyword == "color_3") {
			colors.push_back(loadVector3D(keywords[i + 1]));
		}
	}

	if (!colors.size())
		colors.push_back(Vec3(0, 0, 0));

	Material* material;
	if (texture)
		material = new Material(texture);
	else
		material = new Material(&colors[0], colors.size());
	if (name != "")
		materials.insert(std::pair<std::string, Material*>(name, material));
	return material;
}

Shader* LevelLoader::addShader(const std::vector<std::string>& keywords) {
	std::string name;
	std::string filename;

	for (int i = 2; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "file") {
			filename = keywords[i + 1];
		}
	}

	Shader* shader = nullptr;
	if (filename != "") {
		shader = resManager->loadShader(filename);
	}
	else {
		// Find the shader by name
	}
	if (name != "")
		shaders.insert(std::pair<std::string, Shader*>(name, shader));
	else
		Log::println("Warning: Shader added to LevelLoader not associated to any name, making it unusable!");
	
	assert(shader);
	return shader;
}

Transform2D* LevelLoader::addTransform2D(const std::vector<std::string>& keywords, bool skipFirst) {
	static enum MODE {
		STATIC, DYNAMIC, PIVOT
	};
	MODE mode = STATIC;

	std::string name;

	Vec2 position(0, 0);
	float rotation = 0;
	Vec2 scale(1, 1);

	Vec2 pivot(0, 0);
	float pivot_rotation = 0;

	bool hasPivot = false;

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		if (keyword == "mode") {
			if (keywords[i + 1] == "static")
				mode = STATIC;
			else if (keywords[i + 1] == "dynamic")
				mode = DYNAMIC;
			else if (keywords[i + 1] == "pivot")
				mode = PIVOT;
		}
		else if (keyword == "position" || keyword == "translation") {
			position = loadVector2D(keywords[i + 1]);
		}
		else if (keyword == "rotation") {
			rotation = std::stof(keywords[i + 1]);
		}
		else if (keyword == "scale" || keyword == "scaling") {
			scale = loadVector2D(keywords[i + 1]);
		}
		else if (keyword == "pivot") {
			position = loadVector2D(keywords[i + 1]);
		}
		else if (keyword == "pivot_rotation") {
			pivot_rotation = std::stof(keywords[i + 1]);
		}
	}

	// Creating the transform based on given information
	Transform2D* transform = nullptr;
	if (mode == STATIC) {
		transform = new Transform2D(position, rotation, scale, pivot, pivot_rotation);
	}
	else if (mode == DYNAMIC) {
		transform = new DynamicTransform2D(position, rotation, scale);
	}
	else if (mode == PIVOT) {
		transform = new PivotTransform2D(position, rotation, scale, pivot, pivot_rotation);
	}

	// Register transform2D by name
	if (name != "")
		transforms2D.insert(std::pair<std::string, Transform2D*>(name, transform));

	return transform;
}

Transform3D* LevelLoader::addTransform3D(const std::vector<std::string>& keywords, bool skipFirst) {
	static enum MODE {
		STATIC, DYNAMIC, PIVOT
	};
	MODE mode = STATIC;

	std::string name;

	Vec3 position(0, 0, 0);
	Vec3 rotation(0, 0, 0);
	Vec3 scale(1, 1, 1);

	Vec3 pivot(0, 0, 0);
	Vec3 pivot_rotation(0, 0, 0);

	bool hasPivot = false;

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "mode") {
			if (keywords[i + 1] == "static")
				mode = STATIC;
			else if (keywords[i + 1] == "dynamic")
				mode = DYNAMIC;
			else if (keywords[i + 1] == "pivot")
				mode = PIVOT;
		}
		else if (keyword == "translation" || keyword == "position") {
			position = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "rotation") {
			rotation = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "scale") {
			scale = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "pivot") {
			pivot = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "pivot_rotation") {
			pivot_rotation = loadVector3D(keywords[i + 1]);
		}
	}

	// Creating the transform based on given information
	Transform3D* transform = nullptr;
	if (mode == STATIC) {
		transform = new Transform3D(position, rotation, scale, pivot, pivot_rotation);
	}
	else if (mode == DYNAMIC) {
		transform = new DynamicTransform3D(position, rotation, scale);
	}
	else if (mode == PIVOT) {
		transform = new PivotTransform3D(position, rotation, scale, pivot, pivot_rotation);
	}

	transform->getTransformation()->print();

	// Register transform2D by name
	if (name != "")
		transforms3D.insert(std::pair<std::string, Transform3D*>(name, transform));

	return transform;
}

Mesh2D* LevelLoader::addMesh2D(const std::vector<std::string>& keywords) {
	std::string name;

	Model2D* model = nullptr;
	Material* material = nullptr;
	Shader* shader = nullptr;
	Transform2D* transform = nullptr;

	for (int i = 2; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "model" || keyword == "model2D") {
			std::map<std::string, Model2D*>::const_iterator it = models2D.find(keywords[i + 1]);
			if (it == models2D.end()) {
				Log::println("Fatal Error: Model2D " + keywords[i + 1] + " associated to new Mesh2D in LevelLoader does not exist");
				assert(false);
			}
			model = it->second;
		}
		else if (keyword == "material") {
			std::map<std::string, Material*>::const_iterator it = materials.find(keywords[i + 1]);
			if (it == materials.end()) {
				Log::println("Fatal Error: Material " + keywords[i + 1] + " associated to new Mesh2D in LevelLoader does not exist");
				assert(false);
			}
			material = it->second;
		}
		else if (keyword == "shader") {
			std::map<std::string, Shader*>::const_iterator it = shaders.find(keywords[i + 1]);
			if (it == shaders.end()) {
				Log::println("Fatal Error: Shader " + keywords[i + 1] + " associated to new Mesh2D in LevelLoader does not exist");
				assert(false);
			}
			shader = it->second;
		}
		else if (keyword == "transform") {
			if (keywords[i + 1].substr(0, 1) == "(")
				transform = addTransform2D(getImplicitArguments(keywords, i), false);
			else {
				// Transform already created
				std::map<std::string, Transform2D*>::const_iterator it = transforms2D.find(keywords[i + 1]);
				if (it != transforms2D.end())
					transform = it->second;
				else {
					Log::println("Warning: Transform referenced during Mesh2D creation in LevelLoader was not found: " + keywords[i + 1]);
				}
			}
		}
	}

	// Check that all three components for a Mesh2D are loaded
	if (!model || !material || !shader) {
		Log::println("Fatal Error: Tried to add Mesh2D with missing information. Mesh2D requires a Model2D, a Material and a Shader");
		assert(false);
	}
	Mesh2D* mesh2D = new Mesh2D(model, material, shader, transform);
	if (name != "")
		meshes2D.insert(std::pair<std::string, Mesh2D*>(name, mesh2D));

	// Add Mesh2D to the renderer
	if (name != "")
		level->addNamedMesh2D(name, mesh2D);
	else
		level->addMesh2D(mesh2D);

	return mesh2D;
}

Mesh3D* LevelLoader::addMesh3D(const std::vector<std::string>& keywords) {
	std::string name;

	FullModel3D* fullModel = nullptr;
	Material* material = nullptr;
	Shader* shader = nullptr;
	Transform3D* transform = nullptr;
	PhysicsPrimitive* physics = nullptr;

	std::string physicsType = "";
	bool renderAABB = false;
	bool renderSphere = false;
	bool renderPlane = false;
	bool renderOBB = false;
	bool renderRay = false;
	bool renderLine = false;

	Movement3D* movement = nullptr;

	for (int i = 2; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "model") {
			if (keywords[i + 1].substr(0, 1) == "(")
				fullModel = addModel3D(getImplicitArguments(keywords, i), false);
			else {
				std::map<std::string, FullModel3D*>::const_iterator it = models3D.find(keywords[i + 1]);
				if (it == models3D.end()) {
					Log::println("Error: Model3D referenced during loading of Mesh3D was not found: " + keywords[i + 1]);
					assert(false);
				}
				fullModel = it->second;
			}
		}
		else if (keyword == "material") {
			int copy = 0;
			if (keywords[i + 1].substr(0, 1) == "*") {
				// Make a copy of preset material
				copy = 1;
			}
			std::map<std::string, Material*>::const_iterator it = materials.find(keywords[i + 1].substr(copy, keywords[i + 1].length()));
			if (it == materials.end()) {
				Log::println("Error: Material referenced during loading of Mesh3D was not found: " + keywords[i + 1]);
				assert(false);
			}
			if (!copy)
				material = it->second;
			else
				material = new Material(*it->second);
		}
		else if (keyword == "shader") {
			std::map<std::string, Shader*>::const_iterator it = shaders.find(keywords[i + 1]);
			if (it == shaders.end()) {
				Log::println("Error: Shader referenced during loading of Mesh3D was not found: " + keywords[i + 1]);
				assert(false);
			}
			shader = it->second;
		}
		else if (keyword == "transform") {
			if (keywords[i + 1].substr(0, 1) == "(")
				transform = addTransform3D(getImplicitArguments(keywords, i), false);
			else {
				std::map<std::string, Transform3D*>::const_iterator it = transforms3D.find(keywords[i + 1]);
				if (it == transforms3D.end()) {
					Log::println("Error: Transform3D referenced during loading of Mesh3D was not found: " + keywords[i + 1]);
					assert(false);
				}
				transform = it->second;
			}
		}
		else if (keyword == "physics") {
			std::vector<std::string> keywords_;
			keywords_.push_back(keywords[i + 1]);
			int k = 2;
			while (keywords[i + k].substr(keywords[i + k].length() - 1, 1) != ")") {
				keywords_.push_back(keywords[i + k]);
				++k;
			}
			// Remove trailing ')'
			keywords_.push_back(keywords[i + k].substr(0, keywords[i + k].length() - 1));
			// Get type of physics
			std::vector<std::string> beginning = Utils::splitByRegex(keywords_[0], '(');
			keywords_[0] = beginning[1];
			physicsType = beginning[0];

			if (keywords_.size() <= 1) {
				Log::println("Error: Argument to physics in Mesh3D loading was incorrect size. Expected more than 1, received: " + keywords_.size());
				assert(false);
			}

			if (keywords_.size() > 1) {
				// Build from parameters
				if (physicsType == "AABB") {
					int m = 0;
					physics = addAABB3D(keywords_, false);
				}
				else if (physicsType == "Sphere") {
					physics = addSphere(keywords_, false);
				}
				else if (physicsType == "Plane") {
					physics = addPlane(keywords_, false);
				}
				else if (physicsType == "OBB") {
					physics = addOBB(keywords_, false);
				}
				else if (physicsType == "Ray") {
					physics = addRay(keywords_, false);
				}
				else if (physicsType == "Line") {
					physics = addLine(keywords_, false);
				}
			}
			else {
				// Get from name
				if (physicsType == "AABB") {
					physics = aabbs.find(keywords_[0])->second;
				}
				else if (physicsType == "Sphere") {
					physics = spheres.find(keywords_[0])->second;
				}
				else if (physicsType == "Plane") {
					physics = planes.find(keywords_[0])->second;
				}
				else if (physicsType == "OBB") {
					physics = obbs.find(keywords_[0])->second;
				}
				else if (physicsType == "Ray") {
					physics = rays.find(keywords_[0])->second;
				}
				else if (physicsType == "Line") {
					physics = lines.find(keywords_[0])->second;
				}
			}
		}
		else if (keyword == "render") {
			// Render target
			if (keywords[i + 1] == "AABB") {
				renderAABB = true;
			}
			else if (keywords[i + 1] == "Sphere" || keywords[i + 1] == "sphere") {
				renderSphere = true;
			}
			else if (keywords[i + 1] == "Plane" || keywords[i + 1] == "plane") {
				renderPlane = true;
			}
			else if (keywords[i + 1] == "OBB") {
				renderOBB = true;
			}
			else if (keywords[i + 1] == "Ray" || keywords[i + 1] == "ray") {
				renderRay = true;
			}
			else if (keywords[i + 1] == "Line" || keywords[i + 1] == "line") {
				renderLine = true;
			}
		}
		else if (keyword == "movement") {
			if (keywords[i + 1].substr(0, 1) == "(")
				movement = addMovement3D(getImplicitArguments(keywords, i), false);
			else {
				std::map<std::string, Movement3D*>::const_iterator it = movements3D.find(keywords[i + 1]);
				if (it == movements3D.end()) {
					Log::println("Error: Movement3D referenced during loading of Mesh3D was not found: " + keywords[i + 1]);
					assert(false);
				}
				movement = it->second;
			}
		}
	}

	Mesh3D* mesh = nullptr;
	if (name == "") {
		if (renderAABB) {
			mesh = level->addAABB(new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderSphere) {
			mesh = level->addSphere(new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderPlane) {
			mesh = level->addPlane(new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderOBB) {
			mesh = level->addOBB(new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderRay) {
			mesh = level->addRay(new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderLine) {
			mesh = level->addLine(new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else {
			mesh = level->addMesh3D(new Mesh3D(fullModel, material, shader, transform, physics));
		}
	}
	else {
		if (renderAABB) {
			mesh = level->addNamedAABB(name, new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderSphere) {
			mesh = level->addNamedSphere(name, new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderPlane) {
			mesh = level->addNamedPlane(name, new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderOBB) {
			mesh = level->addNamedOBB(name, new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderRay) {
			mesh = level->addNamedRay(name, new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else if (renderLine) {
			mesh = level->addNamedLine(name, new Mesh3D(fullModel, material, shader, transform, physics));
		}
		else {
			mesh = level->addNamedMesh3D(name, new Mesh3D(fullModel, material, shader, transform, physics));
		}
		meshes3D.insert(std::pair<std::string, Mesh3D*>(name, mesh));
	}

	// Movement 3D
	if (movement) {
		movement->mesh = mesh;
		level->addMovement3D(movement);
	}

	assert(mesh);
	return mesh;
}

Texture* LevelLoader::addTexture(const std::vector<std::string>& keywords) {
	std::string name = "";
	std::string format = "rgb";
	unsigned int size = 0;
	unsigned int formatSize = 4;

	float** pixels = nullptr;

	Texture* result = nullptr;

	for (int i = 2; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "format") {
			format = keywords[i + 1];
		}
		else if (keyword == "size") {
			size = std::stoi(keywords[i + 1]);
			// Init pixels array (Square texture only)
			pixels = new float*[size];
			for (int j = 0; j < size; ++j)
				pixels[j] = new float[size * formatSize];
		}
		else if (keyword == "pixels") {
			if (!size) {
				Log::println("Error: Did not provide size of texture in .lmx level file BEFORE providing pixels");
				assert(false);
				return nullptr;
			}
			//std::vector<std::string> pixels = Utils::splitByRegexT<float>(keywords[i+1], ',', std::stof);
			std::vector<std::string> pixelsStr = Utils::splitByRegex(keywords[i + 1], ',');
			Log::println(std::to_string(pixelsStr.size()));
			if (pixelsStr.size() != (size * size * formatSize)) {
				Log::println("Error: Pixels given in LevelLoader loadTexture function not matching size of texture");
				assert(false);
				return nullptr;
			}
			int pixelIndex = 0;
			for (int j = 0; j < size; ++j)
				for (int k = 0; k < size * formatSize; ++k)
					pixels[j][k] = std::stof(pixelsStr[pixelIndex++]);

#ifdef _USE_OPENGL
			result = new TextureGL(pixels, size, GL_RGB);
#endif
		}
		else if (keyword == "file") {
			// Load with stb_image library
			int width, height, numComponents;
			unsigned char* data = stbi_load(keywords[i + 1].c_str(), &width, &height, &numComponents, 3);

#ifdef _USE_OPENGL
			GLuint texID;
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			stbi_image_free(data);

			result = new TextureGL(texID);
#endif
		}
	}

	if (!result) {
		Log::println("Error: Texture was not initialized properly in LevelLoader during Texture loading");
		assert(false);
		return nullptr;
	}

	if (name != "")
		textures.insert(std::pair<std::string, Texture*>(name, result));

	return result;
}

// Physics Primitives
AABB* LevelLoader::addAABB3D(const std::vector<std::string>& keywords, bool skipFirst) {
	std::string name = "";

	Vec3 position(0, 0, 0);
	Vec3 extents(1, 1, 1);

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "position") {
			position = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "extents") {
			extents = loadVector3D(keywords[i + 1]);
		}
	}

	AABB* aabb = new AABB(position, extents);

	if (name != "")
		aabbs.insert(std::pair<std::string, AABB*>(name, aabb));

	assert(aabb);
	return aabb;
}

Sphere* LevelLoader::addSphere(const std::vector<std::string>& keywords, bool skipFirst) {
	std::string name = "";

	Vec3 position(0, 0, 0);
	float radius = 1;

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "position") {
			position = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "radius") {
			radius = std::stof(keywords[i + 1]);
		}
	}

	Sphere* sphere = new Sphere(position, radius);

	if (name != "")
		spheres.insert(std::pair<std::string, Sphere*>(name, sphere));

	assert(sphere);
	return sphere;
}

Plane* LevelLoader::addPlane(const std::vector<std::string>& keywords, bool skipFirst) {
	std::string name = "";

	Vec3 position;
	Vec3 normal(0, 0, 1);
	Vec3 rotation(0, 0, 0);
	Vec3 tangent(1, 0, 0);
	float scale = 1.0;

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "position") {
			position = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "normal") {
			normal = loadVector3D(keywords[i + 1]);
			if (dot(normal, normal) != 1)
				normal.normalize();
		}
		else if (keyword == "rotation") {
			rotation = loadVector3D(keywords[i + 1]);
			Mat4 rotationMatrix = Mat4().initRotation(rotation);
			normal = rotationMatrix.mulDirection(normal);
		}
		else if (keyword == "tangent") {
			tangent = loadVector3D(keywords[i + 1]).normalize();
		}
		else if (keyword == "scale") {
			scale = std::stof(keywords[i + 1]);
		}
	}

	Plane* plane = new Plane(position, normal);
	plane->rotationMatrix = Mat4().initRotation(rotation);
	plane->tangent = tangent;
	plane->renderingScale = scale;

	if (name != "")
		planes.insert(std::pair<std::string, Plane*>(name, plane));

	assert(plane);
	return plane;
}

OBB* LevelLoader::addOBB(const std::vector<std::string>& keywords, bool skipFirst) {
	std::string name = "";

	Vec3 position;
	Vec3 extents;
	Vec3 u1, u2;
	Vec3 u3(0, 0, 0);

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "position") {
			position = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "extents") {
			extents = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "u1") {
			u1 = loadVector3D(keywords[i + 1]).normalize();
		}
		else if (keyword == "u2") {
			u2 = loadVector3D(keywords[i + 1]).normalize();
		}
		else if (keyword == "u3") {
			if (keywords[i + 1] == "generate") {
				u3 = cross(u1, u2).normalize().normalize();
			}
			else
				u3 = loadVector3D(keywords[i + 1]);
		}
	}

	if (u3.isNull())
		u3 = cross(u1, u2).normalize();

	// TODO: Check if vectors are orthogonal and make a valid OBB (check for degenerate OBBs too).

	Vec3 u[3] = { u1,u2,u3 };
	OBB* obb = new OBB(position, u, extents);

	if (name != "")
		obbs.insert(std::pair<std::string, OBB*>(name, obb));

	assert(obb);
	return obb;
}

Ray* LevelLoader::addRay(const std::vector<std::string>& keywords, bool skipFirst) {
	std::string name = "";

	Vec3 position(0, 0, 0);
	Vec3 direction(0, 0, -1);
	bool acceptNegativeHits = false;

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "position" || keyword == "start") {
			position = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "direction") {
			direction = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "accept_negative") {
			acceptNegativeHits = (bool)std::stof(keywords[i + 1]);
		}
	}

	Ray* ray = new Ray(position, direction, acceptNegativeHits);

	if (name != "")
		rays.insert(std::pair<std::string, Ray*>(name, ray));

	assert(ray);
	return ray;
}

Line* LevelLoader::addLine(const std::vector<std::string>& keywords, bool skipFirst) {
	std::string name = "";

	Vec3 start(0, 0, 0);
	Vec3 end(0, 0, -1);

	int first = 0;
	if (skipFirst)
		first = 2;
	for (int i = first; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "position" || keyword == "start") {
			start = loadVector3D(keywords[i + 1]);
		}
		else if (keyword == "end") {
			end = loadVector3D(keywords[i + 1]);
		}
	}

	Line* line = new Line(start, end);

	if (name != "")
		lines.insert(std::pair<std::string, Line*>(name, line));

	assert(line);
	return line;
}

Movement3D* LevelLoader::addMovement3D(const std::vector<std::string>& keywords, bool skipFirst) {
	std::string name = "";

	Movement3D* movement = new Movement3D(nullptr);

	int start = 0;
	if (skipFirst)
		start = 2;
	for (int i = start; i < keywords.size(); i += 2) {
		std::string keyword = keywords[i];
		keyword = keyword.substr(0, keyword.length() - 1);
		if (keyword == "name") {
			name = keywords[i + 1];
		}
		else if (keyword == "translation") {
			std::vector<std::string> scalars = Utils::splitByRegex(keywords[i + 1], ',');
			movement->t = new MovComponent();
			loadMovComponent(movement->t, scalars);
		}
		else if (keyword == "rotation") {
			std::vector<std::string> scalars = Utils::splitByRegex(keywords[i + 1], ',');
			movement->r = new MovComponent();
			loadMovComponent(movement->r, scalars);
		}
		else if (keyword == "scale") {
			std::vector<std::string> scalars = Utils::splitByRegex(keywords[i + 1], ',');
			movement->s = new MovComponent();
			loadMovComponent(movement->s, scalars);
		}
	}

	if (name != "")
		movements3D.insert(std::pair<std::string, Movement3D*>(name, movement));

	assert(movement);
	return movement;
}

void LevelLoader::loadMovComponent(MovComponent* mov, const std::vector<std::string>& args) {
	assert(args.size() == 3);
	for (int i = 0; i < 3; ++i) {
		//std::string arg = args[i];
		mov->amplitudes[i] = 1;
		mov->frequencies[i] = 1;
		if (args[i].length() >= 3 && args[i].substr(0,3) == "sin") {
			mov->functions[i] = MovFunctions::fsin;
		}
		else if (args[i].length() >= 3 && args[i].substr(0,3) == "cos") {
			mov->functions[i] = MovFunctions::fcos;
		}
		else if (args[i].length() >= 3 && args[i].substr(0,3) == "tan") {
			mov->functions[i] = MovFunctions::ftan;
		}
		else {
			mov->value[i] = std::stof(args[i]);
		}
	}
}

Vec2 LevelLoader::loadVector2D(const std::string& vector) {
	std::vector<std::string> scalars = Utils::splitByRegex(vector, ',');
	if (scalars.size() != 2) {
		Log::println("Error: loadVector2D loaded 2D vector with " + std::to_string(scalars.size()) + " scalars!");
		assert(false);
		return Vec2();
	}
	return Vec2(std::stof(scalars[0]), std::stof(scalars[1]));
}

Vec3 LevelLoader::loadVector3D(const std::string& vector) {
	std::vector<std::string> scalars = Utils::splitByRegex(vector, ',');
	if (scalars.size() != 3) {
		Log::println("Error: loadVector3D loaded 3D vector with " + std::to_string(scalars.size()) + " scalars!");
		assert(false);
		return Vec3();
	}
	return Vec3(std::stof(scalars[0]), std::stof(scalars[1]), std::stof(scalars[2]));
}

std::vector<std::string> LevelLoader::getImplicitArguments(const std::vector<std::string>& keywords, int i) {
	std::vector<std::string> arguments;
	arguments.push_back(keywords[i + 1].substr(1, keywords[i + 1].length() - 1));
	int k = 2;
	while (keywords[i + k].substr(keywords[i + k].length() - 1, 1) != ")") {
		arguments.push_back(keywords[i + k]);
		++k;
	}
	// Add last trailing argument
	arguments.push_back(keywords[i + k].substr(0, keywords[i + k].length() - 1));

	return arguments;
}