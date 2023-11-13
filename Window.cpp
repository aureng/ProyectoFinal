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
	i_key = j_key = k_key = l_key = 0.0f;
	paletaZ = paletaX = paletaC = 0.0f;
	resorte = 0.0f;
	moneda_anim = pidove_anim = false;
	coin_sfx = pidove_sfx = birdflap_sfx = bumper_sfx = spring1_sfx = spring2_sfx = flipper_sfx = false;
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
	mainWindow = glfwCreateWindow(width, height, "Proyecto Final: Pinball", glfwGetPrimaryMonitor(), NULL);
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


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
	glfwSetMouseButtonCallback(mainWindow, ManejaMouseButtons);
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

	//Numeros declarados para uso de animaciones?
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		//printf("Presione 1\n");
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		//printf("Presione 2\n");
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		//printf("Presione 3\n");
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		//printf("Presione 4\n");
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		//printf("Presione 5\n");
	}
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		//printf("Presione 6\n");
	}
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		//printf("Presione 7\n");
	}
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		//printf("Presione 8\n");
	}
	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		//printf("Presione 9\n");
	}
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		//printf("Presione 0\n");
	}

	//Controles TFGH y IJKL para keyframes?
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		theWindow->i_key += 1.0f;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		theWindow->j_key += 1.0f;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		theWindow->k_key += 1.0f;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		theWindow->l_key += 1.0f;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		theWindow->i_key -= 1.0f;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		theWindow->j_key -= 1.0f;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		theWindow->k_key -= 1.0f;
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		theWindow->l_key -= 1.0f;
	}


	if (key == GLFW_KEY_Z)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->flipper_sfx = true;
			do {
				theWindow->paletaZ += 1.0f;
			} while (theWindow->paletaZ <= 45.0f);
		}
		else if (action == GLFW_RELEASE)
		{
			do {
				theWindow->paletaZ -= 1.0f;
			} while (theWindow->paletaZ >= 0.0f);
		}
	}
	if (key == GLFW_KEY_X)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->flipper_sfx = true;
			do {
				theWindow->paletaX +=  1.0f;
			} while (theWindow->paletaX <= 45.0f);
		}
		else if (action == GLFW_RELEASE)
		{
			do {
				theWindow->paletaX -= 1.0f;
			} while (theWindow->paletaX >= 0.0f);
		}
	}
	if (key == GLFW_KEY_C)
	{
		if (action == GLFW_PRESS) 
		{
			theWindow->flipper_sfx = true;
			do {
				theWindow->paletaC += 1.0f;
			} while (theWindow->paletaC <= 45.0f);
		}
		else if(action == GLFW_RELEASE)
		{
			do {
				theWindow->paletaC -= 1.0f;
			} while (theWindow->paletaC >= 0.0f);
		}	
	}


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
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

	// Calculate the offset from the center of the screen
	double xOffset = xPos - theWindow->width / 2.0;
	double yOffset = yPos - theWindow->height / 2.0;

	// Set the mouse position to the center of the screen
	glfwSetCursorPos(window, theWindow->width / 2.0, theWindow->height / 2.0);

	// Update the lastX and lastY values
	theWindow->lastX = theWindow->width / 2.0;
	theWindow->lastY = theWindow->height / 2.0;

	// Update the xChange and yChange values with the offset
	theWindow->xChange = xOffset;
	theWindow->yChange = -yOffset;


}

void Window::ManejaMouseButtons(GLFWwindow* window, int button, int action, int mods)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	// Maneja el clic del botón izquierdo
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			// Realiza la acción deseada para el clic izquierdo
			theWindow->spring1_sfx = true;
			theWindow->resorte += 1.0f;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->spring2_sfx = true;
			theWindow->resorte -= 1.0f;
		}
	}

	// Puedes agregar más bloques condicionales para otros botones si es necesario
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
