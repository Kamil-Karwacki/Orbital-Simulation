#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define MAX_LIGHTS 64

in vec3 Normal;
in vec3 FragPos;  
in vec3 vertCol;
  
uniform vec3 viewPos; 

struct Light {
    float constant;
    float linear;
    float quadratic;
    vec3 col;
    vec3 pos;
    float radius;
    float strength;
};
uniform Light light[MAX_LIGHTS];

void main()
{
    vec3 result = vec3(0.0);
    for(int i=0; i<MAX_LIGHTS; i++) {
        if(light[i].strength > 0) {
            float distance = length(light[i].pos - FragPos);
            float attenuation = 1.0 * light[i].strength / (light[i].constant + light[i].linear * distance + light[i].quadratic * (distance * distance)); 
            float ambientStrength = 0.05;
            vec3 ambient = ambientStrength * light[i].col * attenuation;
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(light[i].pos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * light[i].col * attenuation;
            
            float specularStrength = 0.05;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * light[i].col * attenuation;  

            result += (ambient + diffuse + specular) * vertCol;
        }
    }

    FragColor = vec4(result, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
} 