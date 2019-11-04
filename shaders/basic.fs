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
vec3 nt =  normalize(texture2D( textureSampler, uv)).rgb * 2.0 -1;
vec3 s = normalize(reflect(ul, n));

vec4 c;
float alpha = 0.7;
float y = clamp(texture(textureHeight, uv).r,0,1) ;

const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);
void main()
{

    vec4 wave = texture(textureHeight, uv);
    float s11 = wave.x;
    float s01 = textureOffset(textureHeight, uv, off.xy).x;
    float s21 = textureOffset(textureHeight, uv, off.zy).x;
    float s10 = textureOffset(textureHeight, uv, off.yx).x;
    float s12 = textureOffset(textureHeight, uv, off.yz).x;
    vec3 va = normalize(vec3(size.xy,s21-s01));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    vec4 bump = vec4( cross(va,vb), s11 );
    
    Id = Kd * clamp(dot(n, ul),0,1);
    Is = ks * pow( clamp(dot(s,t),0,1), es);
    //c = (1 -  alpha) * texture(texturefromfbo, uv) + alpha * vec4(0,0.2*y,0.8*y,1.0);
    c = vec4(0,0.4,0.7,1);
    color = (Ia + Id) * c + Is* vec4(0.3,0.3,0.3,1) ;
    //color = vec4(0*y,0.4*y,0.75*y,1);
	//color = vec4(nt,1);
};
