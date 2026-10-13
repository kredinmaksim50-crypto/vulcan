
#version 450

layout(location = 0) out vec3 fragColor;

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

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    vec3(0.0, 1.0, 1.0)
);

void main()
{
    vec3 pos = positions[gl_VertexIndex];

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

    vec3 rotatedPos = (rotY * rotX * pos) * 0.6;

    gl_Position = vec4(
        rotatedPos.x,
        rotatedPos.y,
        rotatedPos.z + 0.5,
        1.0
    );

    fragColor = colors[gl_VertexIndex / 6];
}
