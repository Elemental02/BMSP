#version 430 core
in vec4 fragmentColor;
in vec2 texCoord;
out vec4 color;
uniform sampler2D TextureSampler;
void main(){
	color = texture(TextureSampler, texCoord).rgba;
	color = color * fragmentColor;
}