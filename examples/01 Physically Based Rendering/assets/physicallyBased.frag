#version 450 core

layout (location = 3) uniform vec3 eyePosition;

in VS_OUT
{
  vec3 position;
	vec3 normal;
	vec2 textureCoord;
} fs_in;

out vec4 color;

//layout (binding = 1) uniform sampler2D sampler;
layout (binding = 2) uniform sampler2D sampler2;
layout (binding = 3) uniform samplerCube sky; 

const float PI = 3.141592653589;
const int NUM_SAMPLES = 1;

vec3 fresnel(vec3 f0, float LdotH)
{
  //Schlick approximation
  return f0 + (1.0 - f0) * pow(1.0 - LdotH, 5);
}

float distribution(float NdotH, float roughness)
{
  // GGX/Trowbridge-Reitz
  float a2 = roughness * roughness;
  float b = NdotH * NdotH * (a2 - 1.0) + 1.0;
  return a2 / (PI * b * b);
}

float shadowingMasking(float NdotV, float NdotL, float roughness)
{
  float r2 = roughness * roughness;
  float l = tan(acos(NdotL));
  float v = tan(acos(NdotV));
  float shadowing = (-1.0 + sqrt(r2 * l * l + 1.0)) / 2.0;
  float masking = (-1.0 + sqrt(r2 * v * v + 1.0)) / 2.0;
  return 1.0 / (1.0 + shadowing + masking);
}

void main()
{
  vec4 albedo = texture(sampler2, fs_in.textureCoord);//vec4(1.0, 0.6, 0.9, 1.0) / PI;
  
  float roughness = 0.6;
  vec3 n = normalize(fs_in.normal);
  vec3 v = normalize(eyePosition - fs_in.position);
  
  vec4 specular = vec4(0);
  
  vec3 l = reflect(-v, fs_in.normal);
  vec4 radiance = texture(sky, l);
  vec3 h = normalize(v + l);

  float LdotH = clamp(dot(l, h), 0.0, 1.0);
  float NdotH = clamp(dot(n, h), 0.0, 1.0);
  float NdotL = clamp(dot(n, l), 0.0, 1.0);
  float NdotV = clamp(dot(n, v), 0.0, 1.0);

  // (F * G * D) / 4 * NdotL * NdotV 
  vec3 F = fresnel(vec3(0.7), LdotH);
  float G = shadowingMasking(NdotV, NdotL, roughness);
  float D = distribution(NdotH, roughness);
  
	color = albedo + radiance * vec4((F * G * D) / (4.0 * NdotV * NdotL), 1.0);
}