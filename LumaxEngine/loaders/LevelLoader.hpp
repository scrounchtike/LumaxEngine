
#ifndef LEVEL_LOADER_HPP
#define LEVEL_LOADER_HPP

#include "../RAL/buildDesc.hpp"

#include <fstream>
#include <string>
#include <vector>

#include "../utils/StringUtils.hpp"
#include "../math.hpp"

#include "../GL/Level.hpp"

#ifdef _USE_DIRECTX11
#include "../RL/RendererDX11.hpp"
#elif defined _USE_OPENGL
#include "../RL/RendererGL.hpp"
#endif

#ifdef _USE_OPENGL
#include "../RL/Model2DGL.hpp"
#elif defined _USE_DIRECTX11
#include "../RL/Model2DDX11.hpp"
#endif

#include "../RL/Material.hpp"
#include "../RL/Mesh2D.hpp"

#include "../RML/ResourceManager.hpp"

#ifdef _USE_OPENGL
#include "../RL/Model3DGL.hpp"
#elif defined _USE_DIRECTX11
#include "../RL/Model3DDX11.hpp"
#endif
#include "../RL/Mesh3D.hpp"

#include "../RL/TextureGL.hpp"

#include "../PL/PhysicsPrimitives.hpp"
#include "../PL/Movement3D.hpp"

class LevelLoader {
public:
	static Level* loadLevel(const std::string& filename, ResourceManager* resManager);
private:
	static void set(const std::vector<std::string>& keywords);
	static void add(const std::vector<std::string>& keywords);

	static Model2D* addModel2D(const std::vector<std::string>& keywords);
	static Mesh2D* addMesh2D(const std::vector<std::string>& keywords);
	static Transform2D* addTransform2D(const std::vector<std::string>& keywords, bool skipFirst = true);

	static FullModel3D* addModel3D(const std::vector<std::string>& keywords, bool skipFirst = true);
	static Mesh3D* addMesh3D(const std::vector<std::string>& keywords);
	static Transform3D* addTransform3D(const std::vector<std::string>& keywords, bool skipFirst = true);

	static Shader* addShader(const std::vector<std::string>& keywords);
	static Material* addMaterial(const std::vector<std::string>& keywords);
	static Texture* addTexture(const std::vector<std::string>& keywords);

	// Physics
	static AABB* addAABB3D(const std::vector<std::string>& keywords, bool skipFirst = true);
	static Sphere* addSphere(const std::vector<std::string>& keywords, bool skipFirst = true);
	static Plane* addPlane(const std::vector<std::string>& keywords, bool skipFirst = true);
	static OBB* addOBB(const std::vector<std::string>& keywords, bool skipFirst = true);
	static Ray* addRay(const std::vector<std::string>& keywords, bool skipFirst = true);
	static Line* addLine(const std::vector<std::string>& keywords, bool skipFirst = true);

	static Movement3D* addMovement3D(const std::vector<std::string>& keywords, bool skipFirst = true);

	// Registry by name
	static std::map<std::string, Model2D*> models2D;
	static std::map<std::string, Mesh2D*> meshes2D;
	static std::map<std::string, Transform2D*> transforms2D;

	static std::map<std::string, FullModel3D*> models3D;
	static std::map<std::string, Mesh3D*> meshes3D;
	static std::map<std::string, Transform3D*> transforms3D;

	static std::map<std::string, Texture*> textures;
	static std::map<std::string, Material*> materials;
	static std::map<std::string, Shader*> shaders;

	static std::map<std::string, AABB*> aabbs;
	static std::map<std::string, Sphere*> spheres;
	static std::map<std::string, Plane*> planes;
	static std::map<std::string, OBB*> obbs;
	static std::map<std::string, Ray*> rays;
	static std::map<std::string, Line*> lines;

	static std::map<std::string, Movement3D*> movements3D;

	// Resource Manager
	static ResourceManager* resManager;

	enum INDEX {
		ACTION, TYPE, ACTION1, VALUE1
	};

	// Global scene variables
	static Vec3 defaultClearColor;
	static Vec3 defaultBaseColor;
	static Vec3 defaultHitColor;

	// Static helper pointers
	static Level* level;

	// Static models 3D
	static FullModel3D* modelCube3D;

	// Helper functions
	static void loadMovComponent(MovComponent* mov, const std::vector<std::string>& args);

	static Vec2 loadVector2D(const std::string& vector);
	static Vec3 loadVector3D(const std::string& vector);

	static std::vector<std::string> getImplicitArguments(const std::vector<std::string>& keywords, int index);
};

#endif