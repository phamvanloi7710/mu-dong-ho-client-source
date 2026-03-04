#pragma once

#ifdef USING_SHADER_330
namespace EmbeddedShaderVBO {
struct ShaderSourcePair { const char* vertex; const char* fragment; };
static const char kModelVertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    uv = aTex;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kModelFragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kBlendMeshVertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    uv = aTex + u_meshUV.xy;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kBlendMeshFragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kChrome1VertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    float wave = u_setting1.z;
    uv.x = normal.z * u_setting2.x + wave;
    uv.y = normal.y * u_setting2.y + wave * u_setting2.z;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kChrome1FragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kChrome2VertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    float wave = u_setting1.w;
    uv.x = (normal.z + normal.x) * u_setting2.x + wave * u_setting2.y;
    uv.y = (normal.y + normal.x) * u_setting2.z + wave * u_setting2.w;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kChrome2FragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kChrome3VertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    float d = dot(normal, u_setting2.xyz);
    uv.x = d;
    uv.y = 1.0 - d;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kChrome3FragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kChrome4VertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    vec3 lightVec = u_setting1.xyz;
    float wave = u_setting1.w;
    float d = dot(normal, lightVec);
    uv.x = d + (normal.y * u_setting2.x + lightVec.y * u_setting2.y);
    uv.y = 1.0 - d - (normal.z * u_setting2.z + wave * u_setting2.w);
    uv += u_meshUV.xy;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kChrome4FragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kChrome5VertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    vec3 lightVec = u_setting1.xyz;
    float wave = u_setting1.w;
    float d = dot(normal, lightVec);
    uv.x = d + (normal.y * u_setting2.z + lightVec.y * u_setting2.w);
    uv.y = 1.0 - d - (normal.z * u_setting2.x + wave * u_setting2.y);

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kChrome5FragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kChrome6VertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    float wave = u_setting1.z;
    float base = (normal.z + normal.x) * u_setting1.x + wave * u_setting1.y;
    uv = vec2(base, base);

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kChrome6FragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kChrome7VertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    float base = (normal.z + normal.x);
    float timeTerm = u_setting1.z * u_setting1.w;
    uv.x = base * u_setting1.x + timeTerm;
    uv.y = base * u_setting1.y + timeTerm;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kChrome7FragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kOilVertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    uv = (normal.xy * aTex) + u_meshUV.xy;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kOilFragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const char kMetalVertexSource[] = R"__VBO_SHADER__(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in uint aBone;

uniform mat4 uProj;
uniform mat4 uView;
uniform vec4 u_Bones[600];

uniform vec4 u_bodyLight;
uniform vec4 u_lightPosition;
uniform vec4 u_meshUV;
uniform vec4 u_setting1;
uniform vec4 u_setting2;
uniform int u_enableLight;

out vec2 vTex;
out vec4 vColor;
out vec3 vNormal;

vec3 ApplyBonePosition(vec3 pos, uint boneIndex)
{
    vec4 p = vec4(pos, 1.0);
    vec4 r0 = u_Bones[boneIndex + 0u];
    vec4 r1 = u_Bones[boneIndex + 1u];
    vec4 r2 = u_Bones[boneIndex + 2u];
    return vec3(dot(r0, p), dot(r1, p), dot(r2, p));
}

vec3 ApplyBoneNormal(vec3 normal, uint boneIndex)
{
    vec3 r0 = u_Bones[boneIndex + 0u].xyz;
    vec3 r1 = u_Bones[boneIndex + 1u].xyz;
    vec3 r2 = u_Bones[boneIndex + 2u].xyz;
    return normalize(vec3(dot(r0, normal), dot(r1, normal), dot(r2, normal)));
}

void main()
{
    uint boneIndex = aBone;
    vec3 worldPos = ApplyBonePosition(aPos, boneIndex);
    vec3 normal = ApplyBoneNormal(aNormal, boneIndex);

    vec2 uv = vec2(0.0);
    uv.x = normal.z * u_setting2.x + u_setting2.y;
    uv.y = normal.y * u_setting2.z + u_setting2.w;

    vTex = uv;
    vNormal = normal;

    vec4 color = u_bodyLight;
    if (u_enableLight != 0) {
        float diffuse = max(dot(normal, u_lightPosition.xyz), 0.0);
        float ambient = 0.6;   // tăng ambient
        float intensity = ambient + diffuse * 1.2;
        if (intensity < 0.2) intensity = 0.2;
        color.rgb *= intensity;
    }

    vColor = u_bodyLight;

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}

)__VBO_SHADER__";

static const char kMetalFragmentSource[] = R"__VBO_SHADER__(
#version 330 core

in vec2 vTex;
in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTex);
    FragColor = texColor * vColor;
}
)__VBO_SHADER__";

static const ShaderSourcePair kShaderSources[] = {
    { kModelVertexSource, kModelFragmentSource },
    { kBlendMeshVertexSource, kBlendMeshFragmentSource },
    { kChrome1VertexSource, kChrome1FragmentSource },
    { kChrome2VertexSource, kChrome2FragmentSource },
    { kChrome3VertexSource, kChrome3FragmentSource },
    { kChrome4VertexSource, kChrome4FragmentSource },
    { kChrome5VertexSource, kChrome5FragmentSource },
    { kChrome6VertexSource, kChrome6FragmentSource },
    { kChrome7VertexSource, kChrome7FragmentSource },
    { kOilVertexSource, kOilFragmentSource },
    { kMetalVertexSource, kMetalFragmentSource },
};
} // namespace EmbeddedShaderVBO
#endif