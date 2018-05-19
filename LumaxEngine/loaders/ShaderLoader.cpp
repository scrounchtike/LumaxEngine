
#include "ShaderLoader.hpp"
#include "../RAL/Log.hpp"
#include "../utils/StringUtils.hpp"

#include <fstream>

#ifdef _USE_DIRECTX11

ShaderDX11* ShaderLoader::loadShaderDX11(const std::string& filename) {
	ShaderInformation info;

	// Begin with vertex shader
	std::ifstream file;
	file.open((filename + ".vs").c_str());
	if (!file.good()) {
		Log::println("Error: Could not read DX11 shader given in Shader Loader : " + filename);
		assert(false);
	}

	//State booleans
	bool insideUniformStruct = false;
	bool insideAttributeStruct = false;

	std::string line;
	while (std::getline(file, line)) {
		// Process the line
		// Check if we are in a uniform struct statement
		if (line.length() >= 7 && !line.substr(0, 7).compare("cbuffer")) {
			insideUniformStruct = true;
			continue;
		}
		else if (line.length() >= 2 && insideUniformStruct && !line.compare("};"))
			insideUniformStruct = false;

		// Check if we are in an attribute struct statement
		if (line.length() >= 20 && !line.compare("struct VertexInput {")) {
			insideAttributeStruct = true;
			continue;
		}
		else if (line.length() >= 2 && insideAttributeStruct && !line.compare("};"))
			insideAttributeStruct = false;

		if (!insideUniformStruct && !insideAttributeStruct)
			continue;

		// Remove the tab symbol and trailing semicolon
		if (line[0] == '\t')
			line = line.substr(1, line.length() - 2);

		std::vector<std::string> data = Utils::split(line);
		std::string type = data[0];
		std::string name = data[1];
		if (insideUniformStruct) {
			info.uniformNames.push_back(name);
			info.uniformSizes.push_back(getSize(type));
		}
		else if (insideAttributeStruct) {
			std::string keyword = data[3];
			if (keyword.length() >= 8 && !keyword.substr(0, 8).compare("INSTANCE")) {
				info.instanceAttributeNames.push_back(keyword);
				info.instanceAttributeSizes.push_back(getSize(type));
				info.instanceAttributeKeywords.push_back(keyword);
			}
			else {
				info.attributeNames.push_back(keyword);
				info.attributeSizes.push_back(getSize(type));
				info.attributeKeywords.push_back(keyword);
			}
		}
	}
	file.close();

	// Then pixel shader
	// TODO: Load pixel shader

	// Add path into info struct
	info.shaderPath = filename;

	Log::println("Done loading shader information");

	return new ShaderDX11(info);
}

int ShaderLoader::getSize(const std::string& type) {
	if (type == "float")
		return 1;
	else if (type == "float2")
		return 2;
	else if (type == "float3")
		return 3;
	else if (type == "float4")
		return 4;
	else if (type == "float3x3")
		return 9;
	else if (type == "float4x4")
		return 16;
}

#elif defined _USE_OPENGL

std::vector<StructGLSL> ShaderLoader::structsGLSL;

