#version 330 core

in vec2 uv;
in vec3 n;
in vec3 pos;
in vec3 normal;

out vec4 color;

uniform float time;
uniform sampler2D textureSampler;
uniform sampler2D textureHeight;
uniform sampler2D texturefromfbo;
uniform vec3 POS_CAM;


float Ia = 0.2;
float Kd = 0.8;
float ks = 0.2;
float es = 38;
float Is;
float Id;


vec3 lum = vec3(10,-10,10);
vec3 ul = normalize(lum -pos);
vec3 t = normalize(POS_CAM - pos);
vec4 nt4 = normalize(texture( textureSampler, uv + time/50 * vec2(1,0)));
vec3 nt =  normalize(vec3(nt4.xyz)*2.0 -1);
vec3 s = normalize(reflect(ul, nt));

vec4 c;
float alpha = 0.7;
float y = clamp(texture(textureHeight, uv).r,0,1) ;


void main()
{
    
    Id = Kd * clamp(dot(nt, ul),0,1);
    Is = ks * pow( clamp(dot(s,t),0,1), es);
    c = (1 -  alpha) * texture(texturefromfbo, uv) + alpha * vec4(0,0.2*y,0.8*y,1.0);
    color = (Ia + Id) * c + Is* vec4(0.3,0.3,0.3,1) ;
    color = vec4(0*y,0.4*y,0.75*y,1);
};
