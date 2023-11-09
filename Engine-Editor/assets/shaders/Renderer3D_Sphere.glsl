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
 

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Position.xyz - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),16);

    float distance    = length(light.Position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));  
				 

    vec3 ambient  = light.ambient.xyz  * Input.Color.xyz;
    vec3 diffuse  = light.diffuse.xyz  * diff * Input.Color.xyz;
    vec3 specular = light.specular.xyz * spec * Input.Color.xyz;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void main(){
	
	vec3 norm = normalize(Input.Normal);
	vec3 viewDir = normalize(u_CameraPosition.xyz - Input.Position);
	vec3 result=vec3(0.0f);
	for(int i = 0; i < u_NPointlights; i++)
       result += CalcPointLight(u_PointLights[i], norm, Input.Position,viewDir); 
	color=vec4(result,Input.Color.w);

	color2 = v_EntityID;
}