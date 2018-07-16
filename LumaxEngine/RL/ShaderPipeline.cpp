
#include "ShaderPipeline.hpp"

#include "Renderer.hpp"

ShaderPipeline::ShaderPipeline(Shader* vertex, Shader* fragment) : stagedPipeline(true), vertexShader(vertex), fragmentShader(fragment) {
	glGenProgramPipelines(1, &pipeline);
	glBindProgramPipeline(pipeline);
	
	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexShader->getProgram());
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentShader->getProgram());

	glValidateProgramPipeline(pipeline);

	// Uniforms
	queryUniforms();
	queryUBOs(vertex->getProgram());
	queryUBOs(fragment->getProgram());

	// Subroutines
	GLsizei size;
	glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &size);
	maxSubroutines = size;
	subroutineIndicesVertex = new GLuint[size];
	subroutineIndicesFragment = new GLuint[size];
	for(int i = 0; i < size; ++i)
		subroutineIndicesVertex[i] = 0, subroutineIndicesFragment[i] = 0;
	queryVertexSubroutines();
	queryFragmentSubroutines();
}

ShaderPipeline::ShaderPipeline(Shader* shader) : shader(shader) {
	
}

void ShaderPipeline::bind() const {
	if(stagedPipeline)
		glUseProgram(0), glBindProgramPipeline(pipeline);
	else
		shader->bind();
}

