/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere 
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f/180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0)*dt);
			z = R * sin((0)*dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh *cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res,float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);
	
	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res+2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh *cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh *piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(1920, 1280);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa
	
	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(25, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();
	
	

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 50.0f, 50.0f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
    glm::mat4 projection = glm::perspective(glm::radians(160.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);
	
	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía
	glm::mat4 pivotCabina;
	glm::mat4 pivotBase;
	glm::mat4 pataDL;
	glm::mat4 rodillaDL;
	glm::mat4 pataDIL;
	glm::mat4 rodillaDIL;
	glm::mat4 pataTD;
	glm::mat4 rodillaTD;
	glm::mat4 rodillaTID;
	glm::mat4 hocicoBase;
	glm::mat4 colaBase;
	glm::mat4 colaSegmento;
	glm::vec3 color = glm::vec3(0.0f,0.0f,0.0f); //inicializar Color para enviar a variable Uniform;
	// ARTICULACIONES DEL ZORRO
	//incialización articulaciones del zorro
	float pata1_sup = 0;
	float pata1_inf = 0;

	float pata2_sup = 0;
	float pata2_inf = 0;

	float pata3_sup = 0;
	float pata3_inf = 0;

	float pata4_sup = 0;
	float pata4_inf = 0;

	float cola = 0;

	float oreja_izq = 0;
	float oreja_der = 0;


	
	while (!mainWindow.getShouldClose())
	{
		
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		//Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		// Creando el brazo de una grúa
		//articulacion1 hasta articulación5 sólo son puntos de rotación o articulación, en este caso no dibujaremos esferas que los representen
				
		//para reiniciar la matriz de modelo con valor de la matriz identidad
		//INICIA GRUA
		/*
		model = glm::mat4(1.0);

		//cabina
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		pivotCabina = model;
		model = glm::scale(model, glm::vec3(5.5f, 3.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.5f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();
		model = pivotCabina;

		// BASE 
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		pivotBase = model;
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.8f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();
		model = pivotBase;

	   // LLANTAS
       // LLANTA 1
		model = glm::translate(pivotBase, glm::vec3(2.5f, -1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		// LLANTA 2
		model = glm::translate(pivotBase, glm::vec3(-2.5f, -1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// LLANTA 3
		model = glm::translate(pivotBase, glm::vec3(2.5f, -1.5f, -1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// LLANTA 4
		model = glm::translate(pivotBase, glm::vec3(-2.5f, -1.5f, -1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// regresar al pivote de la cabina
		model = pivotCabina;

		// SE EMPIEZA EL DIBUJO DEL BRAZO
		//articulación 1
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		//rotación alrededor de la articulación que une con la cabina
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		
		//primer brazo que conecta con la cabina
		// //Traslación inicial para posicionar en -Z a los objetos
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		//otras transformaciones para el objeto
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 0.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));		
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular
		//meshList[2]->RenderMeshGeometry(); //dibuja las figuras geométricas cilindro y cono

		//para descartar la escala que no quiero heredar se carga la información de la matrix auxiliar
		model = modelaux;
		//articulación 2
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		model = modelaux;
		//segundo brazo
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular
		model = modelaux;

		//articulación 3 extremo derecho del segundo brazo
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = modelaux;

		// tercer brazo
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(4.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelaux;

		// articulacion 5
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;

		// cesta
		model = glm::scale(model, glm::vec3(2.3f, 3.3f, 1.3f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Crear instancias para completar el brazo y  la cabina. Imporante considerar que la cabina es el nodo padre. 
		//La cabina y el brazo deben de estar unidos a la cabina 
		*/
		//FIN GRUA


		//INICIO ZORRO
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//creación de zorro en 3D	
		pata1_sup = mainWindow.getarticulacion7();
		pata1_inf = mainWindow.getarticulacion8();
		pata2_sup = mainWindow.getarticulacion9();
		pata2_inf = mainWindow.getarticulacion10();
		pata3_sup = mainWindow.getarticulacion11();
		pata3_inf = mainWindow.getarticulacion12();
		pata4_sup = mainWindow.getarticulacion13();
		pata4_inf = mainWindow.getarticulacion14();
		cola = mainWindow.getarticulacion15();
		
		// CUERPO DEL ZORRO

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, -6.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 6.0f));
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, -6.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(6.0f, 2.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// PATA DELANTERA DERECHA
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.8f, -1.2f, 0.7f));
		pataDL = model;

		// esfera unión torso-pierna
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// pierna superior
		model = pataDL;
		model = glm::rotate(model, glm::radians(pata1_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.2f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// articulación rodilla
		model = pataDL;
		model = glm::rotate(model, glm::radians(pata1_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		rodillaDL = model;
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// pata inferior
		model = rodillaDL;
		model = glm::rotate(model, glm::radians(pata1_inf), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.2f, -0.3f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();
		
		// PATA DELANTERA IZQUIIERDA
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.8f, -1.2f, -0.7f));
		pataDIL = model;
		// esfera unión torso-pierna
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		// pierna superior
		model = pataDIL;
		model = glm::rotate(model, glm::radians(pata2_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.2f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();
		// articulación rodilla
		model = pataDIL;
		model = glm::rotate(model, glm::radians(pata2_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		rodillaDIL = model;
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		// pata inferior
		model = rodillaDIL;
		model = glm::rotate(model, glm::radians(pata2_inf), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.2f, -0.3f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// PATA TRASERA DERECHA
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.8f, -1.2f, 0.7f));
		pataTD = model;

		// esfera unión torso-pierna
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// pierna superior
		model = pataTD;
		model = glm::rotate(model, glm::radians(pata3_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.2f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// articulación rodilla
		model = pataTD;
		model = glm::rotate(model, glm::radians(pata3_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		rodillaTD = model;
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// pata inferior
		model = rodillaTD;
		model = glm::rotate(model, glm::radians(pata3_inf), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.2f, -0.3f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// PATA TRASERA IZQUIERDA
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.8f, -1.2f, -0.7f));
		glm::mat4 pataTID = model;

		// esfera unión torso-pierna
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// pierna superior
		model = pataTID;
		model = glm::rotate(model, glm::radians(pata4_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.2f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// articulación rodilla
		model = pataTID;
		model = glm::rotate(model, glm::radians(pata4_sup), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		rodillaTID = model;
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// pata inferior
		model = rodillaTID;
		model = glm::rotate(model, glm::radians(pata4_inf), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(0.2f, -0.3f, 0.0f));
		model = glm::scale(model, glm::vec3(0.75f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// CABEZA
		
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.0f, 1.8f, 1.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();
		
		// HOCICO (CONO)

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.3f, -0.2f, 0.0f));
		hocicoBase = model;
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.2f));
		color = glm::vec3(0.4f, 0.2f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMeshGeometry();

		// OJO IZQUIERDO

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.89f, 0.4f, 0.7f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.1f));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// OJO DERECHO

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.89f, 0.4f, -0.7f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.1f));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// OREJA IZQUIERDA

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.65f, 1.3f, 0.5f));
		model = glm::rotate(model, glm::radians(oreja_izq), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(0.5f, 0.8f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.6f, 0.3f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();
		
		//oreja derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.65f, 1.3f, -0.5f));
		model = glm::rotate(model, glm::radians(oreja_der), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(0.5f, 0.8f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.6f, 0.3f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		// COLA

		model = modelaux;
		model = glm::translate(model, glm::vec3(-6.7f, -0.1f, 0.0f));
		colaBase = model;

		// ESFERA UNION COLA-TORSO
		model = colaBase;
		model = glm::rotate(model, glm::radians(cola), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// COLA PARTE 1 (NARANJA)

		model = colaBase;
		model = glm::rotate(model, glm::radians(cola), glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(-1.2f, 0.0f, 0.0f));
		colaSegmento = model;
		model = glm::scale(model, glm::vec3(2.5f, 1.15f, 1.15f));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		// COLA PUNTA (CAFE)

		model = colaSegmento;
		//model = glm::rotate(model, glm::radians(cola), glm::vec3(0, 0, 1));
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.15f, 1.15f));
		color = glm::vec3(0.4f, 0.2f, 0.1f); // café
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();
		//FIN ZORRO
		glUseProgram(0);
		mainWindow.swapBuffers();
		
		
	}
	return 0;
}

	
		