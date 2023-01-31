#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/HeightMap.h"

class Camera;
class SceneNode;
class Mesh;
class MeshAnimation;
class MeshMaterial;
class HeightMap;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

	void ToggleFog();
	void ToggleBlur();

protected:
	bool LoadTextures();
	bool LoadShaders();
	vector<GLuint> LoadSubTextures(SceneNode* node, MeshMaterial* material);

	// Scene drawing
	void DrawHeightMap();
	void DrawWater();
	void DrawSkybox();
	void DrawPostProcessing();
	void DrawBlur();

	// Scene graph
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* node);

	// Deferred Rendering methods
	void FillBuffers();
	void DrawPointLights();
	void DrawShadows();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint& into, bool depth = false, bool shadow = false);

	Shader* sceneShader;
	Shader* nodeShader;
	Shader* shadowSceneShader;
	Shader* shadowShader;
	Shader* pointlightShader;
	Shader* processShader;
	Shader* combineShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* animationShader;

	GLuint bufferFBO;			// G-Buffer pass 
	GLuint bufferColourTex[2];	// Albedo
	GLuint bufferNormalTex;		// Normals
	GLuint bufferDepthTex;		// Depth

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint pointLightFBO;		// Lighting pass
	GLuint lightrockTex;		// Diffuse Lighting
	GLuint lightSpecularTex;	// Specular Lighting

	GLuint processFBO;

	HeightMap* heightMap;		// Terrain
	Light* pointLights;			// Lighting data
	Mesh* sphere;				// Light volume
	Mesh* quad;					// To draw the full-screen quad
	Camera* camera;				// Camera

	GLuint rockTex;				// Terrain base texture
	GLuint rockBump;			// Terrain bump texture
	GLuint grassTex;
	GLuint grassBump;			
	GLuint sandTex;
	GLuint sandBump;
	GLuint snowTex;
	GLuint snowBump;

	GLuint cubeMap;				// Cube map textures
	GLuint waterTex;			// Water texture
	GLuint waterBump;

	GLuint ruinedTowerTex;
	GLuint ruinedTowerBump;
	GLuint towerRoofTex;
	GLuint towerRoofBump;
	GLuint towerWallsTex;
	GLuint towerWallsBump;
	GLuint towerWoodTex;
	GLuint towerWoodBump;
	GLuint shipTex;
	GLuint shipBump;
	GLuint rockOneTex;
	GLuint rockOneBump;
	GLuint rockTwoTex;
	GLuint rockTwoBump;
	GLuint rockThreeTex;
	GLuint rockThreeBump;
	GLuint rockFourTex;
	GLuint rockFourBump;
	GLuint rockFiveTex;
	GLuint rockFiveBump;
	GLuint spiderTex;

	float waterRotate;
	float waterCycle;

	SceneNode* root;
	SceneNode* ruinedTower;
	SceneNode* roundTower;
	SceneNode* ship;
	SceneNode* rock;
	SceneNode* glowingOrb;

	Mesh* shipMesh;
	Mesh* rockOneMesh;
	Mesh* rockTwoMesh;
	Mesh* rockThreeMesh;
	Mesh* rockFourMesh;
	Mesh* rockFiveMesh;

	Frustum frameFrustum;
	GLuint texture;
	MeshAnimation* animation;
	MeshMaterial* material;
	vector<GLuint> matTextures;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	// Post Processing flags
	bool usingFog = false;
	bool usingBlur = false;

	int currentFrame;
	float frameTime = 0.0f;
	float framerate = 0.0f;
};