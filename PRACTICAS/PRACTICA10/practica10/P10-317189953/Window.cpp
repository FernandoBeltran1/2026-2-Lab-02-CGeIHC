#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	articulacion1 = 0.0f;	
	articulacion5 = 0.0f;
	luzlampara = 1.0f;
	pointCuboPez = 0.1f;
	banderaArt5 = false;
	banderacoche = false;
	luzR = 1.0f;
	luzG = 0.0f;
	luzB = 0.0f;

	spotPezDirX = 1.0f;
	spotPezDirY = -1.0f;
	spotPezDirZ = 1.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "PracticaXX:Nombre de la practica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}

	if (key == GLFW_KEY_E)
	{
		theWindow->articulacion1 += 10.0;
	}
	if (key == GLFW_KEY_R)
	{
		theWindow->articulacion1 -= 10.0;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (theWindow->luzR == 1.0f && theWindow->luzG == 0.0f && theWindow->luzB == 0.0f) {
			theWindow->luzR = 1.0f;
			theWindow->luzG = 1.0f;
			theWindow->luzB = 0.0f;
		}
		else if (theWindow->luzR == 1.0f && theWindow->luzG == 1.0f && theWindow->luzB == 0.0f) {
			theWindow->luzR = 0.0f;
			theWindow->luzG = 1.0f;
			theWindow->luzB = 0.0f;
		}
		else if (theWindow->luzR == 0.0f && theWindow->luzG == 1.0f && theWindow->luzB == 0.0f) {
			theWindow->luzR = 0.0f;
			theWindow->luzG = 1.0f;
			theWindow->luzB = 1.0f;
		}
		else if (theWindow->luzR == 0.0f && theWindow->luzG == 1.0f && theWindow->luzB ==1.0f) {
			theWindow->luzR = 0.0f;
			theWindow->luzG = 0.0f;
			theWindow->luzB = 1.0f;
		}
		else if (theWindow->luzR == 0.0f && theWindow->luzG == 0.0f && theWindow->luzB == 1.0f) {
			theWindow->luzR = 1.0f;
			theWindow->luzG = 0.0f;
			theWindow->luzB = 1.0f;
		}
		else if (theWindow->luzR == 1.0f && theWindow->luzG == 0.0f && theWindow->luzB == 1.0f) {
			theWindow->luzR = 1.0f;
			theWindow->luzG = 0.0f;
			theWindow->luzB = 0.0f;
		}
	}
	
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		if (!theWindow->banderacoche) {
			theWindow->banderacoche = true;
		}
		else {
			theWindow->banderacoche = false;
		}
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		if (theWindow->luzlampara==0.0f) {
			theWindow->luzlampara += 1.0f;
		}
		else if (theWindow->luzlampara == 1.0f){
			theWindow->luzlampara -= 1.0f;
		}
		
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		if (theWindow->pointCuboPez == 0.0f) {
			theWindow->pointCuboPez += 0.1f;
		}
		else if (theWindow->pointCuboPez == 0.1f) {
			theWindow->pointCuboPez = 0.0f;
		}
		

	}

	if (key == GLFW_KEY_G)
	{
		if (!theWindow->banderaArt5)
		{
			theWindow->articulacion5 -= 10.0f;

			if (theWindow->articulacion5 <= -60.13f)
			{
				theWindow->articulacion5 = -60.13f;
				theWindow->banderaArt5 = true;
			}
		}
		else
		{
			theWindow->articulacion5 += 10.0f;

			if (theWindow->articulacion5 >= 0.0f)
			{
				theWindow->articulacion5 = 0.0f;
				theWindow->banderaArt5 = false;
			}
		}
	}
	
	if (key == GLFW_KEY_J && (action == GLFW_PRESS))
	{
		theWindow->spotPezDirX -= 0.2f;
		if (theWindow->spotPezDirX < -1.0f) theWindow->spotPezDirX = -1.0f;
	}
	if (key == GLFW_KEY_L && (action == GLFW_PRESS))
	{
		theWindow->spotPezDirX += 0.2f;
		if (theWindow->spotPezDirX > 1.0f) theWindow->spotPezDirX = 1.0f;
	}

	
	if (key == GLFW_KEY_N && (action == GLFW_PRESS))
	{
		theWindow->spotPezDirY -= 0.2f;
		if (theWindow->spotPezDirY < -1.0f) theWindow->spotPezDirY = -1.0f;
	}
	if (key == GLFW_KEY_M && (action == GLFW_PRESS))
	{
		theWindow->spotPezDirY += 0.2f;
		if (theWindow->spotPezDirY > 1.0f) theWindow->spotPezDirY = 1.0f;
	}

	
	if (key == GLFW_KEY_Z && (action == GLFW_PRESS))
	{
		theWindow->spotPezDirZ -= 0.2f;
		if (theWindow->spotPezDirZ < -1.0f) theWindow->spotPezDirZ = -1.0f;
	}
	if (key == GLFW_KEY_X && (action == GLFW_PRESS))
	{
		theWindow->spotPezDirZ += 0.2f;
		if (theWindow->spotPezDirZ > 1.0f) theWindow->spotPezDirZ = 1.0f;
	}


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
