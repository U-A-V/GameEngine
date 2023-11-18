#type vertex
#version 450 core

layout(location=0) in vec3 a_Position;
layout(location=1) in vec4 a_Color;
layout(location=2) in vec3 a_Normal;
layout(location=3) in vec2 a_TexCoord;
layout(location=4) in float a_TexIndex;
layout(location=5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CamraPosition;
};

struct VertexOutput
{
	vec3 Position;
	vec4 Color;
	vec3 Normal;
	vec2 TexCoord;
	float TexIndex;
};

layout (location = 0) out VertexOutput Output;
layout (location = 5) out flat int v_EntityID;

void main(){
	Output.Position = a_Position;
	Output.Color = a_Color;
	Output.Normal = a_Normal;
	Output.TexCoord = a_TexCoord;
	Output.TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position,1.0);
}

#type fragment
#version 450 core

layout(location=0) out vec4 color;
layout(location=1) out int color2;

layout(std140) struct PointLight{
	vec4 Position;
	
	float constant;
	float linear;
	float quadratic;
	float padding;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

layout(std140) struct Material{
	vec4 Albedo;
	float Roughness;
	float Metallic;
	float padding;
	float padding1;
};

#define N_POINT_LIGHTS 100
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};

layout(std140, binding = 1) uniform PointLights{

	PointLight u_PointLights[N_POINT_LIGHTS];
};
layout(std140, binding = 2) uniform NumLights{
	int u_NPointlights;
};
layout(std140, binding = 3) uniform Materials{
	Material Mat[100];
};

struct VertexOutput
{
	vec3 Position;
	vec4 Color;
	vec3 Normal;
	vec2 TexCoord;
	float TexIndex;
};
layout (location = 0) in VertexOutput Input;
layout (location = 5) in flat int v_EntityID;
 
layout (binding = 0) uniform samplerCube SkyBox;

#define PI 3.14159265359

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

vec3 CalculatePointLight(PointLight light, vec3 fragPos, vec3 viewDir, vec3 normal, vec3 F0){
        // calculate per-light radiance
        vec3 L = normalize(light.Position.xyz - fragPos);
        vec3 H = normalize(viewDir + L);
        float distance    = length(light.Position.xyz - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			                light.quadratic * (distance * distance));  ;
        vec3 radiance     = light.ambient.xyz * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(normal, H, Mat[0].Roughness);        
        float G   = GeometrySmith(normal, viewDir, L, Mat[0].Roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Mat[0].Metallic;	  
        
        vec3 numerator = NDF * G * F;
        float NdotL = max(dot(normal, L), 0.0);                
        float NdotV = max(dot(normal, viewDir), 0.0);                
        float denominator = 4.0 * NdotV*NdotL + 0.0001;
        vec3 specular = numerator / denominator;  
            
        // add to outgoing radiance Lo
        return  vec3((kD * Mat[0].Albedo.xyz / PI + specular) * radiance * NdotL); 
}

void main(){
	
	vec3 norm = normalize(Input.Normal);
	vec3 viewDir = normalize(u_CameraPosition.xyz - Input.Position);
	
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, Mat[0].Albedo.xyz, Mat[0].Metallic);

	vec3 result=vec3(0.0);
	for(int i = 0; i < u_NPointlights; i++) result+=CalculatePointLight(u_PointLights[i], Input.Position.xyz, viewDir, norm, F0);

    vec3 R = reflect(-viewDir, norm);
    vec3 ambient = vec3(1.0 - Mat[0].Roughness) * Mat[0].Albedo.xyz * texture(SkyBox, R).rgb ;

    vec3 col = ambient + result;
	
    col = col / (col + vec3(1.0));
    col = pow(col, vec3(1.0/2.2));

	color=vec4(col,Mat[0].Albedo.w);

	color2 = v_EntityID;
}