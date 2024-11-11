#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_model;
uniform vec3 u_diffuseColor; // The diffuse surface color of the model
uniform vec3 u_lightPosition; // The position of your light source
uniform vec3 u_ambientColor;
uniform vec3 u_specularColor;
uniform float u_specularPower;
uniform mat4 u_myPerspective;
uniform float u_showNormals;

// Vertex inputs (attributes from vertex buffers)
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec3 a_normal;

// Vertex shader outputs
out vec3 v_color;
out mat4 mv;
out mat4 mvp;
out vec3 positionEye;
out vec3 N;
out vec3 L;
out vec3 V;
out float diffuse;
out float normalization;
out vec3 H;
//out float specular_normalized;
//out vec3 specular_color_computed;

void main()
{
    mat4 mvp = u_projection * u_view * u_model;
    mat4 mv = u_view * u_model;
    v_color = 0.5 * a_normal + 0.5; // maps the normal direction to an RGB color
    //gl_Position = a_position * mvp;
    gl_Position = mvp * vec4(a_position.xyz, 1.0);

    // Transform the vertex position to view space (eye coordinates)
    vec3 positionEye = vec3(mv * vec4(a_position.xyz, 1.0));

    // Calculate the view-space normal
    N = normalize(mat3(mv) * a_normal);

    // Calculate the view-space light direction
    L = normalize(u_lightPosition - positionEye);

    //  Calculate the view-vector
    V = -normalize(mat3(mv) * a_normal);

    // Calculate the diffuse (Lambertian) reflection term
    float diffuse = max(0.0, dot(N, L));

    // Multiply the diffuse reflection term with the base surface color
    v_color = diffuse * u_diffuseColor;

    H = (L+V)/(abs(L + V));

    if (u_showNormals > 0) {
        v_color = abs(a_normal);
    } else {
        v_color = v_color + u_ambientColor;
    }
}