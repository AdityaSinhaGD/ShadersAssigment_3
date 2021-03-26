#version 430

uniform mat4 modelMat; 
uniform mat4 viewMat;
uniform mat4 projMat; 

uniform vec3 eyePos;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

out vec3 worldNorm;
out vec3 worldPos;



void main() {
  
  vec3 worldNormal = (modelMat*vec4(vertex_normal,0.0)).xyz;
  worldNormal = normalize(worldNormal);
  vec3 worldPosition = (modelMat*vec4(vertex_position, 1.0f)).xyz;

  worldNorm = worldNormal;
  worldPos = worldPosition;

  gl_Position = projMat * viewMat * modelMat * vec4(vertex_position, 1.0f);
}