#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	GLfloat getarticulacion1() { return articulacion1; }
	GLfloat getarticulacion5() { return articulacion5; }
	GLfloat getluzlampara() { return luzlampara; }
	GLfloat getpointCuboPez() { return pointCuboPez; }
	GLfloat getbanderacoche() { return banderacoche; }
	GLfloat getluzR() { return luzR; }
	GLfloat getluzG() { return luzG; }
	GLfloat getluzB() { return luzB; }

	GLfloat getSpotPezDirX() { return spotPezDirX; }
	GLfloat getSpotPezDirY() { return spotPezDirY; }
	GLfloat getSpotPezDirZ() { return spotPezDirZ; }

	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	GLfloat articulacion1, luzlampara, articulacion5, luzR, luzG, luzB, pointCuboPez, 
		spotPezDirX, spotPezDirY, spotPezDirZ;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool mouseFirstMoved;
	bool banderacoche;
	bool banderaArt5;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

