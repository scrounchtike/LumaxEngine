
#include "ShaderGL.hpp"
#include "../Shader.hpp"

#ifdef _USE_OPENGL

#include <fstream>

#include "../../RAL/Log.hpp"

#include "../Renderer.hpp"

ShaderGL::ShaderGL(const std::string& shaderPath) : shaderPath(shaderPath) {
	bool success = initialize();
	assert(success);
}

ShaderGL::ShaderGL(const std::string& shaderPath, GLenum shaderType) : shaderPath(shaderPath) {
	bool success = false;
	if(shaderType == GL_VERTEX_SHADER)
		success = initializeVertex();
	else
		success = initializeFragment();
	assert(success);
}

ShaderGL::~ShaderGL() {
	bool success = cleanUp();
	assert(success);
}

void ShaderGL::bind() const {
	glUseProgram(program);
}

void ShaderGL::prepareUniforms() const {
	// NOP. Used for DX11 interface compatibility
}

void ShaderGL::addUniform(const std::string& uniformName) {
	GLint location = glGetUniformLocation(program, uniformName.c_str());
	uniforms.insert(std::pair<std::string, int>(uniformName, location));
}

void ShaderGL::setUniform1i(const std::string &uniformName, int value) {
	glUniform1i(uniforms[uniformName], value);
}

void ShaderGL::setUniform1f(const std::string &uniformName, float x) {
	glUniform1f(uniforms[uniformName], x);
}

void ShaderGL::setUniform2f(const std::string &uniformName, float x, float y) {
	glUniform2f(uniforms[uniformName], x, y);
}

void ShaderGL::setUniform3f(const std::string &uniformName, float x, float y, float z) {
	glUniform3f(uniforms[uniformName], x, y, z);
}

void ShaderGL::setUniform4f(const std::string &uniformName, float x, float y, float z, float w) {
	glUniform4f(uniforms[uniformName], x, y, z, w);
}

void ShaderGL::setUniformMatrix(const std::string &uniformName, const float *matrix, unsigned int size) {
	if (size == 16)
		glUniformMatrix4fv(uniforms[uniformName], 1, GL_TRUE, matrix);
	else if (size == 9)
		glUniformMatrix3fv(uniforms[uniformName], 1, GL_TRUE, matrix);
}

void ShaderGL::setUniformMatrix3f(const std::string& uniformName, const Mat3& matrix) {
	glUniformMatrix3fv(uniforms[uniformName], 1, GL_TRUE, matrix.getHeadPointer());
}

void ShaderGL::setUniformMatrix4f(const std::string &uniformName, const Mat4 &matrix) {
	auto it = uniforms.find(uniformName);
	assert(it != uniforms.end());
	glUniformMatrix4fv(it->second, 1, GL_TRUE, matrix.getHeadPointer());
}

void ShaderGL::setSubroutine(const std::string &name, GLenum shaderType, const std::string& function){
	auto it = subroutines.find(name);
	if(it == subroutines.end())
		Log::println("Error: Subroutine location not found: " + name), assert(false);
	unsigned location = it->second;
	auto it_func = subroutineFuncs.find(function);
	if(it_func == subroutineFuncs.end())
		Log::println("Error: Subroutine function not found: " + function), assert(false);
	unsigned index = it_func->second;

	// Set subroutine index
	//std::cout << "subroutine " << name << " at location: " << location << " with index: " << index << std::endl;
	if(shaderType == GL_VERTEX_SHADER)
		subroutineIndicesVertex[location] = index;
	else
		subroutineIndicesFragment[location] = index;
}

void ShaderGL::updateSubroutines() const {
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, activeSubroutinesVertex, subroutineIndicesVertex);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, activeSubroutinesFragment, subroutineIndicesFragment);
}

bool ShaderGL::initializeVertex(){
	program = glCreateProgram();
	glProgramParameteri(program, GL_PROGRAM_SEPARABLE, true);
	
	bool success = compileVertexShader();
	assert(success);

	glAttachShader(program, vertexShader);
	glLinkProgram(program);
	checkError(program, GL_LINK_STATUS, PROGRAM_TYPE, shaderPath);
	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	
	return true;
}

