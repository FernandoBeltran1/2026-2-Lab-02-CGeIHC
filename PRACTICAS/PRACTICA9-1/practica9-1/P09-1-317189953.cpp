/*
Animación:
- Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
-Compleja: Por medio de funciones y algoritmos.
-Textura Animada
*/

//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//waypoints pista
static const std::vector<glm::vec3> trackWaypoints = {
	//  x        y       z      
	{ -1.071f,  0.0f,  0.0f }, 
	{ -1.3008f,  0.21793f,  -11.092f }, 
	{ -2.9616f,  0.21793f,  -29.116f }, 
	{  -8.3203f,  0.53688f,   -46.275f }, 
	{ -8.409f,   2.0923f,   -59.907f }, 
	{8.6984f,   6.3387f,   -75.216f }, 
	{7.9631f,   12.286f,   -90.584f }, 
	{4.2728f,   18.934f,   -104.41f }, 
	{4.2728f,   22.614f,  -112.98f }, 
	{5.5091f,  25.493f,  -121.63f }, 
	{10.832f,  28.404f,  -143.55f }, 
	{15.361f, 28.404f, -157.99f},
	{20.153f, 27.424f, -172.1f},
};

//  Interpolación entre waypoints

glm::vec3 CatmullRom(const glm::vec3& p0, const glm::vec3& p1,
	const glm::vec3& p2, const glm::vec3& p3, float t)
{
	float t2 = t * t, t3 = t2 * t;
	return 0.5f * (
		(2.0f * p1) +
		(-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
		);
}
static glm::vec3 SamplePath(float t)
{
	int n = (int)trackWaypoints.size();
	float s = t * (n - 1);
	int i1 = glm::clamp((int)s, 0, n - 1);
	int i0 = glm::clamp(i1 - 1, 0, n - 1);
	int i2 = glm::clamp(i1 + 1, 0, n - 1);
	int i3 = glm::clamp(i1 + 2, 0, n - 1);
	float lt = s - (float)i1;
	return CatmullRom(trackWaypoints[i0], trackWaypoints[i1],
		trackWaypoints[i2], trackWaypoints[i3], lt);
}
//  Obtiene posición y tangente del coche

void GetPathPosAndTangent(float t, glm::vec3& outPos, glm::vec3& outTangent)
{
	outPos = SamplePath(t);

	const float eps = 0.001f;
	glm::vec3 posA = SamplePath(glm::clamp(t - eps, 0.0f, 1.0f));
	glm::vec3 posB = SamplePath(glm::clamp(t + eps, 0.0f, 1.0f));
	glm::vec3 diff = posB - posA;

	if (glm::length(diff) < 1e-6f)
		outTangent = glm::vec3(0.0f, 0.0f, 1.0f);
	else
		outTangent = glm::normalize(diff);
}

//maquina de estados para la nave
enum EstadoNave { 
	nave_volando, 
	nave_aterrizando, 
	nave_posada
};


//variables para animación del coche

float  tCoche = 0.0f;   
const float velTCoche = 0.0004f;  
bool   cochePausado = false;  
bool   teclaRAnterior = false;  
float  rotllanta = 0.0f;  

//variables para la nave

EstadoNave  estadoNave = nave_volando;
float       tNave = 1.0f;       
const float velTNave = 0.0004f;
const float alturaVuelo = 18.0f;
float       aterrizajeY = 0.0f;
float       rotHelice = 0.0f;
const float velHelice = 360.0f;     
float       aleteoAla = 0.0f;

float tiempoTotal = 0.0f;




float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;



Model Kitt_M;
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Aeolipile_base_M;
Model Aeolipile_M;
Model llanta_sonic;
Model cofre;
Model coche_sonic;
Model ala2;
Model nave1;
Model helice;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	llanta_sonic = Model();
	llanta_sonic.LoadModel("Models/llanta_optimizada.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	Ala_M = Model();
	Ala_M.LoadModel("Models/ala.obj");
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile.obj");
	coche_sonic = Model();
	coche_sonic.LoadModel("Models/coche_sonic.obj");
	cofre = Model();
	cofre.LoadModel("Models/cofre.obj");
	llanta_sonic = Model();
	llanta_sonic.LoadModel("Models/llanta_sonic.obj");
	ala2 = Model();
	ala2.LoadModel("Models/ala2.obj");
	nave1 = Model();
	nave1.LoadModel("Models/nave1.obj");
	helice = Model();
	helice.LoadModel("Models/helice.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//spotlight coche faro1
	spotLightCount++;

	//spotlight coche faro2
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);


	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::mat4 modelNave(1.0);
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 down = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 frontOffset= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posHL = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posHR = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 headlightDir = glm::vec3(0.0f, 0.0f, 0.0f);



	glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
	glm::vec3 carRight = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 carPos, carTangent;
	
	float yawCoche = 0.0f;
	float pitchCoche = 0.0f;


	float aleteoAla = 0.0f;
	float rotYNave = 0.0f;
	float bobY = 0.0f;
	float posZNave = 0.0f;
	float yawNave = 0.0f;
	const float eps = 0.001f;
	const float altObj = 3.0f;
	const float velDesc = 4.0f;
	

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		tiempoTotal += deltaTime;
		angulovaria += 0.5f * deltaTime;

		//animacion coche
		bool* keys = mainWindow.getsKeys();
		bool teclaRActual = keys[GLFW_KEY_R];
		if (teclaRActual && !teclaRAnterior) {
			tCoche = 0.0f;
			cochePausado = false;
			rotllanta = 0.0f;
		}
		teclaRAnterior = teclaRActual;

		if (!cochePausado) {
			tCoche += velTCoche * deltaTime;
			if (tCoche >= 1.0f) {
				tCoche = 1.0f;
				cochePausado = true;
			}
			rotllanta += 360.0f * deltaTime;
		}

		GetPathPosAndTangent(tCoche, carPos, carTangent);

		
		yawCoche = atan2f(carTangent.x, carTangent.z) * (180.0f / 3.14159265f);
		pitchCoche = asinf(glm::clamp(carTangent.y, -1.0f, 1.0f)) * (180.0f / 3.14159265f);
		carRight = glm::normalize(glm::cross(worldUp, carTangent));
		if (glm::length(carRight) < 0.001f)
			carRight = glm::vec3(1.0f, 0.0f, 0.0f); 


		//animacion nave
		aleteoAla = 35.0f * sinf(tiempoTotal * 3.5f); 

		glm::vec3 navePos(0.0f), naveTangent(0, 0, 1);

		switch (estadoNave)
		{
		case nave_volando:
		{
			tNave -= velTNave * deltaTime;
			if (tNave <= 0.0f) {
				tNave = 0.0f; aterrizajeY = alturaVuelo; estadoNave = nave_aterrizando;
			}
			navePos = SamplePath(tNave);
			navePos.y += alturaVuelo;

			glm::vec3 diff = SamplePath(glm::clamp(tNave - eps, 0.0f, 1.0f))
				- SamplePath(glm::clamp(tNave + eps, 0.0f, 1.0f));
			naveTangent = (glm::length(diff) > 1e-6f) ? glm::normalize(diff) : glm::vec3(0, 0, -1);

			rotHelice += velHelice * deltaTime;
			if (rotHelice > 360.0f) rotHelice -= 360.0f;
			break;
		}
		case nave_aterrizando:
		{
			
			aterrizajeY -= velDesc * deltaTime;
			if (aterrizajeY <= altObj) { aterrizajeY = altObj; estadoNave = nave_posada; }

			navePos = SamplePath(0.0f);
			navePos.y += aterrizajeY;
			glm::vec3 diff = SamplePath(glm::clamp(0.0f - eps, 0.0f, 1.0f))
				- SamplePath(glm::clamp(0.0f + eps, 0.0f, 1.0f));
			naveTangent = (glm::length(diff) > 1e-6f) ? glm::normalize(diff) : glm::vec3(0, 0, -1);

			rotHelice += velHelice * deltaTime;
			if (rotHelice > 360.0f) rotHelice -= 360.0f;
			break;
		}
		case nave_posada:
		{
			navePos = SamplePath(0.0f);
			navePos.y += 3.0f;

			glm::vec3 diff = SamplePath(glm::clamp(0.0f - eps, 0.0f, 1.0f))
				- SamplePath(glm::clamp(0.0f + eps, 0.0f, 1.0f));
			naveTangent = (glm::length(diff) > 1e-6f) ? glm::normalize(diff) : glm::vec3(0, 0, -1);

			rotHelice -= velHelice * 0.5f * deltaTime;
			if (rotHelice < 0.0f) rotHelice = 0.0f;
			break;
		}
		}

		yawNave = atan2f(naveTangent.x, naveTangent.z) * (180.0f / 3.14159265f);



		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); 

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		forward = carTangent;
		down = glm::vec3(0.0f, -1.0f, 0.0f);
		
		headlightDir = glm::normalize(forward * 0.7f + down * 0.3f);

		frontOffset = forward * 1.5f + glm::vec3(0.0f, 0.6f, 0.0f);

		posHL = carPos + frontOffset + carRight * 0.5f;   
		posHR = carPos + frontOffset + carRight * -0.5f; 

		
		spotLights[2] = SpotLight(
			1.0f, 1.0f, 0.95f,    
			0.0f, 5.0f,            
			posHL.x, posHL.y, posHL.z,
			headlightDir.x, headlightDir.y, headlightDir.z,
			0.3f, 0.05f, 0.003f,  
			40.0f);                  

		spotLights[3] = SpotLight(
			1.0f, 1.0f, 0.95f,
			0.0f, 5.0f,
			posHR.x, posHR.y, posHR.z,
			headlightDir.x, headlightDir.y, headlightDir.z,
			0.3f, 0.05f, 0.003f,
			40.0f);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Pista
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista_M.RenderModel();


		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, carPos);
		model = glm::rotate(model, yawCoche * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -pitchCoche * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		coche_sonic.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8f, 0.1f, 1.1f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llanta_sonic.RenderModel();

		
		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8f, 0.1f, -0.8f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llanta_sonic.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.8f, 0.1f, 1.1f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	   llanta_sonic.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.8f, 0.1f, -0.8f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llanta_sonic.RenderModel();

		//cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.7f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cofre.RenderModel();

		//nave

		glm::mat4 modelNave = glm::translate(glm::mat4(1.0), navePos);
		modelNave = glm::rotate(modelNave, yawNave * toRadians, glm::vec3(0, 1, 0));

		// Cuerpo principal
		model = glm::translate(model, navePos);
		model = glm::rotate(modelNave, 270.0f * toRadians, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nave1.RenderModel();

		// Ala izquierda
		model = glm::translate(modelNave, glm::vec3(-0.3f, 0.0f, 0.25f));
		model = glm::rotate(model, 75.0f * toRadians, glm::vec3(0, 1, 0));
		model = glm::rotate(model, aleteoAla * toRadians, glm::vec3(1, 0, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		// Ala derecha 
		model = glm::translate(modelNave, glm::vec3(0.3f, 0.0f, 0.25f));
		model = glm::rotate(model, -75.0f * toRadians, glm::vec3(0, 1, 0));
		model = glm::rotate(model, -aleteoAla * toRadians, glm::vec3(1, 0, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ala2.RenderModel();

		// Hélice izquierda
		model = glm::translate(modelNave, glm::vec3(-0.36f, -0.3f, 0.36f));
		model = glm::rotate(model, rotHelice * toRadians, glm::vec3(0, 0, 1));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		helice.RenderModel();

		// Hélice derecha
		model = glm::translate(modelNave, glm::vec3(0.35f, -0.3f, 0.4f));
		model = glm::rotate(model, -rotHelice * toRadians, glm::vec3(0, 0, 1));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		helice.RenderModel();


		//AEOLIPILE
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Aeolipile_base_M.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Aeolipile_M.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[4]->RenderMesh();

		//plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[5]->RenderMesh();

		//número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[6]->RenderMesh();

		for (int i = 1; i < 4; i++)
		{
			//números 2-4
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			//meshList[6]->RenderMesh();

		}

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			//meshList[6]->RenderMesh();
		}


		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		*/
		toffsetnumerocambiau += 0.25;
		if (toffsetnumerocambiau > 1.0)
			toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[6]->RenderMesh();

		//cambiar automáticamente entre textura número 1 y número 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		//if
		//Numero1Texture.UseTexture();
		//Numero2Texture.UseTexture();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[5]->RenderMesh();


		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}