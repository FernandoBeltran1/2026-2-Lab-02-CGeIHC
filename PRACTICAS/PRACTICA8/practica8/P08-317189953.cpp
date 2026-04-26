/*
Práctica 8: Iluminación 2
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

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture vidrioTexture;
Texture aguaTexture;



Model Blackhawk_M;
Model Lampara;
Model Pez;


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


//función de calculo de normales por promedio de vértices 
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
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}

void CrearPecera()
{
	GLfloat pecera_vertices[] =
	{
		//frente
		-1.0f, -1.0f,  1.0f,   0.0f, 0.0f,   0.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,   1.0f, 0.0f,   0.0f,  0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,   1.0f, 1.0f,   0.0f,  0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,   0.0f, 1.0f,   0.0f,  0.0f, 1.0f,

		//atrás
		-1.0f, -1.0f, -1.0f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,

		//izquierda
		-1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		-1.0f, -1.0f,  1.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f,  1.0f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,

		//derecha 
		 1.0f, -1.0f,  1.0f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f, -1.0f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		 1.0f,  1.0f, -1.0f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,

		 //fondo
		 -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
		  1.0f, -1.0f, -1.0f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
		  1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
		 -1.0f, -1.0f,  1.0f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
	};

	unsigned int pecera_indices[] =
	{
		0, 2, 1, 2, 0, 3,
		4, 5, 6, 6, 7, 4,
		8, 10, 9, 10, 8, 11,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
	};

	Mesh* pecera = new Mesh();
	pecera->CreateMesh(pecera_vertices, pecera_indices, 160, 30);
	meshList.push_back(pecera); 
}

void CrearSupAgua()
{
	GLfloat agua_vertices[] =
	{
		//  x      y      z     S     T      NX    NY    NZ
		-1.0f,  1.0f, -1.0f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, 
		-1.0f,  1.0f,  1.0f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, 
	};

	unsigned int agua_indices[] =
	{
		0, 2, 1,
		2, 0, 3,
	};

	Mesh* supAgua = new Mesh();
	supAgua->CreateMesh(agua_vertices, agua_indices, 32, 6);
	meshList.push_back(supAgua);
}

float ActArt(float articulacion, bool bandera)
{
	if (!bandera) {
		articulacion += 0.06f * deltaTime;
	}
	else {
		articulacion -= 0.06f * deltaTime;
	}
	return articulacion;
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CreateShaders();
	CrearPecera();
	CrearSupAgua();


	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

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
	aguaTexture = Texture("Textures/agua.png");
	aguaTexture.LoadTextureA();
	vidrioTexture = Texture("Textures/vidrio.png");
	vidrioTexture.LoadTextureA();


	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Lampara = Model();
	Lampara.LoadModel("Models/lampara.obj");
	Pez = Model();
	Pez.LoadModel("Models/pez_abisal.obj");



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
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.5f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//lampara
	pointLightCount++;
	//cubo pez
	pointLightCount++;


	unsigned int spotLightCount = 0;
	/*//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
		*/
	spotLightCount++;
	/*
	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
		*/
	spotLightCount++;
	
	//se crean mas luces puntuales y spotlight 
	//spotlight coche faro1
	spotLightCount++;

	//spotlight coche faro2
	spotLightCount++;

	// spotligh cubo pez
	spotLightCount++;
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//float helic = 0.0f;
	glm::vec4 lamparaPos = glm::vec4(0.0f,0.0f,0.0f,0.0f);
	glm::vec4 cochePos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	float spot2Int = 0.0f;
	float spot3Int = 0.0f;
	
	float helic = 0.0f;
	glm::vec4 helicPos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	GLfloat now = glfwGetTime();

	glm::vec3 cuboPezPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec4 pointCuboPezPos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);

	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		
		bool* keys = mainWindow.getsKeys(); //deteccion de teclas
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

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

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//pointlight de la lampara
		pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
			0.0f, mainWindow.getluzlampara(),
			lamparaPos.x, lamparaPos.y+2.0f, lamparaPos.z,
			0.3f, 0.2f, 0.1f);

		//pointlight del cubo pez

		pointLights[2] = PointLight(0.0f, 0.0f, 1.0f,
			0.0f,mainWindow.getpointCuboPez(),
			pointCuboPezPos.x - 0.2f, pointCuboPezPos.y+0.3f, pointCuboPezPos.z,
			0.3f, 0.2f, 0.1f);
			

		//spotlight helicoptero
		if (mainWindow.getbanderacoche()) {
			spot2Int = 0.0f;
			spot3Int = 2.0f;

		}
		else { 
			spot2Int = 2.0f;
			spot3Int = 0.0f;

		}
		
		spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f,
			0.0f, spot2Int,
			0.0f, 0.0f, 0.0f,
			-0.5f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			25.0f);
		
		spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f,
			0.0f, spot3Int,
			0.0f, 0.0f, 0.0f,
			0.5f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			25.0f);

		//spotlight para el cubo pez
		spotLights[4] = SpotLight(1.0f, 0.45f, 0.0f,          
			0.0f, 2.0f,                 
			0.0f, 0.0f,  0.0f,      
			mainWindow.getSpotPezDirX(), mainWindow.getSpotPezDirY(), mainWindow.getSpotPezDirZ(),                
			1.0f, 0.0f, 0.0f,           
			25.0f                        
		);
	

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		//spotLights[1].SetPos(poscoche + glm::vec(x, y, cofre));
		spotLights[2].SetPos(glm::vec4(helicPos));
		spotLights[3].SetPos(glm::vec4(helicPos));
		spotLights[4].SetPos(glm::vec4(pointCuboPezPos.x - 0.2f, pointCuboPezPos.y + 0.3f, 
			pointCuboPezPos.z, pointCuboPezPos.w));
		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//2.- Apagar con teclado la luz (pointlight) de su lámpara creada para el reporte de la práctica 7.
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 6.0));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();
		lamparaPos = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		//renderizado del helicoptero
		
		helic = ActArt(helic, mainWindow.getbanderacoche());

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, helic, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		helicPos = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		//para el pez
		
		if (keys[GLFW_KEY_I])        
		{
			cuboPezPos.y += 0.06f * deltaTime;
			cuboPezPos.x -= 0.06f * deltaTime;
		}
		if (keys[GLFW_KEY_K])       
		{
			cuboPezPos.y -= 0.06f * deltaTime;
			cuboPezPos.x += 0.06f * deltaTime;
		}

		// Clamp al interior de la pecera
		cuboPezPos.x = glm::clamp(cuboPezPos.x, 10.0f - 0.5f, 10.0f + 0.5f);
		cuboPezPos.y = glm::clamp(cuboPezPos.y, 0.6f - 0.5f, 0.6f + 0.5f);
		cuboPezPos.z = glm::clamp(cuboPezPos.z, 9.0f - 1.1f, 9.0f + 1.1f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, 0.6f, 1.0f));
		model = glm::translate(model, cuboPezPos);
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pez.RenderModel();
		pointCuboPezPos = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		
		//blending: transparencia o traslucidez
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, 0.6f, 9.0f));
		model = glm::scale(model, glm::vec3(4.0f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//agua
		aguaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();
		vidrioTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