bool ShaderGL::initializeFragment(){
	program = glCreateProgram();
	glProgramParameteri(program, GL_PROGRAM_SEPARABLE, true);
	
	bool success = compileFragmentShader();
	assert(success);

	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	checkError(program, GL_LINK_STATUS, PROGRAM_TYPE, shaderPath);
	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);
	
	return true;
}

bool ShaderGL::initialize(){
	program = glCreateProgram();
	
	bool success = true;
	success &= compileVertexShader();
	success &= compileFragmentShader();
	assert(success);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	checkError(program, GL_LINK_STATUS, PROGRAM_TYPE, shaderPath);
	
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	// Uniforms
	queryUniforms();
	queryUBOs();

	// Subroutines
	GLsizei size;
	glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &size);
	maxSubroutines = size;
	subroutineIndicesVertex = new GLuint[size];
	subroutineIndicesFragment = new GLuint[size];
	for(int i = 0; i < size; ++i)
		subroutineIndicesVertex[i] = 0, subroutineIndicesFragment[i] = 0;

	glUseProgram(program);
	queryVertexSubroutines();
	queryFragmentSubroutines();
	
	return true;
}

bool ShaderGL::compileVertexShader(){
	vertexShader = createShader(shaderPath + ".vs", GL_VERTEX_SHADER);
	glCompileShader(vertexShader);
	checkError(vertexShader, GL_COMPILE_STATUS, SHADER_TYPE, shaderPath);
	return true;
}

bool ShaderGL::compileFragmentShader() {
	fragmentShader = createShader(shaderPath + ".fs", GL_FRAGMENT_SHADER);
	glCompileShader(fragmentShader);
	checkError(fragmentShader, GL_COMPILE_STATUS, SHADER_TYPE, shaderPath);
	return true;
}

void ShaderGL::queryUniforms(){
  GLint size; // size of the variable
  GLenum type; // type of the variable (float, vec3 or mat4, etc)
	
  const GLsizei bufSize = 64; // maximum name length
  GLchar uniformName[bufSize]; // variable name in GLSL
  GLsizei length; // name length
 
	GLint numUniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
	for(GLuint i = 0; i < numUniforms; ++i)
	{
		glGetActiveUniform(program, i, bufSize, &length, &size, &type, uniformName);
		addUniform(std::string(uniformName));
	}
}

void ShaderGL::queryUBOs(){
	GLint numBlocks;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
	for(int i = 0; i < numBlocks; ++i){
		GLint nameLength;
		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);

		std::vector<GLchar> name;
		name.resize(nameLength);
		glGetActiveUniformBlockName(program, i, nameLength, NULL, &name[0]);

		std::string strName;
		strName.assign(name.begin(), name.end() - 1);
		//Log::println("UBO name: " + strName);
		
		// Bind UBO to binding point
		auto it = Renderer::mapUBOs.find(strName);
		if(it == Renderer::mapUBOs.end()){
			Log::println("Error: UBO name not recognized during shader loading: " + shaderPath);
			assert(false);
			return;
		}else{
			unsigned int bindingPoint = it->second;
			glUniformBlockBinding(program, i, bindingPoint);
		}
	}
}

