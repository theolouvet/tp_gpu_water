#version 330 core

in vec2 uv;
in vec3 n;
in vec3 pos;
in vec3 normal;
in mat3 tbn;
in mat4 tbn_4;
in vec3 tangent;
in vec3 bitangent;
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
vec3 nt =  normalize(texture2D( textureSampler, uv)).rgb * 2.0 -1;
vec3 s;
vec3 normal_tangent;

vec4 c;
float alpha = 0.5;
float y = clamp(texture(textureHeight, uv).r,0,1) ;

const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);
vec2 dep;
vec4 normalFromHeight(vec2 UV){
    dep = time/50 * vec2(1,0);
    UV = UV +dep;
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

void main()
{
    vec4 normalfromheight = normalFromHeight(uv);
    normal_tangent = normalfromheight.xyz;
    //normal_tangent = (texture(textureSampler, uv + time/50 * vec2(1,0)).rgb *  2 -1);   
    nt = tbn * normal_tangent;
    nt = nt.rbg;
    s = normalize(reflect(ul, nt));

    Id = Kd * clamp(dot(nt, ul),0,1);
    Is = ks * pow( clamp(dot(s,t),0,1), es);
    
    vec4 watercolor = vec4(0.0,0.4,0.75,1);

    vec4 c = (1 - alpha) * texture(texturefromfbo, uv);// + alpha * watercolor;

    color = (Id + Ia) * watercolor + Is * vec4(0.3,0.3,0.3,1) + c;
    
};
