#type vertex
#version 450 core

layout(location=0) in vec3 a_Position;
layout(location=2) in vec4 a_Color;
layout(location=5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 Position;
	vec4 Color;

};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main(){
	Output.Color = a_Color;
	Output.Position = a_Position;

	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position,1.0);
}

#type fragment
#version 450 core

layout(location=0) out vec4 o_Color;
layout(location=1) out int o_EntityID;

struct VertexOutput
{
	vec3 Position;
	vec4 Color;


};
layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;
 
void main(){


	o_Color = Input.Color;

	o_EntityID = v_EntityID;
}