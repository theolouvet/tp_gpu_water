#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
uniform sampler2D textureSampler;
out vec3 v_pos;
out vec3 n_pos;
out vec2 uv_pos;

uniform mat4 MVP;
vec3 p;


float y = texture(textureSampler, texcoord).r * 2.f;
void main()
{
    gl_Position = MVP * vec4(pos.x, pos.y, pos.z, 1.0);
    v_pos = pos;
    n_pos = normal;
    uv_pos = texcoord;
};
