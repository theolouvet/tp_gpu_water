#version 330 core

in vec2 uv;
in vec3 n;

out vec4 color;

uniform sampler2D textureSampler;

void main()
{
    color = texture( textureSampler, uv );
};
