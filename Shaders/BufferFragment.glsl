#version 330 core

uniform sampler2D rockTex;	// Diffuse texture map
uniform sampler2D rockBump;		// Bump map

uniform sampler2D grassTex;
uniform sampler2D grassBump;

uniform sampler2D sandTex;
uniform sampler2D sandBump;

uniform sampler2D snowTex;
uniform sampler2D snowBump;

uniform float worldHeight;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour[2];	// Final outputted colours

void main(void) {
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal)); // Form tangent binormal normal matrix
	vec3 normal = texture2D(rockBump, IN.texCoord).rgb * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));

	float NYPos = IN.worldPos.y / worldHeight; // Normalised Y pos
	float range1 = 0.50;
	float range2 = 0.55;
	float range3 = 0.75;
	float range4 = 0.85;

	float normalRange = 0.50;

	if(NYPos <= range1) {
		fragColour[0] = texture2D(sandTex, IN.texCoord);
		vec3 normal = texture2D(sandBump, IN.texCoord).rgb * 2.0 - 1.0;
		normal = normalize(TBN * normalize(normal));
		fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
	} else if (NYPos <= range2) { // Normalised weighting for interpolation between two textures
		NYPos -= range1;
		NYPos /= (range2 - range1);
		if (normal.x >= normalRange || normal.x <= -normalRange || normal.z >= normalRange || normal.z <= -normalRange) {
			fragColour[0] = mix(texture2D(sandTex, IN.texCoord), texture2D(rockTex, IN.texCoord), NYPos);
			vec3 normal1 = texture2D(sandBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal1 = normalize(TBN * normalize(normal1));
			vec3 normal2 = texture2D(rockBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal2 = normalize(TBN * normalize(normal2));
			fragColour[1] = vec4(mix(normal1, normal2, NYPos) * 0.5 + 0.5, 1.0);
		} else {
			fragColour[0] = mix(texture2D(sandTex, IN.texCoord), texture2D(grassTex, IN.texCoord), NYPos);
			vec3 normal1 = texture2D(sandBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal1 = normalize(TBN * normalize(normal1));
			vec3 normal2 = texture2D(grassBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal2 = normalize(TBN * normalize(normal2));
			fragColour[1] = vec4(mix(normal1, normal2, NYPos) * 0.5 + 0.5, 1.0);
		}
	} else if (NYPos <= range3) {
		if (normal.x >= normalRange || normal.x <= -normalRange || normal.z >= normalRange || normal.z <= -normalRange) {
			fragColour[0] = texture2D(rockTex, IN.texCoord);
			vec3 normal = texture2D(rockBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal = normalize(TBN * normalize(normal));
			fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
		} else {
			fragColour[0] = texture2D(grassTex, IN.texCoord);
			vec3 normal = texture2D(grassBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal = normalize(TBN * normalize(normal));
			fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
		}
	} else if (NYPos <= range4){ // Normalised weighting for interpolation between two textures	
		NYPos -= range3;
		NYPos /= (range4 - range3);
		if (normal.x >= normalRange || normal.x <= -normalRange || normal.z >= normalRange || normal.z <= -normalRange) {
			fragColour[0] = mix(texture2D(rockTex, IN.texCoord), texture2D(snowTex, IN.texCoord), NYPos);
			vec3 normal1 = texture2D(rockBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal1 = normalize(TBN * normalize(normal1));
			vec3 normal2 = texture2D(snowBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal2 = normalize(TBN * normalize(normal2));
			fragColour[1] = vec4(mix(normal1, normal2, NYPos) * 0.5 + 0.5, 1.0);
		} else {
			fragColour[0] = mix(texture2D(grassTex, IN.texCoord), texture2D(snowTex, IN.texCoord), NYPos);
			vec3 normal1 = texture2D(grassBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal1 = normalize(TBN * normalize(normal1));
			vec3 normal2 = texture2D(snowBump, IN.texCoord).rgb * 2.0 - 1.0;
			normal2 = normalize(TBN * normalize(normal2));
			fragColour[1] = vec4(mix(normal1, normal2, NYPos) * 0.5 + 0.5, 1.0);
		}
	} else {
		fragColour[0] = texture2D(snowTex, IN.texCoord);
		vec3 normal = texture2D(snowBump, IN.texCoord).rgb * 2.0 - 1.0;
		normal = normalize(TBN * normalize(normal));
		fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
	}

	NYPos = IN.worldPos.y / worldHeight;
}