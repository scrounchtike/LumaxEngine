
#include "RendererGL.hpp"

#include "../../RAL/Log.hpp"
#include "../../main.hpp"
#include "../../GL/Level.hpp"

#ifdef _USE_OPENGL

#include <cassert>

RendererGL::RendererGL(Camera* camera) : camera(camera) {
	bool success = initializeGeometries();
	success &= initializeShaders();
	success &= initializeFramebuffers();
	assert(success);
}

RendererGL::~RendererGL() {
	bool success = cleanUpGeometries();
	success &= cleanUpShaders();
	success &= cleanUpFramebuffers();
	assert(success);
}

/*
* Rendering of individual primitives
*/

void RendererGL::renderPoint2D(const Point2D& point) const {
	glBindVertexArray(vaoIDpoint2D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLine2D(const Line2D& line) const {
	glBindVertexArray(vaoIDline2D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprite2D(const Sprite2D& sprite) const {
	glBindVertexArray(vaoIDsprite2D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderPoint3D(const Point3D& point) const {
	glBindVertexArray(vaoIDpoint3D);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDpoint3D);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLine3D(const Line3D& line) const {
	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprite3D(const Sprite3D& sprite) const {
	assert(false);
}

void RendererGL::renderMesh2D(const Mesh2D& mesh2D) const {
	mesh2D.render();
}

void RendererGL::renderMesh3D(const Mesh3D& mesh3D) const {
	mesh3D.render(camera);
}

void RendererGL::renderLightedMesh3D(const Mesh3D& mesh3D) const {
	// TODO: Do lighted 3D rendering
}

void RendererGL::renderAABB(const Mesh3D& aabb) const {
	glBindVertexArray(vaoIDcube3D);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSphere(const Mesh3D& sphere) const {
	glBindVertexArray(vaoIDsphere3D);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDsphere3D);
	glDrawElements(GL_TRIANGLES, numIndicesSphere, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderPlane(const Mesh3D& plane) const {
	glBindVertexArray(vaoIDplane3D);
	glEnableVertexAttribArray(0);
	if (plane.material->getTexture())
		glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	if (plane.material->getTexture())
		glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderOBB(const Mesh3D& obb) const {
	glBindVertexArray(vaoIDcube3D);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderRay(const Mesh3D& ray) const {
	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLine(const Mesh3D& line) const {
	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

/* 
* Rendering of primitive vectors 
*/

void RendererGL::renderPoints2D(const std::vector<Point2D*>& points) const {
	shaderPoint2D->bind();
	glBindVertexArray(vaoIDpoint2D);
	glEnableVertexAttribArray(0);
	for (Point2D* point : points) {
		shaderPoint2D->setUniform2f("position", point->position);
		shaderPoint2D->setUniform3f("color", point->color);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLines2D(const std::vector<Line2D*>& lines) const {
	shaderLine2D->bind();
	glBindVertexArray(vaoIDline2D);
	glEnableVertexAttribArray(0);
	for (Line2D* line : lines) {
		shaderLine2D->setUniform2f("A", line->A);
		shaderLine2D->setUniform2f("B", line->B);
		shaderLine2D->setUniform3f("color", line->color);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprites2D(const std::vector<Sprite2D*>& sprites) const {
	shaderSprite2D->bind();
	glBindVertexArray(vaoIDsprite2D);
	glEnableVertexAttribArray(0);
	for (Sprite2D* sprite : sprites) {
		shaderSprite2D->setUniform2f("position", sprite->position);
		shaderSprite2D->setUniform2f("extents", sprite->extents);
		shaderSprite2D->setUniform3f("color", sprite->color);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderPoints3D(const std::vector<Point3D*>& points) const {
	assert(camera);
	//glDisable(GL_CULL_FACE);
	shaderPoint3D->bind();
	shaderPoint3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderPoint3D->setUniformMatrix4f("view", camera->getViewMatrix());
	glBindVertexArray(vaoIDpoint3D);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDpoint3D);
	for (Point3D* point : points) {
		shaderPoint3D->setUniform3f("position", point->position);
		shaderPoint3D->setUniform3f("color", point->color);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLines3D(const std::vector<Line3D*>& lines) const {
	assert(camera);
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix4f("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	for (Line3D* line : lines) {
		shaderLine3D->setUniform3f("start", line->start);
		shaderLine3D->setUniform3f("end", line->end);
		shaderLine3D->setUniform3f("color", line->color);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSprites3D(const std::vector<Sprite3D*>& sprites) const {
	
}

void RendererGL::renderMeshes2D(const std::vector<Mesh2D*>& meshes2D) const {
	if (!meshes2D.size())
		return;
	// NOT WORKING : Could be different gometries &/+ materials &/+ shaders..
	//meshes2D[0]->bindForRender();
	//for (Mesh2D* mesh : meshes2D)
	//	mesh->renderBuffersOnly();
	//meshes2D[0]->unbindForRender();

	for (Mesh2D* mesh : meshes2D) {
		mesh->render();
	}
}

void RendererGL::renderMeshes3D(const std::vector<Mesh3D*>& meshes3D) const {
	assert(camera);
	if (!meshes3D.size())
		return;
	for (Mesh3D* mesh : meshes3D) {
		mesh->getShader()->bind();
		//material uniforms
		mesh->material->setShaderUniforms(mesh->getShader());
		// Uniforms
		static Mat4 identity = Mat4().initIdentity();
		mesh->getShader()->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		mesh->getShader()->setUniformMatrix4f("view", camera->getViewMatrix());
		if (mesh->transform) {
			mesh->getShader()->setUniformMatrix4f("transform", *mesh->transform->getTransformation());
		}else
			mesh->getShader()->setUniformMatrix4f("transform", identity);

		mesh->fullModel->render();
	}
}

void RendererGL::renderLightedMeshes3D(const std::vector<Mesh3D*>& meshes3D, const LightingDescription& lights) const{
	assert(camera);
	if(!meshes3D.size())
		return;
	for(Mesh3D* mesh : meshes3D){
		mesh->getShader()->bind();
		// material uniforms
		mesh->material->setShaderUniforms(mesh->getShader());
		// Camera uniforms
		static Mat4 identity = Mat4().initIdentity();
		mesh->getShader()->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		mesh->getShader()->setUniformMatrix4f("view", camera->getViewMatrix());
		mesh->getShader()->setUniform3f("cameraPos", camera->getPosition());
		// LIGHTING uniforms
		/*
		// TEMP Test directional light
		mesh->shader->setUniform3f("dLight.direction", Vec3(1,1,-1).normalize());
		mesh->shader->setUniform3f("dLight.color", Vec3(1,0,1));
		mesh->shader->setUniform1f("dLight.intensity", 1.0f);
		mesh->shader->setUniform3f("cameraPos", camera->getPosition());
		*/
		// TEMP Point light uniform test
		//mesh->shader->setUniform3f("pLight.position", Vec3(0.5,1.5,4.5));
		//mesh->shader->setUniform3f("pLight.color", Vec3(1,1,0));
		//mesh->shader->setUniform1f("pLight.intensity", 1.0f);
		//mesh->shader->setUniform1f("pLight.constant", 1.0f);
		//mesh->shader->setUniform1f("pLight.linear", 0.1f);
		//mesh->shader->setUniform1f("pLight.exponent", 0.01f);

		assert(lights.directionalLights.size() <= 4);
		int index = 0;
		for(DirectionalLight* light : lights.directionalLights){
			mesh->getShader()->setUniform3f("dlights[" + std::to_string(index) + "].direction", light->direction);
			mesh->getShader()->setUniform3f("dlights[" + std::to_string(index) + "].color", light->color);
			mesh->getShader()->setUniform1f("dlights[" + std::to_string(index) + "].intensity", light->intensity);
			++index;
		}

		assert(lights.pointLights.size() <= 4);
		index = 0;
		for(PointLight* light : lights.pointLights){
			mesh->getShader()->setUniform3f("plights[" + std::to_string(index) + "].position", light->position);
			mesh->getShader()->setUniform3f("plights[" + std::to_string(index) + "].color", light->color);
			mesh->getShader()->setUniform1f("plights[" + std::to_string(index) + "].constant", light->attenuation.constant);
			mesh->getShader()->setUniform1f("plights[" + std::to_string(index) + "].linear", light->attenuation.linear);
			mesh->getShader()->setUniform1f("plights[" + std::to_string(index) + "].exponent", light->attenuation.exponent);
			++index;
		}

		assert(lights.spotLights.size() <= 4);
		index = 0;
		for(SpotLight* light : lights.spotLights){
			mesh->getShader()->setUniform3f("slights[" + std::to_string(index) + "].pointlight.position", light->pointlight.position);
			mesh->getShader()->setUniform3f("slights[" + std::to_string(index) + "].pointlight.color", light->pointlight.color);
			mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].pointlight.constant", light->pointlight.attenuation.constant);
			mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].pointlight.linear", light->pointlight.attenuation.linear);
			mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].pointlight.exponent", light->pointlight.attenuation.exponent);
			mesh->getShader()->setUniform3f("slights[" + std::to_string(index) + "].direction", light->direction);
			mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].cutoff", light->cutoff);
		}
		
		// TRANSFORM uniform
		if (mesh->transform) {
			mesh->getShader()->setUniformMatrix4f("transform", *mesh->transform->getTransformation());
		}else
			mesh->getShader()->setUniformMatrix4f("transform", identity);

		// Render call for geometry
		mesh->fullModel->render();
	}
}

void RendererGL::renderDeferredLightedMeshes3D(const std::vector<Mesh3D *> &meshes3D, const LightingDescription &lights) const {
	// GPass
	glDisable(GL_BLEND);
	gBuffer->bind();
	for(Mesh3D* mesh : meshes3D){
		Shader* gShader = mesh->getGShader();
		
		gShader->bind();
		gShader->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		gShader->setUniformMatrix4f("view", camera->getViewMatrix());
		if(mesh->transform)
			gShader->setUniformMatrix4f("transform", *mesh->transform->getTransformation());
		else
			gShader->setUniformMatrix4f("transform", Mat4().initIdentity());
		mesh->material->setShaderUniforms(gShader);
		gShader->prepareUniforms();
		mesh->fullModel->render();
	}
	// Copy depth gbuffer to main buffer
	gBuffer->copyDepth();
	gBuffer->unbind();

	// Additive Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Deferred pass
	deferredShader->bind();
	// Set texture uniforms
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getNormalTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getAlbedoTexture());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getDepthTexture());
	deferredShader->setUniform1i("gDepth", 0);
	deferredShader->setUniform1i("gAlbedo", 1);
	deferredShader->setUniform1i("gNormal", 2);

	// Render ambient brightness
	deferredShader->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		deferredShader->setUniformMatrix4f("view", camera->getViewMatrix());

	for(DirectionalLight* dlight : lights.directionalLights){
		
	}
	for(PointLight* plight : lights.pointLights){
		deferredShader->setUniform3f("lightPos", plight->position);
		deferredShader->setUniform3f("lightColor", plight->color);
		deferredShader->setUniform1f("constant", plight->attenuation.constant);
		deferredShader->setUniform1f("linear", plight->attenuation.linear);
		deferredShader->setUniform1f("exponent", plight->attenuation.exponent);
		glDisable(GL_DEPTH_TEST);
		
		float radius = plight->radius;
		//renderLightBox(plight->position, radius);
		deferredShader->bind();
		
		deferredShader->setUniform3f("position", plight->position);
		deferredShader->setUniform1f("radius", plight->radius);

		// Viewport uniform for testing
		deferredShader->setUniform2f("viewport", Vec2(1600,1200));

		deferredShader->prepareUniforms();
		
		// Shameless rendering of an AABB
		glBindVertexArray(vaoIDcube3D);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	for(SpotLight* slight : lights.spotLights){
		
	}

	glEnable(GL_DEPTH_TEST);

	// Normal Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RendererGL::renderLightBox(Vec3 position, float radius) const{
	shaderAABB->bind();
	shaderAABB->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderAABB->setUniformMatrix4f("view", camera->getViewMatrix());
	shaderAABB->setUniform3f("position", position);
	shaderAABB->setUniform3f("extents", Vec3(radius));
	shaderAABB->setUniform3f("color", Vec3(1,0,1));
	
	glBindVertexArray(vaoIDcube3D);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderAnimatedMeshes3D(const std::vector<Mesh3D*>& meshes3D) const {
	assert(camera);
	if (!meshes3D.size())
		return;
	for (Mesh3D* mesh : meshes3D) {
		mesh->getShader()->bind();
		//material uniforms
		mesh->material->setShaderUniforms(mesh->getShader());
		// Uniforms
		static Mat4 identity = Mat4().initIdentity();
		mesh->getShader()->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		mesh->getShader()->setUniformMatrix4f("view", camera->getViewMatrix());
		if (mesh->transform) {
			mesh->getShader()->setUniformMatrix4f("transform", *mesh->transform->getTransformation());
		}else
			mesh->getShader()->setUniformMatrix4f("transform", identity);

		// Animation bones
		std::vector<Mat4> boneTransforms;
		boneTransforms.resize(32);

		animation->getBoneTransforms();
		
		for(int i = 0; i < 32; ++i)
			mesh->getShader()->setUniformMatrix4f("boneTransforms[" + std::to_string(i) + "]", identity);
		
		mesh->fullModel->render();
	}
}




// v0.2 Interface methods

void RendererGL::renderMeshes3DForward(const std::vector<Mesh3D*>& meshes3D, const LightingDescription& lights, Animation* animation) const {
	assert(camera);
	for(Mesh3D* mesh : meshes3D){
		mesh->getShader()->bind();
		
		// Basic transformation uniforms
		mesh->getShader()->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		mesh->getShader()->setUniformMatrix4f("view", camera->getViewMatrix());
		static Mat4 identity = Mat4().initIdentity();
		if(mesh->transform)
			mesh->getShader()->setUniformMatrix4f("transform", mesh->transform->getTransformation());
		else
			mesh->getShader()->setUniformMatrix4f("transform", identity);
		
		// Material uniforms - textures, colors, etc...
		mesh->material->setShaderUniforms(mesh->getShader());
		
		bool lighted = mesh->getShader()->lighted;
		if(lighted)
			setLightUniforms(mesh->getShader(), lights);
		
		bool animated = mesh->getShader()->animated && mesh->skeleton && animation;
		if(animated)
			setAnimationUniforms(mesh->getShader(), animation, mesh->skeleton);
		
		mesh->getShader()->prepareUniforms();
		
		// Full mesh rendering
		mesh->fullModel->render();
	}
}

void RendererGL::setLightUniformsForward(Shader *shader, const LightingDescription &lights){
	// Send eye position
	shader->setUniform3f("cameraPos", camera->position);

	// Send light uniforms
	assert(lights.directionalLights.size() <= 4);
	int index = 0;
	for(DirectionalLight* light : lights.directionalLights){
		mesh->getShader()->setUniform3f("dlights[" + std::to_string(index) + "].direction", light->direction);
		mesh->getShader()->setUniform3f("dlights[" + std::to_string(index) + "].color", light->color);
		mesh->getShader()->setUniform1f("dlights[" + std::to_string(index) + "].intensity", light->intensity);
		++index;
	}
	
	assert(lights.pointLights.size() <= 4);
	index = 0;
	for(PointLight* light : lights.pointLights){
		mesh->getShader()->setUniform3f("plights[" + std::to_string(index) + "].position", light->position);
		mesh->getShader()->setUniform3f("plights[" + std::to_string(index) + "].color", light->color);
		mesh->getShader()->setUniform1f("plights[" + std::to_string(index) + "].constant", light->attenuation.constant);
		mesh->getShader()->setUniform1f("plights[" + std::to_string(index) + "].linear", light->attenuation.linear);
		mesh->getShader()->setUniform1f("plights[" + std::to_string(index) + "].exponent", light->attenuation.exponent);
		++index;
	}
	
	assert(lights.spotLights.size() <= 4);
	index = 0;
	for(SpotLight* light : lights.spotLights){
		mesh->getShader()->setUniform3f("slights[" + std::to_string(index) + "].pointlight.position", light->pointlight.position);
		mesh->getShader()->setUniform3f("slights[" + std::to_string(index) + "].pointlight.color", light->pointlight.color);
		mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].pointlight.constant", light->pointlight.attenuation.constant);
		mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].pointlight.linear", light->pointlight.attenuation.linear);
		mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].pointlight.exponent", light->pointlight.attenuation.exponent);
		mesh->getShader()->setUniform3f("slights[" + std::to_string(index) + "].direction", light->direction);
		mesh->getShader()->setUniform1f("slights[" + std::to_string(index) + "].cutoff", light->cutoff);
	}
}

void RendererGL::renderMeshes3DDeferred(const std::vector<Mesh3D *> &meshes3D, const LightingDescription &lights, Animation* animation) const {
	assert(camera);

	// GPass
	glDisable(GL_BLEND);
	gBuffer->bind();
	for(Mesh3D* mesh : meshes3D){
		mesh->getShader()->bind();

		// Camera and transformation uniforms
		mesh->getShader()->setUniformMatrix4f("projection", camera->getProjectionMatrix());
		mesh->getShader()->setUniformMatrix4f("view", camera->getViewMatrix());
		static Mat4 identity = Mat4().initIdentity();
		if(mesh->transform)
			mesh->getShader()->setUniformMatrix4f("transform", mesh->transform->getTransformation());
		else
			mesh->getShader()->setUniformMatrix4f("transform", identity);

		// material uniforms
		mesh->material->setShaderUniforms(mesh->getShader());

		// Animation uniforms
		bool animated = mesh->getShader()->animated && mesh->skeleton && animation;
		if(animated)
			setAnimationUniforms(mesh->getShader(), animation, skeleton);
		
		mesh->getShader()->prepareUniforms();

		mesh->fullModel->render();
	}
	gBuffer->unbind();
	// Additive blending for deferred pass
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Deferred pass
	deferredShader->bind();

	// Gbuffer textures
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getNormalTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getAlbedoTexture());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getDepthTexture());
	deferredShader->setUniform1i("gDepth", 0);
	deferredShader->setUniform1i("gAlbedo", 1);
	deferredShader->setUniform1i("gNormal", 2);
	
	// Camera uniforms
	deferredShader->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	deferredShader->setUniformMatrix4f("view", camera->getViewMatrix());

	// Render light volumes
	for(DirectionalLight* dlight : lights.directionalLights){
		
	}
	for(PointLight* plight : lights.pointLights){
		deferredShader->setUniform3f("lightPos", plight->position);
		deferredShader->setUniform3f("lightColor", plight->color);
		deferredShader->setUniform1f("constant", plight->attenuation.constant);
		deferredShader->setUniform1f("linear", plight->attenuation.linear);
		deferredShader->setUniform1f("exponent", plight->attenuation.exponent);
		glDisable(GL_DEPTH_TEST);
		
		float radius = plight->radius;
		//renderLightBox(plight->position, radius);
		deferredShader->bind();
		
		deferredShader->setUniform3f("position", plight->position);
		deferredShader->setUniform1f("radius", plight->radius);

		// Viewport uniform for testing
		deferredShader->setUniform2f("viewport", Vec2(1600,1200));

		deferredShader->prepareUniforms();
		
		// Shameless rendering of an AABB
		glBindVertexArray(vaoIDcube3D);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	for(SpotLight* slight : lights.spotLights){
		
	}
	
	// Back to normal blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RendererGL::setAnimationUniforms(Shader *shader, Animation *animation, Skeleton *skeleton){
	static int MAX_BONES = 64;
	assert(skeleton->bones.size() <= MAX_BONES);

	std::vector<Mat4> boneTransforms;
	boneTransforms.resize(skeleton->bones.size());
	animation->updateBones(skeleton, boneTransforms, 0.0f);

	for(int i = 0; i < skeleton->bones.size(); ++i)
		shader->setUniformMatrix4f("boneTransforms[" + std::to_string(i) + "]", boneTransforms[i]);
}







void RendererGL::renderAABBs(const std::vector<Mesh3D*>& aabbs) const {
	shaderAABB->bind();
	shaderAABB->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderAABB->setUniformMatrix4f("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDcube3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : aabbs) {
		AABB* aabb = (AABB*)mesh->physics;
		shaderAABB->setUniform3f("position", aabb->position);
		shaderAABB->setUniform3f("extents", aabb->extents);
		mesh->material->setShaderUniforms(shaderAABB);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderSpheres(const std::vector<Mesh3D*>& spheres) const {
	shaderSphere->bind();
	shaderSphere->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderSphere->setUniformMatrix4f("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDsphere3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : spheres) {
		Sphere* sphere = (Sphere*)mesh->physics;
		shaderSphere->setUniform3f("position", sphere->position);
		shaderSphere->setUniform1f("radius", sphere->radius);
		mesh->material->setShaderUniforms(shaderSphere);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDsphere3D);
		glDrawElements(GL_TRIANGLES, numIndicesSphere, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderPlanes(const std::vector<Mesh3D*>& planes) const {
	shaderPlane->bind();
	shaderPlane->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderPlane->setUniformMatrix4f("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDplane3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : planes) {
		if (mesh->material->getTexture()) {
			glEnableVertexAttribArray(1);
			shaderPlane->setUniform1f("isTextured", 1.0);
		}
		else {
			shaderPlane->setUniform1f("isTextured", 0.0);
		}

		Plane* plane = (Plane*)mesh->physics;
		// Calculate rotation matrix
		Vec3 u = Vec3(0, 0, 1);
		if (u.equals(plane->normal))
			u = Vec3(1, 0, 0);
		Vec3 v1 = cross(plane->normal, u).normalize();
		Vec3 v2 = cross(v1, plane->normal).normalize();

		//Mat4 rotation = Mat4().initRotationTargetNormal(Vec3(0,0,1), plane->normal);
		Vec3 bitangent = -cross(plane->tangent, plane->normal);
		Mat4 rotation = Mat4().initSpecialRotation2(plane->tangent, bitangent, plane->normal);
		//
		shaderPlane->setUniformMatrix4f("rotation", rotation);
		shaderPlane->setUniform3f("position", plane->position);
		shaderPlane->setUniform1f("scale", plane->renderingScale);
		mesh->material->setShaderUniforms(shaderPlane);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (mesh->material->getTexture())
			glDisableVertexAttribArray(1);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderOBBs(const std::vector<Mesh3D*>& obbs) const {
	shaderOBB->bind();
	shaderOBB->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderOBB->setUniformMatrix4f("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDcube3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : obbs) {
		OBB* obb = (OBB*)mesh->physics;
		Vec3 u2 = cross(obb->u[0], obb->u[1]);
		Mat4 translation = Mat4().initTranslation(obb->position);
		Mat4 rotation = Mat4().initSpecialRotation2(obb->u[0], obb->u[1], u2);
		Mat4 scale = Mat4().initScale(obb->e);
		Mat4 transform = translation.mul(rotation.mul(scale));
		shaderOBB->setUniformMatrix4f("transform", transform);

		mesh->material->setShaderUniforms(shaderOBB);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderRays(const std::vector<Mesh3D*>& rays) const {
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix4f("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : rays) {
		Ray* ray = (Ray*)mesh->physics;
		shaderLine3D->setUniform3f("start", ray->position);
		shaderLine3D->setUniform3f("end", ray->position + ray->direction * 10000.0f);

		mesh->material->setShaderUniforms(shaderLine3D);

		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void RendererGL::renderLines(const std::vector<Mesh3D*>& lines) const {
	shaderLine3D->bind();
	shaderLine3D->setUniformMatrix4f("projection", camera->getProjectionMatrix());
	shaderLine3D->setUniformMatrix4f("view", camera->getViewMatrix());

	glBindVertexArray(vaoIDline3D);
	glEnableVertexAttribArray(0);
	for (Mesh3D* mesh : lines) {
		Line* ray = (Line*)mesh->physics;
		shaderLine3D->setUniform3f("start", ray->start);
		shaderLine3D->setUniform3f("end", ray->end);

		mesh->material->setShaderUniforms(shaderLine3D);

		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

bool RendererGL::initializeGeometries() {
	// Point2D initialization
	{
		const float sizePoint = 0.025;
		float ar = getStaticWindowHeight() / getStaticWindowWidth();
		float offset = sizePoint / 2.0f;
		float verticesPoints2D[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,sizePoint - offset,(sizePoint - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };

		glGenVertexArrays(1, &vaoIDpoint2D);
		glBindVertexArray(vaoIDpoint2D);

		glGenBuffers(1, &vboIDpoint2D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDpoint2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPoints2D), verticesPoints2D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	}

	// Line2D initialization
	{
		float verticesLines2D[4] = { 0,0,1,1 };

		glGenVertexArrays(1, &vaoIDline2D);
		glBindVertexArray(vaoIDline2D);

		glGenBuffers(1, &vboIDline2D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDline2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLines2D), verticesLines2D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	}

	// Sprite2D initialization
	{
		const float sizeSprite = 0.5f;
		float ar = getStaticWindowHeight() / getStaticWindowWidth();
		float offset = sizeSprite / 2.0f;
		float verticesSprites2D[12] = { (0 - offset)*ar,0 - offset,(0 - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,sizeSprite - offset,(sizeSprite - offset)*ar,0 - offset,(0 - offset)*ar,0 - offset };
		float texCoordsSprites2D[12] = {0,0,0,1,1,1,1,1,1,0,0,0};

		glGenVertexArrays(1, &vaoIDsprite2D);
		glBindVertexArray(vaoIDsprite2D);

		glGenBuffers(1, &vboIDsprite2D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDsprite2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSprites2D), verticesSprites2D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &vbotIDsprite2D);
		glBindBuffer(GL_ARRAY_BUFFER, vbotIDsprite2D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordsSprites2D), texCoordsSprites2D, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	}

	// Point3D initialization
	{
		const float pointSize = 0.25f;
		float verticesPoints3D[24] = { -pointSize, pointSize, pointSize,
									    pointSize, pointSize, pointSize,
										pointSize, -pointSize, pointSize,
									   -pointSize, -pointSize, pointSize,
									   -pointSize, pointSize, -pointSize,
										pointSize, pointSize, -pointSize,
										pointSize, -pointSize, -pointSize,
									   -pointSize, -pointSize, -pointSize};
		int indicesPoints3D[36] = {	0,1,2,2,3,0,
									1,5,6,6,2,1,
									4,5,1,1,0,4,
									4,0,3,3,7,4,
									6,7,3,3,2,6,
									5,4,7,7,6,5};

		glGenVertexArrays(1, &vaoIDpoint3D);
		glBindVertexArray(vaoIDpoint3D);

		glGenBuffers(1, &vboIDpoint3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDpoint3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPoints3D), verticesPoints3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &iboIDpoint3D);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDpoint3D);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPoints3D), indicesPoints3D, GL_STATIC_DRAW);
	}

	// Cube3D initialization
	{
		const float pointSize = 0.5; // 2x2x2 cube
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

		glGenVertexArrays(1, &vaoIDcube3D);
		glBindVertexArray(vaoIDcube3D);

		glGenBuffers(1, &vboIDcube3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDcube3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubes3D), verticesCubes3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &iboIDcube3D);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDcube3D);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesCubes3D), indicesCubes3D, GL_STATIC_DRAW);
	}

	// Line3D initialization
	{
		float verticesLines3D[] = { 0,0,0,1,1,1 };

		glGenVertexArrays(1, &vaoIDline3D);
		glBindVertexArray(vaoIDline3D);

		glGenBuffers(1, &vboIDline3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDline3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLines3D), verticesLines3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	}

	// Sprite3D initialization
	{

	}

	// Sphere 3D initialization
	{
		float stackCount = 20;
		float sliceCount = 20;
		float radius = 1;

		std::vector<Vec3> vertices;
		std::vector<int> indices;
		std::vector<Vec2> texCoords;
		std::vector<Vec3> normals;
		std::vector<Vec3> tangents;
		vertices.push_back(Vec3(1, 0, 0));
		normals.push_back(Vec3(0, 1, 0));
		tangents.push_back(Vec3(0, radius, 0));
		texCoords.push_back(Vec2(0, 0));

		float phiStep = PI / stackCount;
		float thetaStep = 2.0f*PI / sliceCount;

		for (int i = 1; i <= stackCount - 1; i++) {
			float phi = i*phiStep;
			for (int j = 0; j <= sliceCount; j++) {
				float theta = j*thetaStep;
				Vec3 t = Vec3(
					(radius*sin(phi)*cos(theta)),
					(radius*cos(phi)),
					(radius*sin(phi)*sin(theta))
				);

				Vec3 p = Vec3(-radius*sin(phi)*sin(theta), 0, radius*sin(phi)*cos(theta));
				t.normalize();
				Vec3 n = p;
				n.normalize();
				Vec2 uv = Vec2(theta / (PI * 2), phi / PI);
				vertices.push_back(t), normals.push_back(n), tangents.push_back(p), texCoords.push_back(uv);
			}
		}

		vertices.push_back(Vec3(1, 0, 0));
		normals.push_back(Vec3(0, -1, 0));
		tangents.push_back(Vec3(0, -radius, 0));
		texCoords.push_back(Vec2(0, 1));


		for (int i = 1; i <= sliceCount; i++) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(0);
		}
		float baseIndex = 1;
		float ringVertexCount = sliceCount + 1;
		for (int i = 0; i < stackCount - 2; i++) {
			for (int j = 0; j < sliceCount; j++) {
				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
				indices.push_back(baseIndex + i*ringVertexCount + j + 1);
				indices.push_back(baseIndex + i*ringVertexCount + j);

				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
				indices.push_back(baseIndex + i*ringVertexCount + j + 1);
				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			}
		}
		float southPoleIndex = vertices.size() - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (int i = 0; i < sliceCount; i++) {
			indices.push_back(baseIndex + i + 1);
			indices.push_back(baseIndex + i);
			indices.push_back(southPoleIndex);
		}

		numIndicesSphere = indices.size();

		glGenVertexArrays(1, &vaoIDsphere3D);
		glBindVertexArray(vaoIDsphere3D);

		glGenBuffers(1, &vboIDsphere3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDsphere3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &vbotIDsphere3D);
		glBindBuffer(GL_ARRAY_BUFFER, vbotIDsphere3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords.size() * 2, &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &iboIDsphere3D);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIDsphere3D);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Plane 3D initialization
	{
		float verticesPlanes3D[18] = { -0.5f,-0.5f,0,-0.5f,0.5f,0,0.5f,0.5f,0,0.5f,0.5f,0,0.5f,-0.5f,0,-0.5f,-0.5f,0 };
		float texCoordsPlanes3D[12] = { 0,0,0,1,1,1,1,1,1,0,0,0 };

		glGenVertexArrays(1, &vaoIDplane3D);
		glBindVertexArray(vaoIDplane3D);

		glGenBuffers(1, &vboIDplane3D);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDplane3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlanes3D), verticesPlanes3D, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glGenBuffers(1, &vbotIDplane3D);
		glBindBuffer(GL_ARRAY_BUFFER, vbotIDplane3D);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordsPlanes3D), texCoordsPlanes3D, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	}

	// Unbind buffers
	glBindVertexArray(0);

	return true;
}

bool RendererGL::initializeShaders() {
	ResourceManager* resManager = getStaticResourceManager();
	shaderPoint2D = resManager->getShader("renderer/shader2Dpoint");
	shaderLine2D = resManager->getShader("renderer/shader2Dline");
	shaderSprite2D = resManager->getShader("renderer/shader2Dsprite");

	shaderPoint3D = resManager->getShader("renderer/shader3Dpoint");
	shaderLine3D = resManager->getShader("renderer/shader3Dline");
	//shaderSprite3D = ShaderLoader::loadShaderGL("shaders/GLSL/renderer/shader3Dsprite");

	shaderAABB = resManager->getShader("physics/shader3DAABB");
	shaderSphere = resManager->getShader("physics/shader3Dsphere");
	shaderPlane = resManager->getShader("physics/shader3Dplane");
	shaderOBB = resManager->getShader("physics/shader3DOBB");

	deferredShader = resManager->getShader("deferred/deferred");

	return true;
}

bool RendererGL::initializeFramebuffers(){
	gBuffer = new GBuffer();
	
	return true;
}

bool RendererGL::cleanUpFramebuffers(){
	if(gBuffer)
		delete gBuffer;

	return true;
}

bool RendererGL::cleanUpGeometries() {
	// TODO: destroy OpenGL buffers

	return true;
}

bool RendererGL::cleanUpShaders() {
	delete shaderPoint2D;
	delete shaderLine2D;
	delete shaderSprite2D;

	return true;
}

#endif