void ShaderPipeline::setUniform1i(const std::string& name, int value){
	if(!stagedPipeline){
		shader->setUniform1i(name, value);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform1i(uniform.first, uniform.second, value);
}

void ShaderPipeline::setUniform1f(const std::string& name, float value){
	if(!stagedPipeline){
		shader->setUniform1f(name, value);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform1f(uniform.first, uniform.second, value);
}

void ShaderPipeline::setUniform2f(const std::string& name, float x, float y){
	if(!stagedPipeline){
		shader->setUniform2f(name, x, y);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform2f(uniform.first, uniform.second, x, y);
}

void ShaderPipeline::setUniform2f(const std::string& name, const Vec2& value){
	if(!stagedPipeline){
		shader->setUniform2f(name, value.x, value.y);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform2f(uniform.first, uniform.second, value.x, value.y);
}

void ShaderPipeline::setUniform3f(const std::string& name, float x, float y, float z){
	if(!stagedPipeline){
		shader->setUniform3f(name, x, y, z);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform3f(uniform.first, uniform.second, x, y, z);
}

void ShaderPipeline::setUniform3f(const std::string& name, const Vec3& value){
	if(!stagedPipeline){
		shader->setUniform3f(name, value.x, value.y, value.z);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform3f(uniform.first, uniform.second, value.x, value.y, value.z);
}

void ShaderPipeline::setUniform4f(const std::string& name, float x, float y, float z, float w){
	if(!stagedPipeline){
		shader->setUniform4f(name, x, y, z, w);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform4f(uniform.first, uniform.second, x, y, z, w);
}

void ShaderPipeline::setUniform4f(const std::string& name, const Vec4& value){
	if(!stagedPipeline){
		shader->setUniform4f(name, value.x, value.y, value.z, value.w);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform4f(uniform.first, uniform.second, value.x, value.y, value.z, value.w);
}

void ShaderPipeline::setUniform4f(const std::string& name, const Quat4& value){
	if(!stagedPipeline){
		shader->setUniform4f(name, value.x, value.y, value.z, value.w);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniform4f(uniform.first, uniform.second, value.x, value.y, value.z, value.w);
}

void ShaderPipeline::setUniformMatrix4f(const std::string& name, const Mat4& matrix){
	if(!stagedPipeline){
		shader->setUniformMatrix4f(name, matrix);
		return;
	}
	auto uniform = uniforms.find(name)->second;
	glProgramUniformMatrix4fv(uniform.first, uniform.second, GL_FLOAT, false, matrix.getHeadPointer());
}

void ShaderPipeline::setSubroutine(const std::string &name, GLenum shaderType, const std::string& function){
	if(!stagedPipeline){
		shader->setSubroutine(name, shaderType, function);
		return;
	}
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

void ShaderPipeline::updateSubroutines() {
	if(!stagedPipeline){
		shader->updateSubroutines();
		return;
	}
	glActiveShaderProgram(pipeline, vertexShader->getProgram());
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, activeSubroutinesVertex, subroutineIndicesVertex);
	glActiveShaderProgram(pipeline, fragmentShader->getProgram());
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, activeSubroutinesFragment, subroutineIndicesFragment);
}

void ShaderPipeline::queryUniforms(){
	GLsizei numUniforms;
	glUseProgram(0);
	GLint size;
	GLenum type;
	const GLsizei bufSize = 32;
	GLchar name[bufSize];
	GLsizei length;
		
	glActiveShaderProgram(pipeline, vertexShader->getProgram());
	glGetProgramiv(vertexShader->getProgram(), GL_ACTIVE_UNIFORMS, &numUniforms);
	for(int i = 0; i < numUniforms; ++i){
		glGetActiveUniform(vertexShader->getProgram(), (GLuint)i, bufSize, &length, &size, &type, name);
		GLuint location = glGetUniformLocation(vertexShader->getProgram(), name);
		std::pair<GLuint, GLuint> uniform = std::pair<GLuint, GLuint>(vertexShader->getProgram(), location);
		uniforms.insert(std::pair<std::string, std::pair<GLuint, GLuint> >(std::string(name), uniform));
	}
	glActiveShaderProgram(pipeline, fragmentShader->getProgram());
	glGetProgramiv(fragmentShader->getProgram(), GL_ACTIVE_UNIFORMS, &numUniforms);
	for(int i = 0; i < numUniforms; ++i){
		glGetActiveUniform(fragmentShader->getProgram(), (GLuint)i, bufSize, &length, &size, &type, name);
		GLuint location = glGetUniformLocation(fragmentShader->getProgram(), name);
		std::pair<GLuint, GLuint> uniform = std::pair<GLuint, GLuint>(fragmentShader->getProgram(), location);
		uniforms.insert(std::pair<std::string, std::pair<GLuint, GLuint> >(std::string(name), uniform));
	}	
}

void ShaderPipeline::queryUBOs(GLuint program){
	glActiveShaderProgram(pipeline, program);
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
		
		// Bind UBO to binding point
		auto it = Renderer::mapUBOs.find(strName);
		if(it == Renderer::mapUBOs.end()){
			Log::println("Error: UBO name not recognized during shader pipeline loading");
			assert(false);
			return;
		}else{
			unsigned int bindingPoint = it->second;
			glUniformBlockBinding(program, i, bindingPoint);
		}
	}
}

void ShaderPipeline::queryVertexSubroutines(){
	// Subroutine uniforms
	glActiveShaderProgram(pipeline, vertexShader->getProgram());
	GLint numSubroutineUniforms;
	glGetProgramStageiv(vertexShader->getProgram(), GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubroutineUniforms);
	for(int i = 0; i < numSubroutineUniforms; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineUniformName(vertexShader->getProgram(), GL_VERTEX_SHADER, i, 32, &length, name);
		GLint location = glGetSubroutineUniformLocation(vertexShader->getProgram(), GL_VERTEX_SHADER, name);
		if(location < 0){
			//Log::println("Error: subroutine named " + info.subroutinesVertex[i] + " not recognized in vertex shader: " + info.shaderPath + ".vs");
			assert(false);
		}
		subroutines.insert(std::pair<std::string, unsigned>(std::string(name), location));
	}
	// Indices
	GLint numSubroutineFunctions;
	glGetProgramStageiv(vertexShader->getProgram(), GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINES, &numSubroutineFunctions);
	for(int i = 0; i < numSubroutineFunctions; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineName(vertexShader->getProgram(), GL_VERTEX_SHADER, i, 32, &length, name);
		GLuint index = glGetSubroutineIndex(vertexShader->getProgram(), GL_VERTEX_SHADER, name);
		if(index == GL_INVALID_INDEX){
			//Log::println("Error: unrecognized subroutine function : " + info.subroutineFuncsVertex[i] + " in vertex shader: " + info.shaderPath);
			assert(false);
		}
		subroutineFuncs.insert(std::pair<std::string, unsigned>(std::string(name), index));
	}
	glGetProgramStageiv(vertexShader->getProgram(), GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &activeSubroutinesVertex);
}

void ShaderPipeline::queryFragmentSubroutines(){
	// Subroutine uniforms
	glActiveShaderProgram(pipeline, fragmentShader->getProgram());
	GLint numSubroutineUniforms;
	glGetProgramStageiv(fragmentShader->getProgram(), GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubroutineUniforms);
	for(int i = 0; i < numSubroutineUniforms; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineUniformName(fragmentShader->getProgram(), GL_FRAGMENT_SHADER, i, 32, &length, name);
		GLint location = glGetSubroutineUniformLocation(fragmentShader->getProgram(), GL_FRAGMENT_SHADER, name);
		if(location < 0){
			//Log::println("Error: subroutine named " + info.subroutinesFragment[i] + " not recognized in fragment shader: " + info.shaderPath + ".fs");
			assert(false);
		}
		std::cout << "Adding subroutine uniform : " << std::string(name) << " at location: " << location << std::endl;
		subroutines.insert(std::pair<std::string, unsigned>(std::string(name), location));
	}
	// Subroutine indices
	GLint numSubroutineFunctions;
	glGetProgramStageiv(fragmentShader->getProgram(), GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINES, &numSubroutineFunctions);
	for(int i = 0; i < numSubroutineFunctions; ++i){
		GLchar name[32];
		GLsizei length;
		glGetActiveSubroutineName(fragmentShader->getProgram(), GL_FRAGMENT_SHADER, i, 32, &length, name);
		GLuint index = glGetSubroutineIndex(fragmentShader->getProgram(), GL_FRAGMENT_SHADER, name);
		if(index == GL_INVALID_INDEX){
			//Log::println("Error: unrecognized subroutine function : " + info.subroutineFuncsFragment[i] + " in fragment shader: " + info.shaderPath);
			assert(false);
		}
		std::cout << "Adding subroutine function : " << std::string(name) << " at index: " << index << std::endl;
		subroutineFuncs.insert(std::pair<std::string, unsigned>(std::string(name), index));
	}
	glGetProgramStageiv(fragmentShader->getProgram(), GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &activeSubroutinesFragment);
}
