#version 400 core

out vec4 color;

uniform sampler2D diffuseMap;
uniform sampler2D emissiveMap;
uniform sampler2D occlusionMap;
uniform sampler2D specularMap;

uniform vec4 diffuseColor;
uniform vec4 emissiveColor;
uniform vec4 specularColor;

uniform vec3  light_pos;
uniform float ambient;
uniform float shininess;
uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;
uniform mat4 view;

in vec3 position;
in vec3 normal; 
in vec2 uv0;

void main()
{
    /*Ambient */
    vec3 finalAmbient = k_ambient * ambient * vec3(texture(occlusionMap, uv0));  
	
    /*Diffuse */
	vec3 normal = normalize(normal);
    vec3 lightDir = normalize(light_pos - position);
    float diff = max(dot(normal, lightDir), 0.0);
	vec3 finalDiffuse = k_diffuse * diff * diffuseColor.rgb * vec3(texture(diffuseMap, uv0));
    
    /*Specular */
    vec3 viewPos = transpose(mat3(view))*(-view[3].xyz);
	vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec4 textureSpecularColor = texture(specularMap, uv0);
	vec4 algo = vec4(textureSpecularColor.rgb*specularColor.rgb, max(textureSpecularColor.a*shininess*128.0f, 8.0f));
	vec3 finalSpecular = algo.rgb * k_specular * spec * specularColor.rgb;

    	
    vec3 finalEmission = emissiveColor.rgb * texture(emissiveMap, uv0).rgb;
		
    color = vec4(finalAmbient + finalDiffuse + finalSpecular + finalEmission, 1.0f);
}
