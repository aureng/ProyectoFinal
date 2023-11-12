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
	GLfloat getI() { return i_key; }
	GLfloat getJ() { return j_key; }
	GLfloat getK() { return k_key; }
	GLfloat getL() { return l_key; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	//PropiaDeclaracion
	GLfloat getPaletaZ() { return paletaZ; }
	GLfloat getPaletaX() { return paletaX; }
	GLfloat getPaletaC() { return paletaC; }
	GLfloat getResorte() { return resorte; }
	GLboolean getFlipper_sfx() { return flipper_sfx; }
	GLboolean getSpring1_sfx() { return spring1_sfx; }
	GLboolean getSpring2_sfx() { return spring2_sfx; }
	GLboolean getBumper_sfx() { return bumper_sfx; }
	GLboolean getCoin_sfx() { return coin_sfx; }
	GLboolean getPidove_sfx() { return pidove_sfx; }
	GLboolean getBirdFlap_sfx() { return birdflap_sfx; }
	void setCoin_sfx() { coin_sfx = false; }
	void setPidove_sfx() { pidove_sfx = false; }
	void setBirdFlap_sfx() { birdflap_sfx = false; }
	void setFlipper_sfx() { flipper_sfx = false; }
	void setSpring1_sfx() { spring1_sfx = false; }
	void setSpring2_sfx() { spring2_sfx = false; }
	void setBumper_sfx() { bumper_sfx = false; }
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat i_key, j_key, k_key, l_key;
	bool mouseFirstMoved;
	GLfloat paletaZ, paletaX, paletaC;
	GLfloat resorte;
	GLboolean flipper_sfx,spring1_sfx,spring2_sfx, bumper_sfx, coin_sfx, pidove_sfx, birdflap_sfx;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
	static void ManejaMouseButtons(GLFWwindow* window, int button, int action, int mode);
};

