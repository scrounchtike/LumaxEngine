
#ifndef SHADER_GL_HPP
#define SHADER_GL_HPP

#include "Shader.hpp"

#include <map>

#include "../RAL/buildDesc.hpp"

#ifdef _USE_OPENGL

#include "../RAL/UsingOpenGL.hpp"

class ShaderGL : public Shader {
public:
	ShaderGL(ShaderInformation info);
	virtual ~ShaderGL();

	virtual void bind() const;

	virtual void addUniform(const std::string& uniformName);

	virtual void setUniform1i(const std::string& uniformName, int value);
	virtual void setUniform1f(const std::string& uniformName, float x);
	virtual void setUniform2f(const std::string& uniformName, float x, float y);
	virtual void setUniform2f(const std::string& uniformName, const Vec2& value);
	virtual void setUniform3f(const std::string& uniformName, float x, float y, float z);
	virtual void setUniform3f(const std::string& uniformName, const Vec3& value);
	virtual void setUniform4f(const std::string& uniformName, float x, float y, float z, float w);
	virtual void setUniform4f(const std::string& uniformName, const Vec4& value);
	virtual void setUniform4f(const std::string& uniformName, const Quaternion& value);
	virtual void setUniformMatrix(const std::string& uniformName, const float* matrix, unsigned int size);
	virtual void setUniformMatrix(const std::string& uniformName, const Mat4& matrix);
	virtual void setUniformMatrix(const std::string& uniformName, const Mat3& matrix);
private:
	virtual bool initialize(ShaderInformation info);
	virtual bool cleanUp();

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	std::map<std::string, int> uniforms;

	static GLuint createShader(const std::string& filename, GLenum type);
	static std::string loadShader(const std::string& filename);

	enum TYPE {
		SHADER_TYPE, PROGRAM_TYPE
	};
	static void checkError(GLuint shader, GLenum pname, TYPE type);
};

#endif
#endif
