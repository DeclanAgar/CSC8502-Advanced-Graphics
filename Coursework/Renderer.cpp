#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include <algorithm>

#define SHADOWSIZE 2048
const int POST_PASSES = 10;
const int LIGHT_NUM = 2;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	quad = Mesh::GenerateQuad();

	if (!LoadTextures()) 
		return;

	SetTextureRepeating(rockTex, true);
	SetTextureRepeating(rockBump, true);
	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(grassBump, true);
	SetTextureRepeating(sandTex, true);
	SetTextureRepeating(sandBump, true);
	SetTextureRepeating(snowTex, true);
	SetTextureRepeating(snowBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterBump, true);
	SetTextureRepeating(spiderTex, true);

	if (!LoadShaders()) 
		return;

	Vector3 heightmapSize = heightMap->GetHeightMapSize();

	camera = new Camera(0.0f, 0.0f, 0.0f, Vector3(heightmapSize.x * 0.5f, 500.0f, heightmapSize.z * 0.5f));
	root = new SceneNode();
	root->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.5f, 0.5f, 0.5f)));

	ruinedTower = new SceneNode();
	root->AddChild(ruinedTower);
	ruinedTower->SetMesh(Mesh::LoadFromMeshFile("RuinedTower.msh"));
	ruinedTower->SetTexture(ruinedTowerTex);
	ruinedTower->SetTransform(Matrix4::Translation(Vector3(-3600.0f, 0.0f, 3750.0f)) * Matrix4::Rotation(180, Vector3(0.0f, 1.0f, 0.0f)));
	ruinedTower->SetModelScale(Vector3(25.0f, 25.0f, 25.0f));
	ruinedTower->SetBoundingRadius(50.0f);

	ship = new SceneNode();
	root->AddChild(ship);
	ship->SetMesh(Mesh::LoadFromMeshFile("BrigSloop.msh"));
	material = new MeshMaterial("BrigSloop.mat");
	ship->SetSubTextures(LoadSubTextures(ship, material));
	ship->SetTransform(Matrix4::Translation(Vector3(1650.0f, 0.0f, -1650.0f)) * Matrix4::Rotation(90, Vector3(0.0f, 1.0f, 0.0f)));
	ship->SetModelScale(Vector3(10.0f, 10.0f, 10.0f));

	glowingOrb = new SceneNode();
	ruinedTower->AddChild(glowingOrb);
	glowingOrb->SetMesh(Mesh::LoadFromMeshFile("Sphere.msh"));
	glowingOrb->SetTransform(Matrix4::Translation(Vector3(0.0f, 350.0f, 0.0f)));
	glowingOrb->SetModelScale(Vector3(30.0f, 30.0f, 30.0f));
	glowingOrb->SetColour(Vector4(1, 0, 0, 1));

	SceneNode* rockOne = new SceneNode();
	root->AddChild(rockOne);
	rockOne->SetMesh(Mesh::LoadFromMeshFile("Rock_02.msh"));
	material = new MeshMaterial("Rock_02.mat");
	rockOne->SetSubTextures(LoadSubTextures(rockOne, material));
	rockOne->SetTransform(Matrix4::Translation(Vector3(450.0f, 0.0f, -650.0f))* Matrix4::Rotation(-30, Vector3(0.0f, 1.0f, 0.0f)));
	rockOne->SetModelScale(Vector3(50.0f, 50.0f, 50.0f));

	SceneNode* rockTwo = new SceneNode();
	root->AddChild(rockTwo);
	rockTwo->SetMesh(Mesh::LoadFromMeshFile("Rock_03.msh"));
	material = new MeshMaterial("Rock_03.mat");
	rockTwo->SetSubTextures(LoadSubTextures(rockTwo, material));
	rockTwo->SetTransform(Matrix4::Translation(Vector3(1200.0f, 0.0f, -650.0f)));
	rockTwo->SetModelScale(Vector3(80.0f, 50.0f, 50.0f));

	SceneNode* spider = new SceneNode();
	root->AddChild(spider);
	spider->SetMesh(Mesh::LoadFromMeshFile("spider.msh"));
	spider->SetTexture(spiderTex);
	spider->SetHasAnim(true);
	animation = new MeshAnimation("Spider.anm");
	spider->SetTransform(Matrix4::Translation(Vector3(-1100.0f, 130.0f, -1250.0f)));
	spider->SetModelScale(Vector3(20.0f, 20.0f, 20.0f));

	SceneNode* rockThree = new SceneNode();
	spider->AddChild(rockThree);
	rockThree->SetMesh(Mesh::LoadFromMeshFile("Rock_04.msh"));
	material = new MeshMaterial("Rock_04.mat");
	rockThree->SetSubTextures(LoadSubTextures(rockThree, material));
	rockThree->SetTransform(Matrix4::Translation(Vector3(-200.0f, -130.0f, 400.0f)) * Matrix4::Rotation(-30, Vector3(0.0f, 1.0f, 0.0f)));
	rockThree->SetModelScale(Vector3(20.0f, 20.0f, 20.0f));

	SceneNode* rockFour = new SceneNode();
	spider->AddChild(rockFour);
	rockFour->SetMesh(Mesh::LoadFromMeshFile("Rock_05.msh"));
	material = new MeshMaterial("Rock_05.mat");
	rockFour->SetSubTextures(LoadSubTextures(rockFour, material));
	rockFour->SetTransform(Matrix4::Translation(Vector3(400.0f, -130.0f, 300.0f)));
	rockFour->SetModelScale(Vector3(25.0f, 20.0f, 20.0f));

	SceneNode* rockFive = new SceneNode();
	spider->AddChild(rockFive);
	rockFive->SetMesh(Mesh::LoadFromMeshFile("Rock_06.msh"));
	material = new MeshMaterial("Rock_06.mat");
	rockFive->SetSubTextures(LoadSubTextures(rockFive, material));
	rockFive->SetTransform(Matrix4::Translation(Vector3(830.0f, -130.0f, -30.0f)));
	rockFive->SetModelScale(Vector3(20.0f, 20.0f, 20.0f));

	pointLights = new Light[LIGHT_NUM];
	pointLights[0] = Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f);
	pointLights[1] = Light((heightmapSize * Vector3(0.5f, 0.5f, 0.5f)) + Vector3(-3600.0f, 0.0f, 3750.0f), Vector4(1, 0, 0, 1), heightmapSize.x * 0.2f);

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &processFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1
	};

	GenerateScreenTexture(bufferDepthTex, true); // Generate scene depth texture
	GenerateScreenTexture(bufferColourTex[0]);
	GenerateScreenTexture(bufferColourTex[1]);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightrockTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(shadowTex, false, true);

	// Attach textures to respective FBO
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightrockTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//camera->SetAutomated(true);
	currentFrame = 0;
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;
}

