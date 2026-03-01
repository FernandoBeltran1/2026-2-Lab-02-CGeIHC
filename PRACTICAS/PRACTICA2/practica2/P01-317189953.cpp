#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glew.h>
#include <glfw3.h>
//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
layout (location =1) in vec3 inColor;               \n\
out vec3 fragColor;                                 \n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
fragColor = inColor;                                 \n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
in vec3 fragColor;                                  \n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(fragColor,1.0f);	 			\n\
}";



void CrearTriangulo()
{
	GLfloat vertices[] = {
		//Letra F
		-0.9f,-0.9f,0.0f, 1.0f,1.0f,0.0f,
		-0.85f,-0.9f,0.0f, 1.0f,1.0f,0.0f,
		-0.9f,-0.5f,0.0f, 1.0f,1.0f,0.0f,

		-0.85f,-0.9f,0.0f, 1.0f,1.0f,0.0f,
		-0.85f,-0.5f,0.0f, 1.0f,1.0f,0.0f,
		-0.9f,-0.5f,0.0f, 1.0f,1.0f,0.0f,

		-0.9f,-0.5f,0.0f, 1.0f,1.0f,0.0f,
		-0.6f,-0.5f,0.0f, 1.0f,1.0f,0.0f,
		-0.9f,-0.45f,0.0f, 1.0f,1.0f,0.0f,

		-0.6f,-0.5f,0.0f, 1.0f,1.0f,0.0f,
		-0.6f,-0.45f,0.0f, 1.0f,1.0f,0.0f,
		-0.9f,-0.45f,0.0f, 1.0f,1.0f,0.0f,

		-0.9f,-0.7f,0.0f, 1.0f,1.0f,0.0f,
		-0.7f,-0.7f,0.0f, 1.0f,1.0f,0.0f,
		-0.9f,-0.65f,0.0f, 1.0f,1.0f,0.0f,

		-0.7f,-0.7f,0.0f, 1.0f,1.0f,0.0f,
		-0.7f,-0.65f,0.0f, 1.0f,1.0f,0.0f,
		-0.9f,-0.65f,0.0f, 1.0f,1.0f,0.0f,
		 

		//Letra I

		-0.3f,-0.3f,0.0f, 1.0f,1.0f,1.0f,
		-0.25f,-0.3f,0.0f, 1.0f,1.0f,1.0f,
		-0.3f,0.3f,0.0f, 1.0f,1.0f,1.0f,

		-0.25f,-0.3f,0.0f, 1.0f,1.0f,1.0f,
		-0.25f,0.3f,0.0f, 1.0f,1.0f,1.0f,
		-0.3f,0.3f,0.0f, 1.0f,1.0f,1.0f,

				// ===== Barra superior =====
			-0.35f, 0.3f, 0.0f, 1.0f,1.0f,1.0f,
			-0.2f,  0.3f, 0.0f, 1.0f,1.0f,1.0f,
			-0.35f, 0.35f,0.0f, 1.0f,1.0f,1.0f,

			-0.2f,  0.3f, 0.0f, 1.0f,1.0f,1.0f,
			-0.2f,  0.35f,0.0f, 1.0f,1.0f,1.0f,
			-0.35f, 0.35f,0.0f, 1.0f,1.0f,1.0f,

			// ===== Barra inferior =====
			-0.35f, -0.35f, 0.0f, 1.0f,1.0f,1.0f,
			-0.2f,  -0.35f, 0.0f, 1.0f,1.0f,1.0f,
			-0.35f, -0.3f,  0.0f, 1.0f,1.0f,1.0f,

			-0.2f,  -0.35f, 0.0f, 1.0f,1.0f,1.0f,
			-0.2f,  -0.3f,  0.0f, 1.0f,1.0f,1.0f,
			-0.35f, -0.3f,  0.0f, 1.0f,1.0f,1.0f,
		// Letra G 

		0.3f,0.0f,0.0f, 0.5f,0.5f,0.5f,
		0.36f,0.0f,0.0f, 0.5f,0.5f,0.5f,
		0.3f,0.7f,0.0f, 0.5f,0.5f,0.5f,

		0.36f,0.0f,0.0f, 0.5f,0.5f,0.5f,
		0.36f,0.7f,0.0f, 0.5f,0.5f,0.5f,
		0.3f,0.7f,0.0f, 0.5f,0.5f,0.5f,

		0.36f,0.7f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.7f,0.0f, 0.5f,0.5f,0.5f,
		0.36f,0.62f,0.0f, 0.5f,0.5f,0.5f,

		0.62f,0.7f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.62f,0.0f, 0.5f,0.5f,0.5f,
		0.36f,0.62f,0.0f, 0.5f,0.5f,0.5f,

		0.36f,0.08f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.08f,0.0f, 0.5f,0.5f,0.5f,
		0.36f,0.0f,0.0f, 0.5f,0.5f,0.5f,

		0.62f,0.08f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.0f,0.0f, 0.5f,0.5f,0.5f,
		0.36f,0.0f,0.0f, 0.5f,0.5f,0.5f,

		0.56f,0.35f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.35f,0.0f, 0.5f,0.5f,0.5f,
		0.56f,0.08f,0.0f, 0.5f,0.5f,0.5f,

		0.62f,0.35f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.08f,0.0f, 0.5f,0.5f,0.5f,
		0.56f,0.08f,0.0f, 0.5f,0.5f,0.5f,

		0.46f,0.35f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.35f,0.0f, 0.5f,0.5f,0.5f,
		0.46f,0.28f,0.0f, 0.5f,0.5f,0.5f,

		0.62f,0.35f,0.0f, 0.5f,0.5f,0.5f,
		0.62f,0.28f,0.0f, 0.5f,0.5f,0.5f,
		0.46f,0.28f,0.0f, 0.5f,0.5f,0.5f
		
	};
	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estático pues no se modificarán los valores

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
		glEnableVertexAttribArray(1);
		//agregar valores a vèrtices y luego declarar un nuevo vertexAttribPointer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

}
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader,1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n",shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

void CompileShaders() {
	shader= glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta gráfica
	 //verificaciones y prevención de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}



}
int main()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 LÍNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIÓN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

 //Llamada a las funciones creadas antes del main
	CrearTriangulo();
	CompileShaders();

	float lastTime = 0.0f;
	int color = 0;

	srand((unsigned int)glfwGetTime());
	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Recibir eventos del usuario
		glfwPollEvents();

		float currentTime = (float)glfwGetTime();

		// Cambiar color cada 2 segundos
		if (currentTime - lastTime >= 2.0f)
		{
			int newColor;

			// Evita que repita el mismo color consecutivo
			do {
				newColor = rand() % 3;
			} while (newColor == color);

			color = newColor;
			lastTime = currentTime;
		}

		if (color == 0)
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Rojo
		else if (color == 1)
			glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Verde
		else
			glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Azul

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES,0,72);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
		 
		//NO ESCRIBIR NINGUNA LÍNEA DESPUÉS DE glfwSwapBuffers(mainWindow); 
	}


	return 0;
}