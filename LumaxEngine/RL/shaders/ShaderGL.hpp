
#ifndef SHADER_GL_HPP
#define SHADER_GL_HPP

#include <map>

#include "../../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

#include "../../RAL/UsingOpenGL.hpp"
#include "../../math.hpp"

struct ShaderInformation;

class Shader;

class ShaderGL {
	friend class Shader;
public:
	ShaderGL(const std::string& shaderPath);
	ShaderGL(const std::string& shaderPath, GLenum shaderType);
	~ShaderGL();

	void bind() const;
	void prepareUniforms() const;
	void updateSubroutines() const;

	void addUniform(const std::string& uniformName);

	void setUniform1i(const std::string& uniformName, int value);
	void setUniform1f(const std::string& uniformName, float x);
	void setUniform2f(const std::string& uniformName, float x, float y);
	void setUniform3f(const std::string& uniformName, float x, float y, float z);
	void setUniform4f(const std::string& uniformName, float x, float y, float z, float w);
	void setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size);
	void setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix);
	void setUniformMatrix4f(const std::string& uniformName, const Mat4& matrix);

	void setSubroutine(const std::string& name, GLenum shaderType, const std::string& function);
private:
	bool initialize();
	bool initializeVertex();
	bool initializeFragment();
	bool cleanUp();

	bool compileVertexShader();
	bool compileFragmentShader();

	void queryUniforms();
	void queryUBOs();
	void queryVertexSubroutines();
	void queryFragmentSubroutines();
	
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	std::string shaderPath;
	
	std::map<std::string, int> uniforms;
	// Subroutine indices
	GLsizei maxSubroutines;
	GLint activeSubroutinesVertex, activeSubroutinesFragment;
	GLuint* subroutineIndicesVertex;
	GLuint* subroutineIndicesFragment;
	std::map<std::string, unsigned> subroutines;
	std::map<std::string, unsigned> subroutineFuncs;
	
	static GLuint createShader(const std::string& filename, GLenum type);
	static std::string loadShader(const std::string& filename);
	
	enum TYPE {
		SHADER_TYPE, PROGRAM_TYPE
	};
	static void checkError(GLuint shader, GLenum pname, TYPE type, const std::string& shaderName);
};

#endif
#endif
