#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D sampler;
uniform sampler2D sampler2;

void main()
{
FragColor = mix(texture(sampler,TexCoord),texture(sampler2,TexCoord),0.2);
}