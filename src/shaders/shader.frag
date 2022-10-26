#version 330 core
out vec4 FragColor;

#define MAX_LIGHTS 128

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 viewPos; 
uniform vec3 objectColor;

struct Light {
    float constant;
    float linear;
    float quadratic;
    vec3 col;
    vec3 pos;
    float radius;
};
uniform Light light[MAX_LIGHTS];

void main()
{
    bool isEmissive;
    vec3 result;
    for(int i=0; i<MAX_LIGHTS; i++) {
        if(light[i].col.x != 0 && light[i].col.y != 0 && light[i].col.z != 0) {
            float distance = length(light[i].pos - FragPos);
            float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * (distance * distance)); 
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

            // if distance between light src and frag pos is equal, then its emmissive surface
            if(distance <= light[i].radius * 1.1)
                isEmissive = true;
            else
                result += (ambient + diffuse + specular) * objectColor;
        }
    }
    if(isEmissive)
        result = objectColor;
    FragColor = vec4(result, 1.0);
} 