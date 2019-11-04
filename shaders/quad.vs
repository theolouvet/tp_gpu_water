#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
uniform sampler2D textureSampler;

out vec3 pos;
out vec2 uv;
out vec3 n;

vec3 p;

uniform mat4 MVP;
float y = texture(textureSampler, texcoord).r * 2.f;

void main()
{
  gl_Position = MVP*vec4(position , 1.0);
 
  pos = position;
  uv = texcoord;
  n = normal;
  
};
