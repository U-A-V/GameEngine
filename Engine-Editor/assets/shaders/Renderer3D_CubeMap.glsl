#type vertex
#version 450 core
layout (location = 0) in vec4 a_Position;

layout (location =0) out vec3 TexCoords;

layout(std140,  binding = 0) uniform Camera{
    mat4 u_ViewProjection;
    vec4 u_CameraPosition;
};

void main()
{
    TexCoords = a_Position.xyz;
    vec4 pos = u_ViewProjection * vec4(a_Position.xyz, 0.001);
    gl_Position = pos.xyww;
}  

#type fragment
#version 450 core

layout(location=0) out vec4 FragColor;
layout(location=1) out int color2;
layout (location =0) in vec3 TexCoords;

layout (binding =0) uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
    color2 = -1;
}