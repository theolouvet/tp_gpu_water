#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 MVP;
uniform float test;
uniform sampler2D textureSampler;
uniform sampler2D textureHeight;
uniform sampler2D texturefromfbo;
in vec3 pos[];
in vec2 uv[];
in vec3 n[];
out vec3 normal;
vec3 deltapos1;
vec3 deltapos2;
vec3 deltauv1;
vec3 deltauv2;
float r;
vec3 tangent;
vec3 bitangent;
mat3 tbn;
mat4 tbn_4;



void main(void)
{

deltapos1 = pos[1] - pos[0];
deltapos2 = pos[2] - pos[0];
deltauv1 = uv[1] - uv[0];
deltauv2 = uv[2] - uv[0];
r = 1.0f / (deltauv1.x * deltauv2.y - deltauv1.y * deltauv2.x); 
tangent = (deltapos1 * deltauv2.y   - deltapos2 * deltauv1.y)*r; 
bitangent = (deltapos2 * deltauv1.x   - deltapos1 * deltauv2.x)*r;

tbn = transpose(mat3(
    normalize(tangent),
    normalize(bitangent),
    normalize(n[0])
));
tbn_4 = mat4(tbn);
for (int i = 0; i < 3; i++)
	{
    tbn = transpose(mat3(
        normalize(tangent),
        normalize(bitangent),
        normalize(n[i])
        ));
        tbn_4 = mat4(tbn);
	  gl_Position = 0* MVP * vec4(pos[i].xy,2.0 , 1.0);
	  EmitVertex();
	}
	EndPrimitive();
}