
#ifndef SHADER_PIPELINE_HPP
#define SHADER_PIPELINE_HPP

#include "../RAL/UsingOpenGL.hpp"
#include "Shader.hpp"

#include <vector>
#include <map>
#include <string>

class ShaderPipeline {
public:
	ShaderPipeline(Shader* vertex, Shader* fragment);
	ShaderPipeline(Shader* shader);

	void bind() const;

	// Uniforms
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

	void setSubroutine(const std::string& name, GLenum shaderType, const std::string& subroutineName);
	void updateSubroutines();

	bool stagedPipeline = false;

	Shader* getShader() const { return shader; }
private:
	void queryUniforms();
	void queryUBOs(GLuint program);
	void queryVertexSubroutines();
	void queryFragmentSubroutines();

	std::map<std::string, std::pair<GLuint, GLuint> > uniforms;
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
