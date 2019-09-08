
#ifndef SHADER_PIPELINE_HPP
#define SHADER_PIPELINE_HPP

#include "../RAL/UsingOpenGL.hpp"
#include "Shader.hpp"

#include <vector>
#include <map>
#include <string>
#include <unordered_map>

#include "../math/types.hpp"

#include "../utils/StringHasher.hpp"

class ShaderPipeline {
public:
	const uint16 ID;
	
	ShaderPipeline(Shader* vertex, Shader* fragment);
	ShaderPipeline(Shader* shader);

	void bind() const;

	// Uniforms
	void addUniform(const std::string& name);
	void setUniform1i(const std::string& name, int value);
	void setUniform1f(const std::string& name, float value);
	void setUniform2f(const std::string& name, float x, float y);
	void setUniform2f(const std::string& name, const Vec2& value);
	void setUniform3f(const std::string& name, float x, float y, float z);
	void setUniform3f(const std::string& name, const Vec3& value);
	void setUniform4f(const std::string& name, float x, float y, float z, float w);
	void setUniform4f(const std::string& name, const Vec4& value);
	void setUniform4f(const std::string& name, const Quat4& value);
	void setUniformMatrix4f(const std::string& name, const Mat4& matrix);

	// Subroutines have to be added manually
	// Special subroutines are detected by name for now
	//
	// getFinalColor - subroutine for choosing between texture, color or blended
	//
	bool usingColorSubroutine = false;
	
	void addSubroutine(const std::string& name)
	{
		// Somehow this doesnt work if "getFinalColor" is passed
		// as a literal to the CRC32_STR function (??!)
		constexpr const char* getFinalColorStr = "getFinalColor";
		using StringHasher::crc32;
		enum Hashes
		{
			HASH_GET_FINAL_COLOR = CRC32_STR(getFinalColorStr),
		};
		
		switch(CRC32_STR(name.c_str()))
		{
		case HASH_GET_FINAL_COLOR:
			usingColorSubroutine = true;
			break;
		};
	}

	
	void setSubroutine(const std::string& name, GLenum shaderType, const std::string& subroutineName);
	void updateSubroutines();

	bool stagedPipeline = false;

	Shader* getShader() const { return shader; }

	// TEMPORARY function for manually adding a sampler2D name -> index
	void addSampler2D(const std::string& name, unsigned index)
	{
		bind();
		glActiveTexture(GL_TEXTURE0 + index);
		glEnable(GL_TEXTURE_2D);
		addUniform(name);
		setUniform1i(name, index);
		sampler2Ds.insert(std::pair<unsigned, std::string>(index, name));
	}
	
	std::string getSamplerName(unsigned index)
	{
		std::map<unsigned, std::string>::const_iterator it = sampler2Ds.find(index);
		if(it != sampler2Ds.end())
			return it->second;
		return "";
	}
	
	unsigned getNumSamplers2D()
	{
		return sampler2Ds.size();
	}
private:
	void queryUniforms();
	void queryUBOs(GLuint program);
	void queryVertexSubroutines();
	void queryFragmentSubroutines();

	// Uniform maps
	std::map<std::string, std::pair<GLuint, GLuint> > uniforms;
	std::map<unsigned, std::string> sampler2Ds;
	
	// Subroutine indices
	GLsizei maxSubroutines;
	GLint activeSubroutinesVertex, activeSubroutinesFragment;
	GLuint* subroutineIndicesVertex;
	GLuint* subroutineIndicesFragment;
	std::map<std::string, unsigned> subroutines;
	std::map<std::string, unsigned> subroutineFuncs;

	Shader* vertexShader,* fragmentShader;
	GLuint pipeline;
	Shader* shader;
};

#endif