ShaderGL* ShaderLoader::loadShaderGL(const std::string& filename) {
	ShaderInformation info;
	info.shaderPath = filename;

	loadShaderFile(filename + ".vs", info);
	loadShaderFile(filename + ".fs", info);
	
	return new ShaderGL(info);

	// Load vertex shader first
	std::ifstream file;
	file.open((filename + ".vs").c_str());
	if (!file.good()) {
		Log::println("Error: Could not read GLSL vertex shader given in Shader Loader : " + filename);
		assert(false);
	}

	bool inStruct = false;
	StructGLSL structglsl;

	std::string line;
	while (std::getline(file, line)) {
		// Process the line
		if (line.length() >= 8 && line.substr(0, 8) == "uniform ") {
			// Uniform statement
			std::vector<std::string> data = Utils::split(line);
			int uniformSize = getSize(data[1]);
			std::string uniformName = data[2];
			uniformName = uniformName.substr(0, uniformName.size() - 1);
			// Check if uniform is an array
			if(uniformName.substr(uniformName.length() - 1, 1) == "]"){
				int arraySize = std::stoi(uniformName.substr(uniformName.length() - 2, 1));
				for(int i = 0; i < arraySize; ++i){
					std::string baseName = uniformName.substr(0, uniformName.length() - 2) + std::to_string(i) + "]";
					if(uniformSize == -1){
						// Uniform is a struct
						StructGLSL* structglsl = nullptr;
						for (int k = 0; k < structsGLSL.size(); ++k) {
							if (structsGLSL[k].type == data[1]) {
								// Uniform match
								structglsl = &structsGLSL[k];
								break;
							}
						}
						if (!structglsl) {
							Log::println("Error: Could not find uniform type (possibly a struct) in shader loading GLSL");
							assert(false);
						}
						// Add struct uniforms
						for (int k = 0; k < structglsl->attributes.size(); ++k) {
							info.uniformNames.push_back(baseName + "." + structglsl->attributes[k].first);
							info.uniformSizes.push_back(getSize(structglsl->attributes[k].second));
						}
					}else{
						info.uniformNames.push_back(baseName);
						info.uniformSizes.push_back(uniformSize);
					}
				}
			}else{
				if (uniformSize == -1) {
					// Uniform type is a struct
					StructGLSL* structglsl = nullptr;
					for (int k = 0; k < structsGLSL.size(); ++k) {
						if (structsGLSL[k].type == data[1]) {
							// Uniform match
							structglsl = &structsGLSL[k];
							break;
						}
					}
					if (!structglsl) {
						Log::println("Error: Could not find uniform type (possibly a struct) in shader loading GLSL");
						assert(false);
					}
					// Add struct uniforms
					for (int k = 0; k < structglsl->attributes.size(); ++k) {
						info.uniformNames.push_back(uniformName + "." + structglsl->attributes[k].first);
						info.uniformSizes.push_back(getSize(structglsl->attributes[k].second));
					}
				}
				else {
					// Add uniform to structure
					info.uniformNames.push_back(uniformName);
					info.uniformSizes.push_back(uniformSize);
				}
			}
		}
		else if (line.length() >= 7 && line.substr(0, 7) == "struct ") {
			inStruct = true;
			std::vector<std::string> statement = Utils::split(line);
			structglsl = StructGLSL();
			structglsl.type = statement[1];
		}
		else if (inStruct) {
			if (line.length() == 2 && line.substr(0, 2) == "};") {
				inStruct = false;
				// Add final struct to the list
				structsGLSL.push_back(structglsl);
			}
			else {
				// Remove the tab symbol
				if (line[0] == '\t')
					line = line.substr(1, line.length() - 1);
				// Register attributes
				std::vector<std::string> data = Utils::split(line);
				structglsl.attributes.push_back(std::pair<std::string, std::string>(data[1].substr(0,data[1].length() - 1), data[0]));
			}
		}
	}
	file.close();

	// Load fragment shader
	file.open((filename + ".fs").c_str());
	if (!file.good()) {
		Log::println("Error: Could not read GLSL fragment shader given in Shader Loader : " + filename);
		assert(false);
	}

	while (std::getline(file, line)) {
		// Process the line
		if (line.length() >= 8 && line.substr(0, 8) == "uniform ") {
			// Uniform statement
			std::vector<std::string> data = Utils::split(line);
			// Continue if sampler2D
			if (data[1] == "sampler2D")
				continue;
			int uniformSize = getSize(data[1]);
			std::string uniformName = data[2];
			uniformName = uniformName.substr(0, uniformName.size() - 1);
			// Check if uniform is an array
			if(uniformName.substr(uniformName.length() - 1, 1) == "]"){
				int arraySize = std::stoi(uniformName.substr(uniformName.length() - 2, 1));
				for(int i = 0; i < arraySize; ++i){
					std::string baseName = uniformName.substr(0, uniformName.length() - 2) + std::to_string(i) + "]";
					if(uniformSize == -1){
						// Uniform is a struct
						StructGLSL* structglsl = nullptr;
						for (int k = 0; k < structsGLSL.size(); ++k) {
							if (structsGLSL[k].type == data[1]) {
								// Uniform match
								structglsl = &structsGLSL[k];
								break;
							}
						}
						if (!structglsl) {
							Log::println("Error: Could not find uniform type (possibly a struct) in shader loading GLSL");
							assert(false);
						}
						// Add struct uniforms
						for (int k = 0; k < structglsl->attributes.size(); ++k) {
							info.uniformNames.push_back(baseName + "." + structglsl->attributes[k].first);
							info.uniformSizes.push_back(getSize(structglsl->attributes[k].second));
						}
					}else{
						info.uniformNames.push_back(baseName);
						info.uniformSizes.push_back(uniformSize);
					}
				}
			}else{
				if (uniformSize == -1) {
					// Uniform type is a struct
					StructGLSL* structglsl = nullptr;
					for (int k = 0; k < structsGLSL.size(); ++k) {
						if (structsGLSL[k].type == data[1]) {
							// Uniform match
							structglsl = &structsGLSL[k];
							break;
						}
					}
					if (!structglsl) {
						Log::println("Error: Could not find uniform type (possibly a struct) in shader loading GLSL");
						assert(false);
					}
					// Add struct uniforms
					for (int k = 0; k < structglsl->attributes.size(); ++k) {
						info.uniformNames.push_back(uniformName + "." + structglsl->attributes[k].first);
						info.uniformSizes.push_back(getSize(structglsl->attributes[k].second));
					}
				}
				else {
					// Add uniform to structure
					info.uniformNames.push_back(uniformName);
					info.uniformSizes.push_back(uniformSize);
				}
			}
		}
		else if (line.length() >= 7 && line.substr(0, 7) == "struct ") {
			inStruct = true;
			std::vector<std::string> statement = Utils::split(line);
			structglsl = StructGLSL();
			structglsl.type = statement[1];
		}
		else if (inStruct) {
			if (line.length() == 2 && line.substr(0, 2) == "};") {
				inStruct = false;
				// Add final struct to the list
				structsGLSL.push_back(structglsl);
			}
			else {
				// Remove the tab symbol
				if (line[0] == '\t')
					line = line.substr(1, line.length() - 1);
				// Register attributes
				std::vector<std::string> data = Utils::split(line);
				structglsl.attributes.push_back(std::pair<std::string, std::string>(data[1].substr(0, data[1].length() - 1), data[0]));
			}
		}
	}
	file.close();

	return new ShaderGL(info);
}

