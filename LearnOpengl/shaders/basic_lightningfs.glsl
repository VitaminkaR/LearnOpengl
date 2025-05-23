#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;  

uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
uniform Material material;

struct LightSource
{
    int isDirLight;

    vec3 position;
    vec3 direction;
    
    float constant;
    float linear;
    float quadratic;  
    float spotCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define MAX_LIGHT_SOURCE 8 
uniform LightSource lightSources[MAX_LIGHT_SOURCE];
uniform int enabledLightSourceCount;

vec3 CalcDirLight(LightSource light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

    float angle = dot(normalize(light.direction), -normalize(lightDir));
    angle = max(angle,0); 
   if(light.spotCutoff <= 90 && acos(angle) > radians(light.spotCutoff))
       return vec3(0);

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0);

    for(int i = 0; i < enabledLightSourceCount; i++)
    {
        LightSource light = lightSources[i];
        if(light.isDirLight == 1)
            result += CalcDirLight(light, norm, viewDir);
        else
            result += CalcPointLight(light, norm, FragPos, viewDir);   
    }
    
    FragColor = vec4(result, 1.0);
} 