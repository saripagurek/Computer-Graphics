#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_gammaCorrection;
uniform float u_specularPower;
uniform vec3 u_specularColor;

// Fragment shader inputs
in vec3 v_color;
in vec3 N;
in vec3 L;
in vec3 V;
in vec3 H;

// Fragment shader outputs
out vec4 frag_color;

void main()
{
    float normalization = (8.0 + u_specularPower) / 8.0; 
    float specular_normalized = normalization * pow(max(0.0, dot(N, H)), u_specularPower);
    vec3 specular_color_computed = u_specularColor * specular_normalized;
    vec3 calc_color = v_color + specular_color_computed;


    vec3 output_color = pow(calc_color, vec3(1 / 2.2));

    if (u_gammaCorrection > 0) {
        frag_color = vec4(output_color, 1.0);
    } else {
        frag_color = vec4(calc_color, 1.0);
   }

}