void ShaderLoader::loadShaderFile(const std::string& filename, ShaderInformation& info){
	StructGLSL structglsl;
	// Load file
	std::ifstream file;
	file.open(filename.c_str());
	if (!file.good()) {
		Log::println("Error: Could not read GLSL  shader given in Shader Loader : " + filename);
		assert(false);
	}

	// Read file
	bool inStruct = false;
	std::string line;
	while(std::getline(file, line)){
		if (line.length() >= 8 && line.substr(0, 8) == "uniform ") {
			// Uniform statement
			std::vector<std::string> data = Utils::split(line);
			// Exit if sampler2D
			if (data[1] == "sampler2D")
				continue;
			// Get uniform name and size
			int uniformSize = getSize(data[1]);
			std::string uniformName = data[2];
			// Remove trailing semicolon
			uniformName = uniformName.substr(0, uniformName.size() - 1);
			int numUniforms = 1;
			// Check if uniform is an array
			std::string baseName = "";
			if(uniformName.substr(uniformName.length() - 1, 1) == "]"){
				// Uniform is an array
				numUniforms = std::stoi(uniformName.substr(uniformName.length() - 2, 1));
				baseName = uniformName.substr(0, uniformName.length() - 3);
			}
			if(numUniforms == 1)
				loadUniform(uniformName, data[1], info);
			else{
				for(int i = 0; i < numUniforms; ++i){
					std::string name = baseName + "[" + std::to_string(i) + "]";
					loadUniform(name, data[1], info);
				}
			}
		}
		else if (line.length() >= 7 && line.substr(0, 7) == "struct ") {
			inStruct = true;
			std::vector<std::string> statement = Utils::split(line);
			structglsl = StructGLSL();
			structglsl.type = statement[1];
		}
		else if(inStruct){
			if (line.length() == 2 && line.substr(0, 2) == "};") {
				inStruct = false;
				// Add final struct to the list
				structsGLSL.push_back(structglsl);
			}
			else{
				// Actual struct data loading
				// Remove the tab symbol
				if (line[0] == '\t')
					line = line.substr(1, line.length() - 1);
				if(line[0] == '/' && line[1] == '/')
					continue;
				// Register attributes
				std::vector<std::string> data = Utils::split(line);
				std::cout << "registering" << std::endl;
				std::cout << line << std::endl;
				std::cout << data[0] << " " << data[1] << std::endl;
				structglsl.attributes.push_back(std::pair<std::string, std::string>(data[1].substr(0, data[1].length() - 1), data[0]));
			}
		}
	}

	// Close file
	file.close();
}

void ShaderLoader::loadUniform(const std::string &uniformName, const std::string& uniformSizeStr, ShaderInformation& info){
	std::cout << "called with: " << std::endl;
	std::cout << uniformSizeStr << " " << uniformName << std::endl;
	
	int uniformSize = getSize(uniformSizeStr);
	if(uniformSize > 0){
		// Built-in type
		info.uniformNames.push_back(uniformName);
		info.uniformSizes.push_back(uniformSize);
	}else{
		// Not a built-in type
		// Assume struct and load uniform as struct
		StructGLSL* structglsl = nullptr;
		for (int k = 0; k < structsGLSL.size(); ++k) {
			if (structsGLSL[k].type == uniformSizeStr) {
				// Uniform match
				structglsl = &structsGLSL[k];
				break;
			}
		}
		if (!structglsl) {
			Log::println("Error: Could not find uniform type (possibly a struct) during GLSL shader loading");
			Log::println(uniformSizeStr + " " + uniformName);
			assert(false);
		}
		// We have a valid struct
		// Add struct uniforms
		for (int k = 0; k < structglsl->attributes.size(); ++k) {
			std::string newUniformName = uniformName + "." + structglsl->attributes[k].first;
			std::string newUniformSizeStr = structglsl->attributes[k].second;
			loadUniform(newUniformName, newUniformSizeStr, info);
		}
	}
}

int ShaderLoader::getSize(const std::string& type) {
	if (type == "float")
		return 1;
	else if (type == "vec2")
		return 2;
	else if (type == "vec3")
		return 3;
	else if (type == "vec4")
		return 4;
	else if (type == "mat3")
		return 9;
	else if (type == "mat4")
		return 16;
	else if (type == "sampler2D")
		return -2;
	else
		return -1;
}

#endif