Renderer::~Renderer(void) {
	delete sceneShader;
	delete shadowShader;
	
	delete pointlightShader;
	delete reflectShader;
	delete skyboxShader;
	delete combineShader;

	delete root;
	delete heightMap;
	delete camera;
	delete sphere;
	delete quad;
	delete[] pointLights;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightrockTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &shadowTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth, bool shadow) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint format = depth ? GL_DEPTH_COMPONENT : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	if (!shadow) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float dt) {
	if (dt > 2.0f)
		dt = 0.0f;

	camera->UpdateCamera(dt);

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	std::cout << camera->GetAutomated() << "\n";
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;

	framerate = 1.0 / dt;

	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % animation->GetFrameCount();
		frameTime += 1.0f / animation->GetFrameRate();
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0)) {
		camera->SetAutomated(!camera->GetAutomated());
		if (camera->GetAutomated()) {
			camera->SetPosition(Vector3(heightMap->GetHeightMapSize().x * 0.5f, 500.0f, heightMap->GetHeightMapSize().z * 0.5f));
			camera->SetPitch(0.0f);
			camera->SetYaw(0.0f);
			camera->SetRoll(0.0f);
			camera->SetTimeElapsed(0.0f);
		}
	}

	root->Update(dt);
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	FillBuffers();
	//DrawShadows();
	DrawPointLights();
	DrawPostProcessing();
	CombineBuffers();
}

