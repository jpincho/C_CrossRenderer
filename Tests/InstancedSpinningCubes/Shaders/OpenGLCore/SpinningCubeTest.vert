#version 400
uniform mat4 u_VP;

in vec3 a_VertexPosition;
in vec2 a_TexCoord;
in mat4 a_InstanceMatrix;
out vec2 v_TexCoord;

void main()
    {
    gl_Position = u_VP * a_InstanceMatrix * vec4 ( a_VertexPosition, 1.0f );
    v_TexCoord = a_TexCoord;
    }