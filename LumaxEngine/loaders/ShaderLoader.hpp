
#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include "../RAL/UsingDX11.hpp"

#include "../RL/Shader.hpp"

#include <string>
#include <vector>

struct StructGLSL {
	std::string type;
	typedef std::pair<std::string, std::string> pair_type;
	std::vector<pair_type> attributes;
};

class ShaderLoader {
public:
	
#ifdef _USE_DIRECTX11
	static Shader* loadShaderDX11(const std::string& filename);
	static int getSize(const std::string& type);
	
#elif defined _USE_OPENGL
	static Shader* loadShaderGL(const std::string& filename);

	static void loadShaderFile(const std::string& filename, ShaderInformation& info);
	static void loadUniform(const std::string& uniformName, const std::string& uniformSizeStr, ShaderInformation& info);
	
	static int getSize(const std::string& type);
#endif

	static std::vector<StructGLSL> structsGLSL;
};

#endif
