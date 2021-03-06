#version 330 core

in VS_OUT
{
	vec2 texCoord;
	vec3 normal,tangent;
	vec4 color;
	vec3 fragPos;
	//vec4 fragPosLightSpace; 
	
} fs_in;

struct Light {
    vec4 position ;
    vec4 direction;
    vec4 color ;
};

#define MAXLIGHTS 32
layout(std140, binding = 1) uniform uLights {
    Light lights [ MAXLIGHTS ];
    int lightNum;
};

#define MaxShadowMap 16
layout(std140, binding = 2) uniform uLightTransformMatrix {
     mat4 uLightViewProj[MaxShadowMap];
     int ShadowMapCount;

};


uniform vec3 viewPos; 
//uniform vec3 lightColor;
#define MaxNumShadowMap 16
uniform sampler2D shadowMap[MaxShadowMap];
uniform samplerCube skyBox;

out vec4 FragColor;
//uniform vec3 lightPos; 

//uLightViewProj[UShadowMapIndex]* vec4(vs_out.fragPos, 1.0);

float ShadowCalculation(vec4 fragPosLightSpace,vec3 lightPos,int shadowMapIndex)
{
     // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap[shadowMapIndex], projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    if (currentDepth>1.0) {
        return 1.0;
    }
    // check whether current frag pos is in shadow
    vec3 lightDir = normalize(lightPos - fs_in.fragPos);
    //PCF
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);  
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap[shadowMapIndex], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap[shadowMapIndex], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

vec3 CalculateLambertian(vec3 lightPos,vec3 lightColor){
     // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    float diffusetStrength = 0.8;
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = normalize(lightPos - fs_in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =diffusetStrength* diff * lightColor;
    
    // specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    //reflection skycolor
    vec3 I = normalize( fs_in.fragPos-viewPos);
    vec3 R = reflect(I, norm);
    vec4 skyColor= texture(skyBox, R);
    vec3 reflection = skyColor.rgb;  

    float ratio = 1.00 / 1.52;
    R = refract(I, norm, ratio);
    skyColor= texture(skyBox, R);
    vec3 refraction = skyColor.rgb;  
    
   
    // ������Ӱ
    float shadow=0.0; 
    for(int i=0;i<ShadowMapCount;i++) {
        vec4 fragPosLightSpace=uLightViewProj[i]* vec4(fs_in.fragPos, 1.0);
        shadow += ShadowCalculation(fragPosLightSpace,lightPos,i);
    }
    shadow/=float(ShadowMapCount);
    float attenuation= 1.0/ pow(length(lightPos-fs_in.fragPos)+1.0,2);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * fs_in.color.rgb;  
    return lighting;
}

void main()
{
    vec3 lighting=vec3(0.0f);
    for (int i=0;i<lightNum;i++){
        lighting+=CalculateLambertian(lights[i].position.xyz,lights[i].color.xyz);
    }
    FragColor = vec4(lighting,1.0f);
}