#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragTexCoord; 

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    float angleX;
    float angleY;
} pc;

// ДОБАВИЛИ КЛЮЧЕВОЕ СЛОВО uniform
layout(binding = 1) uniform sampler2D texSampler; 

void main()
{
    if (fragTexCoord.z < 0.0) 
    {
        outColor = vec4(fragColor, 1.0);
    } 
    else 
    {
        vec4 texColor = texture(texSampler, fragTexCoord.xy);
        outColor = vec4(texColor.rgb, 1.0); 
    }
}