void ShaderGL::queryVertexSubroutines(){
	// Subroutine uniforms
	GLint numSubroutineUniforms;
	glGetProgramStageiv(program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubroutineUniforms);
	for(int i = 0; i < numSubroutineUniforms; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineUniformName(program, GL_VERTEX_SHADER, i, 32, &length, name);
		GLint location = glGetSubroutineUniformLocation(program, GL_VERTEX_SHADER, name);
		if(location < 0){
			Log::println("Error: subroutine named " + std::string(name) + " not recognized in vertex shader: " + shaderPath + ".vs");
			assert(false);
		}
		subroutines.insert(std::pair<std::string, unsigned>(std::string(name), location));
	}
	// Indices
	GLint numSubroutineFunctions;
	glGetProgramStageiv(program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINES, &numSubroutineFunctions);
	for(int i = 0; i < numSubroutineFunctions; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineName(program, GL_VERTEX_SHADER, i, 32, &length, name);
		GLuint index = glGetSubroutineIndex(program, GL_VERTEX_SHADER, name);
		if(index == GL_INVALID_INDEX){
			Log::println("Error: unrecognized subroutine function : " + std::string(name) + " in vertex shader: " + shaderPath);
			assert(false);
		}
		subroutineFuncs.insert(std::pair<std::string, unsigned>(std::string(name), index));
	}
	glGetProgramStageiv(program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &activeSubroutinesVertex);
}

void ShaderGL::queryFragmentSubroutines(){
	// Subroutine uniforms
	GLint numSubroutineUniforms;
	glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubroutineUniforms);
	for(int i = 0; i < numSubroutineUniforms; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineUniformName(program, GL_FRAGMENT_SHADER, i, 32, &length, name);
		GLint location = glGetSubroutineUniformLocation(program, GL_FRAGMENT_SHADER, name);
		if(location < 0){
			Log::println("Error: subroutine named " + std::string(name) + " not recognized in fragment shader: " + shaderPath + ".fs");
			assert(false);
		}
		std::cout << "Adding subroutine uniform : " << std::string(name) << " at location: " << location << std::endl;
		subroutines.insert(std::pair<std::string, unsigned>(std::string(name), location));
	}
	// Subroutine indices
	GLint numSubroutineFunctions;
	glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINES, &numSubroutineFunctions);
	for(int i = 0; i < numSubroutineFunctions; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineName(program, GL_FRAGMENT_SHADER, i, 32, &length, name);
		GLuint index = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, name);
		if(index == GL_INVALID_INDEX){
			Log::println("Error: unrecognized subroutine function : " + std::string(name) + " in fragment shader: " + shaderPath);
			assert(false);
		}
		std::cout << "Adding subroutine function : " << std::string(name) << " at index: " << index << std::endl;
		subroutineFuncs.insert(std::pair<std::string, unsigned>(std::string(name), index));
	}
	glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &activeSubroutinesFragment);
}

bool ShaderGL::cleanUp() {
	return true;
}

GLuint ShaderGL::createShader(const std::string& filename, GLenum type) {
	GLuint shader = glCreateShader(type);

	std::string shaderSourceString = loadShader(filename);
	const GLchar* shaderSource[1] = { shaderSourceString.c_str() };
	GLint shaderSourceLength[1] = { (GLint)shaderSourceString.length() };
	glShaderSource(shader, 1, shaderSource, shaderSourceLength);

	return shader;
}

std::string ShaderGL::loadShader(const std::string& filename) {
	std::ifstream file;
	file.open(filename);

	if (!file.good()) {
		Log::println("Error while trying to read shader source file: " + filename);
		return NULL;
	}

	std::string line;
	std::string result = "";
	while (std::getline(file, line)) {
		result += line + '\n';
	}

	return result;
}

void ShaderGL::checkError(GLuint object, GLenum pname, TYPE type, const std::string& shaderName) {
	GLint success = 1;
	if (type == SHADER_TYPE)
		glGetShaderiv(object, pname, &success);
	else if (type == PROGRAM_TYPE)
		glGetProgramiv(object, pname, &success);
	if (!success) {
		std::cerr << "An error occured with shader generation: " << shaderName << std::endl;
		GLchar errorBuf[512];
		GLint length = 0;
		if (type == SHADER_TYPE)
			glGetShaderInfoLog(object, 512, &length, errorBuf);
		else if (type == PROGRAM_TYPE)
			glGetProgramInfoLog(object, 512, &length, errorBuf);
		for (int i = 0; i < length; ++i) {
			std::cerr << errorBuf[i];
		}
		std::cout << std::endl;
		
		// Crash for testing purposes
		assert(false);
	}
}

#endif
