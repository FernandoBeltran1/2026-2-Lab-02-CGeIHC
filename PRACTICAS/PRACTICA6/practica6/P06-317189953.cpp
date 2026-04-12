/*
Práctica 6: Texturizado
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

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;
Texture octaedroTexture;

Model Kitt_M;
Model Llanta_M;
//Model Dado_M;
Model Llanta_S;
Model cocheO;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";




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
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

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

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearOctaedro()
{
	// Normal de cada cara 
	const float n = 0.5774f;

	//  x        y        z         S        T      NX    NY    NZ
	GLfloat oct_vertices[] = {
	// Cara 1 (frente superior)
		 0.0f,  0.5f,  0.0f,    0.50f,  0.73f,     n,   n,   n,  
		 0.0f,  0.0f,  0.5f,    0.275f,    0.51f,     n,   n,   n,  
		 0.5f,  0.0f,  0.0f,    0.71f,   0.51f,     n,   n,   n,  

	// Cara 2 (frente derecha)
		 0.0f,  0.5f,  0.0f,    0.54f,  0.73f,     n,   n,  -n,  
		 0.5f,  0.0f,  0.0f,    0.74f,   0.52f,     n,   n,  -n,  
		 0.0f,  0.0f, -0.5f,    0.95f,    0.74f,     n,   n,  -n,  

	// Cara 3 (superior trasera)
		 0.0f,  0.5f,  0.0f,    0.54f,  0.76f,    -n,   n,  -n,  
		 0.0f,  0.0f, -0.5f,    0.95f,    0.76f,    -n,   n,  -n,  
		-0.5f,  0.0f,  0.0f,    0.73f,   0.98f,    -n,   n,  -n,  

    // Cara 4 (frente izquierda)
		 0.0f,  0.5f,  0.0f,    0.46f,  0.74f,    -n,   n,   n,  
		-0.5f,  0.0f,  0.0f,    0.039f,   0.74f,    -n,   n,   n,  
		 0.0f,  0.0f,  0.5f,    0.25f,    0.52f,    -n,   n,   n,  

	// Cara 5 (inferior frontal)
		 0.0f, -0.5f,  0.0f,    0.51f,  0.28f,     n,  -n,   n,  
		 0.5f,  0.0f,  0.0f,    0.72f,  0.49f,     n,  -n,   n,  
		 0.0f,  0.0f,  0.5f,    0.28f,   0.49f,     n,  -n,   n,  

	// Cara 6 (6)
		 0.0f, -0.5f,  0.0f,    0.54f,  0.28f,     n,  -n,  -n,
		 0.0f,  0.0f, -0.5f,    0.73f,   0.48f,     n,  -n,  -n,
		 0.5f,  0.0f,  0.0f,    0.92f,    0.26f,     n,  -n,  -n,

	// Cara 7 (8)
		 0.0f, -0.5f,  0.0f,    0.53f,  0.23f,    -n,  -n,  -n,
		-0.5f,  0.0f,  0.0f,    0.95f,    0.23f,    -n,  -n,  -n,
		 0.0f,  0.0f, -0.5f,    0.72f,   0.038f,    -n,  -n,  -n,

	// Cara 8 (2)
		 0.0f, -0.5f,  0.0f,    0.47f,  0.27f,    -n,  -n,   n,
		 0.0f,  0.0f,  0.5f,    0.26f,   0.49f,    -n,  -n,   n,
		-0.5f,  0.0f,  0.0f,    0.039f,    0.26f,    -n,  -n,   n,
	};

	unsigned int oct_indices[] = {
		 0,  1,  2,   // Cara 1
		 3,  4,  5,   // Cara 2
		 6,  7,  8,   // Cara 3
		 9, 10, 11,   // Cara 4
		12, 13, 14,   // Cara 5
		15, 16, 17,   // Cara 6
		18, 19, 20,   // Cara 7
		21, 22, 23,   // Cara 8
	};

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(oct_vertices, oct_indices, 192, 24);
	meshList.push_back(octaedro);
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	//CrearDado();
	CreateShaders();
	CrearOctaedro();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	//dadoTexture = Texture("Textures/dado-de-emociones.tga");
	//dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();
	octaedroTexture = Texture("Textures/dado_ocho.tga");
	octaedroTexture.LoadTextureA();
	
	
	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	//Dado_M = Model();
	//Dado_M.LoadModel("Models/cubo-emociones.obj");
	Llanta_S = Model();
	Llanta_S.LoadModel("Models/llanta_sonic_texturizada.obj");
	cocheO = Model();
	cocheO.LoadModel("Models/coche_ojos.obj");

	
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
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
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();


		
		/*Reporte de práctica :
		Ejercicio 1: Crear un dado de 8 caras y texturizarlo por medio de código
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		octaedroTexture.UseTexture();
		meshList[4]->RenderMesh();

		/*
		Ejercicio 2 y 3: Importar el modelo de su coche con sus 4 llantas acomodadas
		y tener texturizadas las 4 llantas (diferenciar caucho y rin)  */

		//Coche sonic
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocheO.RenderModel();
		modelaux = model;
		

		//llanta 1
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9f, 0.2f, 0.77f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_S.RenderModel();

		//llanta4
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9f, 0.2f, -1.09f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_S.RenderModel();
	

		//llanta2
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.9f, 0.2f, -1.07f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_S.RenderModel();
		

		//llanta3
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.9f, 0.2f, 0.8f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_S.RenderModel();
		

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

//blending: transparencia o traslucidez
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		//FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		//glDisable(GL_BLEND);

