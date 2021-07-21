#version 330

in vec3 worldPos;
in vec3 normal;

struct Light
{
	vec3 lightCol;
	vec3 lightPos;
};
uniform Light light;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 cameraPos; 

out vec4 FragColor;


const float PI = 3.14159265359;

// fresnel - reflected radiance
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

// normal distribution function: micro facets align with H 
float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

//  micro surface overshadowing
float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

void main()
{
	vec3 N = normalize(normal);
	vec3 V = normalize(cameraPos - worldPos);
	vec3 L = normalize(light.lightPos - worldPos);
	vec3 H = normalize(V + L);
	float distance = length(light.lightPos - worldPos);
	float attenuation = 1 / (distance * distance);
	float cosTheta = max(dot(N, L), 0);
	// rediance
	vec3 radiance = light.lightCol * attenuation;

	// fresnal
	vec3 f0 = vec3(0.4);
	f0 = mix(f0, albedo, metallic);
	vec3 fresnel = fresnelSchlick(cosTheta, f0);

	vec3 KS = fresnel; // reflected light energy -- specular ratio
	vec3 KD = vec3(1) - KS; // refracted light energy -- diffuse ratio

	// reflection part of BRDF
	float geometryOvershadow = GeometrySmith(N, V, L, roughness);
	float normalDistribution = DistributionGGX(N, H, roughness);
	vec3 DFG = geometryOvershadow * normalDistribution * fresnel;
	float dominator = 4 * max(dot(N, V), 0) * cosTheta;
	vec3 specular = DFG / max(dominator, 0.001);

	
	vec3 diffuse = albedo / PI; // c / PI

	vec3 reflectance = (KD * diffuse + KS * specular) * radiance * cosTheta;

	// ambience
	vec3 ambient = vec3(0.03) * albedo * ao;

	vec4 col = vec4(1);
	FragColor = col;
}