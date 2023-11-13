#type compute
#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#define PI 3.14159265359


layout ( binding=0, rgba8) readonly uniform image2D inputImage;
layout( binding=1, rgba8) writeonly uniform imageCube CubeMap;


void main() {
	ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
    int face = int(gl_GlobalInvocationID.z);
    
    vec2 normTexCoords = vec2(float(texCoord.x)/float(1024) - 0.5, float(texCoord.y)/float(1024) -0.5);
    vec3 norm3DCoords;
    if(face == 0)   norm3DCoords = vec3(0.5,normTexCoords.x,normTexCoords.y );
    else if(face == 1)   norm3DCoords = vec3(-0.5,-normTexCoords.x,normTexCoords.y );
    else if(face == 5)   norm3DCoords = vec3(-normTexCoords.x,0.5,normTexCoords.y );
    else if(face == 4)   norm3DCoords = vec3(normTexCoords.x,-0.5,normTexCoords.y );
    else if(face == 3)   norm3DCoords = vec3(normTexCoords.x,normTexCoords.y,0.5 );
    else if(face == 2)   norm3DCoords = vec3(normTexCoords.x,-normTexCoords.y,-0.5 );


    float radius = sqrt(norm3DCoords.x * norm3DCoords.x + norm3DCoords.y * norm3DCoords.y + norm3DCoords.z * norm3DCoords.z);

    float rad = sqrt(norm3DCoords.x * norm3DCoords.x + norm3DCoords.y * norm3DCoords.y);
    //float theta = atan((rad - abs(norm3DCoords.x))/(abs(norm3DCoords.x) + rad));
    float theta = atan(norm3DCoords.y,norm3DCoords.x);
    float phi = acos(norm3DCoords.z/radius);

    float u = theta/(2*PI);
    float v = phi/PI;
    if(u<0) u=1+u;
    
    u = u*imageSize(inputImage).x;
    v = v*imageSize(inputImage).y;

    int iu = int(u);
    int iv = int(v);
    
    // Sample HDR image and write to the cubemap face
    vec4 color = imageLoad(inputImage, ivec2(iu,iv));

     //color = texelFetch(inputImage, ivec2(iu,iv),0);

    //color = vec4( vec2(texCoord.x, texCoord.y)/vec2(512),float(face)/float(6),1.0);
    imageStore(CubeMap,ivec3(gl_GlobalInvocationID), vec4(color));
    
}