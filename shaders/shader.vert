#version 450

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragTexCoord; // <-- Теперь vec3 (X=U, Y=V, Z=Флаг текстуры)

// 1. НАШ БЛОК МАТРИЦ (Uniform Buffer Object)
layout(binding = 0) uniform UniformBufferObject {
    mat4 model; 
    mat4 view;  
    mat4 proj;  
} ubo;

// 2. ПУШ-КОНСТАНТЫ ДЛЯ ВРАЩЕНИЯ СТРЕЛОЧКАМИ
layout(push_constant) uniform PushConstants {
    float angleX;
    float angleY;
} pc;

vec3 positions[36] = vec3[](
    // Front
    vec3(-0.5, -0.5,  0.5), vec3( 0.5, -0.5,  0.5), vec3( 0.5,  0.5,  0.5),
    vec3(-0.5, -0.5,  0.5), vec3( 0.5,  0.5,  0.5), vec3(-0.5,  0.5,  0.5),
    // Back
    vec3( 0.5, -0.5, -0.5), vec3(-0.5, -0.5, -0.5), vec3(-0.5,  0.5, -0.5),
    vec3( 0.5, -0.5, -0.5), vec3(-0.5,  0.5, -0.5), vec3( 0.5,  0.5, -0.5),
    // Left
    vec3(-0.5, -0.5, -0.5), vec3(-0.5, -0.5,  0.5), vec3(-0.5,  0.5,  0.5),
    vec3(-0.5, -0.5, -0.5), vec3(-0.5,  0.5,  0.5), vec3(-0.5,  0.5, -0.5),
    // Right
    vec3( 0.5, -0.5,  0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5,  0.5, -0.5),
    vec3( 0.5, -0.5,  0.5), vec3( 0.5,  0.5, -0.5), vec3( 0.5,  0.5,  0.5),
    // Top
    vec3(-0.5,  0.5,  0.5), vec3( 0.5,  0.5,  0.5), vec3( 0.5,  0.5, -0.5),
    vec3(-0.5,  0.5,  0.5), vec3( 0.5,  0.5, -0.5), vec3(-0.5,  0.5, -0.5),
    // Bottom
    vec3(-0.5, -0.5, -0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5, -0.5,  0.5),
    vec3(-0.5, -0.5, -0.5), vec3( 0.5, -0.5,  0.5), vec3(-0.5, -0.5,  0.5)
);

vec3 floorPositions[6] = vec3[](
    vec3(-5.0,  1.5, -5.0), vec3( 5.0,  1.5,  5.0), vec3( 5.0,  1.5, -5.0),
    vec3(-5.0,  1.5, -5.0), vec3(-5.0,  1.5,  5.0), vec3( 5.0,  1.5,  5.0)
);


// UV КООРДИНАТЫ ТОЛЬКО ДЛЯ ПОЛА
vec2 floorUvs[6] = vec2[](
    vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(1.0, 0.0),
    vec2(0.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0)
);

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0)
);

void main()
{
    vec3 pos;

    if (gl_VertexIndex < 36)
    {
        // КУБ
        pos = positions[gl_VertexIndex];

        mat3 rotX = mat3(
            1.0, 0.0, 0.0,
            0.0, cos(pc.angleX), sin(pc.angleX),
            0.0, -sin(pc.angleX), cos(pc.angleX)
        );

        mat3 rotY = mat3(
            cos(pc.angleY), 0.0, -sin(pc.angleY),
            0.0, 1.0, 0.0,
            sin(pc.angleY), 0.0, cos(pc.angleY)
        );

        pos = (rotY * rotX * pos) * 0.6;

        fragColor = colors[gl_VertexIndex / 6];
        
        // Для КУБА ставим флаг -1.0 (текстура не нужна)
        fragTexCoord = vec3(0.0, 0.0, -1.0); 
    }
    else
    {
        // ПОЛ
        pos = floorPositions[gl_VertexIndex - 36];

        fragColor = vec3(1.0, 1.0, 1.0);
        
        // Для ПОЛА передаем UV координаты и флаг 0.0 (текстура НУЖНА)
        vec2 uv = floorUvs[gl_VertexIndex - 36];
        fragTexCoord = vec3(uv, 0.0); 
    }

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);
}
