#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 MVP;

in vec3 v_pos[];
in vec3 n_pos[];
in vec2 uv_pos[];

out vec2 uv;
out vec3 n;
out vec3 pos; 

vec3 deltapos1;
vec3 deltapos2;
vec3 deltauv1;
vec3 deltauv2;
float r;
vec3 tangent;
vec3 bitangent;
mat3 tbn;
mat4 tbn_4;

vec3 Ligthdirection_tangentspace = vec3(10,-10,10);
uniform sampler2D textureHeight;
vec3 normal_tangent;

const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);

vec4 normalFromHeight(vec2 UV){
	vec4 wave = texture(textureHeight, UV);
    float s11 = wave.x;
    float s01 = textureOffset(textureHeight, UV, off.xy).x;
    float s21 = textureOffset(textureHeight, UV, off.zy).x;
    float s10 = textureOffset(textureHeight, UV, off.yx).x;
    float s12 = textureOffset(textureHeight, UV, off.yz).x;
    vec3 va = normalize(vec3(size.xy,s21-s01));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    vec4 bump = vec4( cross(va,vb), s11 );
	return bump;
}

void main(void)
{

deltapos1 = v_pos[1] - v_pos[0];
deltapos2 = v_pos[2] - v_pos[0];
deltauv1 = vec3(uv_pos[1],0) - vec3(uv_pos[0],0);
deltauv2 = vec3(uv_pos[2], 0) - vec3(uv_pos[0],0);
r = 1.0f / (deltauv1.x * deltauv2.y - deltauv1.y * deltauv2.x); 
tangent = (deltapos1 * deltauv2.y   - deltapos2 * deltauv1.y)*r; 
bitangent = (deltapos2 * deltauv1.x   - deltapos1 * deltauv2.x)*r;
for (int i = 0; i < 3; i++)
	{
	//normal_tangent = texture(textureSampler, uv_pos[i]).rgb *  2 -1;
	vec4 no = normalFromHeight(uv_pos[i]);
	tbn = transpose(mat3(
        normalize(tangent),
        normalize(bitangent),
        normalize(n_pos[i])
        ));
        tbn_4 = mat4(tbn);
	  gl_Position = MVP * vec4(v_pos[i] , 1.0);
	  uv = uv_pos[i];
	  pos = v_pos[i];
	  //n = tbn * normal_tangent;
	  n = tbn * no.xyz;
	  EmitVertex();
	}
	//n = tbn * n_pos[0];
	EndPrimitive();
}


