#version 430

uniform mat4 modelMat; 
uniform mat4 viewMat;
uniform mat4 projMat; 
uniform vec3 eyePos;

struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int coeff;
};

uniform PointLight pointLights[2];

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

out vec3 color;

vec3 CalculatePhongValue(PointLight light, vec3 worldNormal, vec3 worldPosition){
	vec3 lightDir = normalize(light.position - worldPosition);
	float diff = max(dot(worldNormal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, worldNormal);
	vec3 viewDir = normalize(eyePos - worldPosition);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),light.coeff);

	vec3 ambient = light.ambient;
	vec3 specular = light.specular*spec;
	vec3 diffuse = light.diffuse*diff;

	return (ambient+specular+diffuse)*0.5f;
}

void main() {
  
  vec3 worldNormal = (modelMat*vec4(vertex_normal,0.0)).xyz;
  worldNormal = normalize(worldNormal);
  vec3 worldPosition = (modelMat*vec4(vertex_position, 1.0f)).xyz;

  vec3 result = vec3(0.0,0.0,0.0);

  for(int i=0;i<2;i++)
  {
	result += CalculatePhongValue(pointLights[i], worldNormal, worldPosition);
  }
  color = result;
  
  gl_Position = projMat * viewMat * modelMat * vec4(vertex_position, 1.0f);
}