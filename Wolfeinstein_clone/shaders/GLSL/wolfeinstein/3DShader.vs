#version 400 core
//game
//wolfenstein

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 texCoord0;
out vec4 worldPos0;
out vec3 surfaceNormal;
out vec3 toLightVector;
out vec3 toCameraVector;
out float visibility;

uniform mat4 scale;
uniform mat4 translation;
uniform vec4 rotation;
uniform mat4 rotationMatrix;
uniform vec3 centerPoint;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 directional;

uniform float useFakeLighting;

const float density = 0.0035;
const float gradient = 5.0;

uniform vec4 plane;
//uniform float clipForWater;
//const vec4 plane = vec4(0, -1, 0, 5);

vec4 inverse(vec4 quat){
	return vec4(-quat.x, -quat.y, -quat.z, quat.w);

}

vec4 multiplication(vec4 quat1, vec4 quat2){
	float w_ = quat1.w * quat2.w - quat1.x * quat2.x - quat1.y * quat2.y - quat1.z * quat2.z;
	float x_ = quat1.x * quat2.w + quat1.w * quat2.x + quat1.y * quat2.z - quat1.z * quat2.y;
	float y_ = quat1.y * quat2.w + quat1.w * quat2.y + quat1.z * quat2.x - quat1.x * quat2.z;
	float z_ = quat1.z * quat2.w + quat1.w * quat2.z + quat1.x * quat2.y - quat1.y * quat2.x;
	
	return vec4(x_, y_, z_, w_);
		
}

void main(){
	//Scaling
	vec4 positionPrime = vec4(position, 1.0) * scale;
	
	//Rotation
	positionPrime = vec4(positionPrime.x - centerPoint.x, 
					 positionPrime.y - centerPoint.y, 
					 positionPrime.z - centerPoint.z, 0.0);
	
	vec4 qInverse = inverse(rotation);
	vec4 qp = multiplication(positionPrime, qInverse);
	vec4 qpq = multiplication(rotation, qp);
	
	//Translation
	vec4 worldPos = translation * vec4(qpq.x + centerPoint.x, 
									   qpq.y + centerPoint.y, 
									   qpq.z + centerPoint.z, 
									   1.0);
	
	gl_ClipDistance[0] = dot(worldPos, plane);
	
	vec4 positionRelativeToCamera = view * worldPos;

	gl_Position = projection * positionRelativeToCamera;
	texCoord0 = texCoord;
	
	vec3 actualNormal = normal;
	if(useFakeLighting > 0.5){
		actualNormal = vec3(0.0, 1.0, 0.0);
	
	}
	
	//Rotation						
	vec4 qn = multiplication(vec4(actualNormal, 0.0), qInverse);
	vec4 qnq = multiplication(rotation, qn);
	
	surfaceNormal = (vec4(qnq.x, qnq.y, qnq.z, 1.0)).xyz;
	
	toLightVector = directional;
	toCameraVector = normalize((inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz);
	
	float distance = length(positionRelativeToCamera.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
	
	worldPos0 = worldPos;

}