bool Renderer::LoadTextures() {
	heightMap = new HeightMap(TEXTUREDIR"1island.PNG");
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.JPG", TEXTUREDIR"rusted_east.JPG",
		TEXTUREDIR"rusted_up.JPG", TEXTUREDIR"rusted_down.JPG",
		TEXTUREDIR"rusted_south.JPG", TEXTUREDIR"rusted_north.JPG",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	rockTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"Grass.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassBump = SOIL_load_OGL_texture(TEXTUREDIR"GrassN.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sandTex = SOIL_load_OGL_texture(TEXTUREDIR"Sand.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sandBump = SOIL_load_OGL_texture(TEXTUREDIR"SandN.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	snowTex = SOIL_load_OGL_texture(TEXTUREDIR"Snow.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	snowBump = SOIL_load_OGL_texture(TEXTUREDIR"SnowN.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	ruinedTowerTex = SOIL_load_OGL_texture(TEXTUREDIR"/RuinedTower/RuinedTower.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	ruinedTowerBump = SOIL_load_OGL_texture(TEXTUREDIR"/RuinedTower/RuinedTower_normals.BMP", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	towerRoofTex = SOIL_load_OGL_texture(TEXTUREDIR"/RoundTower/Roof.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	towerRoofBump = SOIL_load_OGL_texture(TEXTUREDIR"/RoundTower/Roof_bump.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	towerWallsTex = SOIL_load_OGL_texture(TEXTUREDIR"/RoundTower/Walls.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	towerWallsBump = SOIL_load_OGL_texture(TEXTUREDIR"/RoundTower/Walls_bump.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	towerWoodTex = SOIL_load_OGL_texture(TEXTUREDIR"/RoundTower/weathwood.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	towerWoodBump = SOIL_load_OGL_texture(TEXTUREDIR"/RoundTower/weathwood_bump.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	shipTex = SOIL_load_OGL_texture(TEXTUREDIR"/BrigSloop/texture_1800early_brigSloop_albedo.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockOneTex = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_02/T_Rock_02_D.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockOneBump = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_02/T_Rock_02_N.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockTwoTex = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_03/T_Rock_03_D.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockTwoBump = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_03/T_Rock_03_N.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockThreeTex = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_04/T_Rock_04_D.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockThreeBump = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_04/T_Rock_04_N.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockFourTex = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_05/T_Rock_05_D.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockFourBump = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_05/T_Rock_05_N.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockFiveTex = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_06/T_Rock_06_D.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockFiveBump = SOIL_load_OGL_texture(TEXTUREDIR"/Rocks/Rock_06/T_Rock_06_N.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	spiderTex = SOIL_load_OGL_texture(TEXTUREDIR"/Spider/spidertex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	if (!heightMap || !cubeMap || !rockTex || !rockBump || !grassTex || !grassBump || !sandTex || !sandBump || !snowTex || !snowBump || !waterTex
		|| !ruinedTowerTex || !ruinedTowerBump || !shipTex || !towerRoofTex || !towerRoofBump || !towerWallsTex || !towerWallsBump || !towerWoodTex || !towerWoodBump
		|| !rockOneTex || !rockOneBump || !rockTwoTex || !rockTwoBump || !rockThreeTex || !rockThreeBump
		|| !rockFourTex || !rockFourBump || !rockFiveTex || !rockFiveBump || !spiderTex) {
		return false;
	}
	return true;
}

bool Renderer::LoadShaders() {
	sceneShader = new Shader("BumpVertex.glsl", "BufferFragment.glsl");
	nodeShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	shadowShader = new Shader("ShadowVertex.glsl", "ShadowFragment.glsl");
	shadowSceneShader = new Shader("ShadowSceneVertex.glsl", "ShadowSceneFragment.glsl");
	pointlightShader = new Shader("PointLightVertex.glsl", "PointLightFragment.glsl");
	processShader = new Shader("ProcessVertex.glsl", "ProcessFrag.glsl");
	combineShader = new Shader("CombineVertex.glsl", "CombineFragment.glsl");
	reflectShader = new Shader("ReflectVertex.glsl", "ReflectFragment.glsl");
	skyboxShader = new Shader("SkyboxVertex.glsl", "SkyboxFragment.glsl");
	animationShader = new Shader("SkinningVertex.glsl", "TexturedFragment.glsl");

	if (!sceneShader->LoadSuccess() || !shadowSceneShader->LoadSuccess() || !shadowShader->LoadSuccess() ||
		!pointlightShader->LoadSuccess() || !processShader->LoadSuccess() || !combineShader->LoadSuccess() ||
		!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !animationShader->LoadSuccess()) {
		return false;
	}
	return true;
}

vector<GLuint> Renderer::LoadSubTextures(SceneNode* node, MeshMaterial* material) {
	matTextures.clear();
	for (int i = 0; i < node->GetMesh()->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);

		node->SetSubTextures(matTextures);
	}
	return matTextures;
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}   

void Renderer::DrawHeightMap() {
	BindShader(sceneShader);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "rockTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "rockBump"), 1);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "grassTex"), 2);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "grassBump"), 3);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "sandTex"), 4);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "sandBump"), 5);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "snowTex"), 6);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "snowBump"), 7);

	glUniform1f(glGetUniformLocation(sceneShader->GetProgram(), "worldHeight"), heightMap->GetHeightMapSize().y);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rockTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rockBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grassBump);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, sandTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, sandBump);
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, snowTex);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, snowBump);

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightMapSize();

	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(-1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();

	quad->Draw();
}

void Renderer::DrawPostProcessing() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	if (usingBlur) {
		DrawBlur();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawBlur() {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(processShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 0);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();

		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}
}

void Renderer::ToggleFog() {
	usingFog = !usingFog;
}

void Renderer::ToggleBlur() {
	usingBlur = !usingBlur;
}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawHeightMap();
	DrawWater();

	BuildNodeLists(root);
	SortNodeLists();
	DrawNodes();
	ClearNodeLists();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadows() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(pointLights[0].GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix;

	UpdateShaderMatrices();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightrockTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	
	quad->Draw();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
		BuildNodeLists(*i);
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* node) {
	BindShader(nodeShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(nodeShader->GetProgram(), "diffuseTex"), 0);

	if (node->GetMesh()) {
		Matrix4 model = node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale());
		if (node->GetHasAnim()) {
			BindShader(animationShader);
			UpdateShaderMatrices();

			glUniform1i(glGetUniformLocation(animationShader->GetProgram(), "diffuseTex"), 0);
			glUniformMatrix4fv(glGetUniformLocation(animationShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(animationShader->GetProgram(), "nodeColour"), 1, (float*)&node->GetColour());

			vector<Matrix4> frameMatrices;

			const Matrix4* invBindPose = node->GetMesh()->GetInverseBindPose();
			const Matrix4* frameData = animation->GetJointData(currentFrame);

			for (unsigned int i = 0; i < node->GetMesh()->GetJointCount(); ++i)
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);

			int j = glGetUniformLocation(animationShader->GetProgram(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

			texture = node->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			node->Draw(*this);

			return;
		}

		BindShader(nodeShader);
		glUniformMatrix4fv(glGetUniformLocation(nodeShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(nodeShader->GetProgram(), "nodeColour"), 1, (float*)&node->GetColour());

		if (node->GetMesh()->GetSubMeshCount() > 1) {
			for (int i = 0; i < node->GetMesh()->GetSubMeshCount(); i++) {
				vector<GLuint> subTextures = node->GetSubTextures();

				node->SetTexture(subTextures[i]);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, subTextures[i]);
				glUniform1i(glGetUniformLocation(nodeShader->GetProgram(), "useTexture"), node->GetTexture());
				node->GetMesh()->DrawSubMesh(i);
			}
		} else {
			texture = node->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(nodeShader->GetProgram(), "useTexture"), node->GetTexture());
			node->Draw(*this);
		}
		
		
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}