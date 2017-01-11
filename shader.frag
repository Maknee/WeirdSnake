#version 330 core
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	float distance = length(lightPos - FragPos);
	float attenuation = 1.0f / (1.0f + 0.09f * distance + (0.032f * distance * distance));
	//ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diffuseStrength = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * lightColor * vec3(texture(ourTexture, TexCoord));

	//specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	vec3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0f), 128.0f) * lightColor;

	vec3 result = (ambient + diffuse + specular) * attenuation * objectColor;
	color = vec4(result, 1.0f);
}