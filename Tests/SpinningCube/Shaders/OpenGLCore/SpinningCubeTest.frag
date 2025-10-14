#version 400
out vec4 FragOutput;

in vec2 v_TexCoord;
uniform sampler2D u_Texture;

void main()
    {
    FragOutput = texture ( u_Texture, v_TexCoord );
    }
