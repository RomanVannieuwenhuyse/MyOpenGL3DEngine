#version 330 core
layout (location = 0) in vec3 aPos;

uniform float NearClippingPlane, FarClippingPlane, FOV, AspectRatio;
uniform vec3 CPosition, CRotation;

vec3 CRcos = cos(CRotation);
vec3 CRsin = sin(CRotation);

vec4 c1 = vec4(1/(AspectRatio*tan(FOV)), 0.0, 0.0, 0.0);
vec4 c2 = vec4(0.0, 1/tan(FOV), 0.0, 0.0);
vec4 c3 = vec4(0.0, 0.0, (FarClippingPlane + NearClippingPlane)/(FarClippingPlane - NearClippingPlane), 1.0);
vec4 c4 = vec4(0.0, 0.0, (-2*FarClippingPlane*NearClippingPlane)/(FarClippingPlane - NearClippingPlane), 0.0);
mat4x4 WorldToCameraMat = mat4(c1, c2, c3, c4);

void main()
{
	vec3 vertexPos = aPos.xyz;
	vertexPos -= CPosition;
	//i = x, j = y, k = z
	vertexPos = vec3(vertexPos.x, CRcos.x*CRcos.x*vertexPos.y + 2*CRsin.x*CRcos.x*vertexPos.z - CRsin.x*CRsin.x*vertexPos.y, CRcos.x*CRcos.x*vertexPos.z - 2*CRsin.x*CRcos.x*vertexPos.y - CRsin.x*CRsin.x*vertexPos.z);
	vertexPos = vec3(CRcos.y*CRcos.y*vertexPos.x - 2*CRsin.y*CRcos.y*vertexPos.z - CRsin.y*CRsin.y*vertexPos.x, vertexPos.y, CRcos.y*CRcos.y*vertexPos.z + 2*CRsin.y*CRcos.y*vertexPos.x - CRsin.y*CRsin.y*vertexPos.z);
	vertexPos = vec3(CRcos.z*CRcos.z*vertexPos.x + 2*CRsin.z*CRcos.z*vertexPos.y - CRsin.z*CRsin.z*vertexPos.x, CRcos.z*CRcos.z*vertexPos.y - 2*CRsin.z*CRcos.z*vertexPos.x - CRsin.z*CRsin.z*vertexPos.y, vertexPos.z);
	gl_Position = WorldToCameraMat * vec4(vertexPos.xyz, 1.0);
}