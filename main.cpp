//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

//DECLARACION DE TODOS LOS HEADERS
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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"
#include <iostream>
#include <fstream>
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "irrKlang.h"
using namespace irrklang;

//para iluminación
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
std::vector<Skybox> skyboxList;
std::vector<Camera> cameralist;
std::vector<Material> materialList;
std::vector<PointLight> pointlightList;
std::vector<SpotLight> spotlightList;

int skyboxI = 0, cameraI = 0, materialI = 0, pointlightI = 0, spotlight = 0;

std::string keyFrametxt = "keyFrames.txt";
std::ofstream archivo(keyFrametxt, std::ios::app);

Camera camera;

//LISTA DE TEXTURAS
Texture plainTexture;
Texture dirtTexture;
Texture brickTexture;
Texture pisoTexture;
Texture glassTexture;
Texture scoreboardTexture;

//LISTA DE MODELOS
Model Pinball_M; //Base de la estructura
Model Flipper_M;
Model Bumper_M;
Model Resorte_M;
Model Moneda_M;
Model Bola_M; //Bola de Pinball
Model PBT_M; //Pokeball Top
Model PBB_M; //Pokeball Bottom

//Exports para PIDOVE
Model PidoveBase_M;
Model PidoveAIC_M;
Model PidoveADC_M;
Model PidoveAIA_M;
Model PidoveADA_M;

Skybox skyboxDia, skyboxTarde, skyboxNoche;

//MATERIALES
Material Material_brillante;
Material Material_opaco;

//LUCES
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat oneST =0.0f, fiveST = 0.0f, tenST = 0.0f;
float lastFrame = 0.0f;
static double limitFPS = 1.0 / 60.0;

//############ VARIABLES PARA USO DE KEYFRAMES ####################
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_z;		//Variable para PosicionZ
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float movAvion_zInc;		//Variable para IncrementoZ
	float giroAvion;
	float giroAvionInc;
}FRAME;

bool animacion = false;
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float movAvion_x = 0.0f, movAvion_y = 0.0f, movAvion_z = 0.0f;
float giroAvion = 0;
#define MAX_FRAMES 100 //100
int i_max_steps = 90; //90
int i_curr_steps = 6; //Depende del frame actual

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;			//introducir datos
bool play = false;
int playIndex = 0; //Empieza desde 0
//#####################


//VARIABLES PARA USO DE TEXTURE-OFFSET 
//Dobles valores para cada textura que se quiera se pueda mover con offset: float->el offset, int->cuantas partes hay
float toffsetScoreboardU = 0.0f, toffsetScoreboardV = 0.0f;
int toffsetcountu = 0, toffsetcountv = 0;


//SHADERS
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// Para los keyframes (De preferencia se va si los puedo implementar en window.cpp)
void inputKeyframes(bool* keys);

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
		0, 2, 1,
		0, 3, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		0.0f, -0.5f, -0.5f,		0.0f, 0.875f,		1.0f, 0.0f, 0.0f, //0 izq inf
		0.0f, 0.5f, -0.5f,		0.0,  1.0f,			1.0f, 0.0f, 0.0f, //1 izq sup
		0.0f, 0.5f, 0.5f,		0.333f, 1.0f,			1.0f, 0.0f, 0.0f, //2 der sup
		0.0f, -0.5f, 0.5f,		0.333f, 0.875f,		1.0f, 0.0f, 0.0f  //3 der inf
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

	unsigned int glassIndices[] = {
	   0, 1, 2,
	   0, 2, 3
	};

	GLfloat glassVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 6);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(glassVertices, glassIndices, 32, 6);
	meshList.push_back(obj4);

	//calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(glassIndices, 6, glassVertices, 32, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// ################## FUNCIONES DE KEYFRAMES ######################
void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].movAvion_z = movAvion_z;
	KeyFrame[FrameIndex].giroAvion = giroAvion;//completar
	//no volatil, agregar una forma de escribir a un archivo para guardar los frames
	if (!archivo.is_open()) {
		std::cerr << "No se pudo abrir el archivo." << std::endl;
	}

	archivo << "KeyFrame[" << FrameIndex << "].movAvion_x = " << movAvion_x << ".0f;\n";
	archivo << "KeyFrame[" << FrameIndex << "].movAvion_y = " << movAvion_y << ".0f;\n";
	archivo << "KeyFrame[" << FrameIndex << "].movAvion_z = " << movAvion_z << ".0f;\n";
	archivo << "KeyFrame[" << FrameIndex << "].giroAvion = " << giroAvion << ".0f;\n\n";

	FrameIndex++;
}

void resetElements(void) //Tecla 0
{
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	movAvion_z = KeyFrame[0].movAvion_z;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].movAvion_zInc = (KeyFrame[playIndex + 1].movAvion_z - KeyFrame[playIndex].movAvion_z) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

}

/*
void animate(void)
{
	if (mainWindow.getAnimGeneral())
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				mainWindow.setAnimGeneralFalse();
				mainWindow.setTeclaSpace();
			}
			else //Interpolación del próximo cuadro
			{
				i_curr_steps = 0; //Resetea contador
				interpolation(); //Lo que va consiguiendo cada frame intermedio
			}
		}
		else
		{
			//Dibujar Animación incremento por incremento
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			movAvion_z += KeyFrame[playIndex].movAvion_zInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}
	}
}
*/
///////////////* FIN KEYFRAMES*////////////////////////////

int main()
{
	mainWindow = Window(1920, 1080); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	ISoundEngine* engine = createIrrKlangDevice();
	engine->play2D("audio/theme.mp3", true);

	// TEXTURAS PARA MIS COSAS
	pisoTexture = Texture("Textures/piso.jpg");
	pisoTexture.LoadTexture();
	glassTexture = Texture("Textures/glass.png");
	glassTexture.LoadTextureA();
	scoreboardTexture = Texture("Textures/scoreboard.jpg");
	scoreboardTexture.LoadTexture();

	// Carga de Modelos
	Pinball_M = Model();
	Pinball_M.LoadModel("Models/pinball.obj");
	Flipper_M = Model();
	Flipper_M.LoadModel("Models/flipper.obj");
	Bumper_M = Model();
	Bumper_M.LoadModel("Models/bumper.obj");
	Resorte_M = Model();
	Resorte_M.LoadModel("Models/resorte.obj");
	Moneda_M = Model();
	Moneda_M.LoadModel("Models/moneda.obj");
	Bola_M = Model();
	Bola_M.LoadModel("Models/bola.obj");
	PBT_M = Model();
	PBT_M.LoadModel("Models/Pokebola/poke_arriba.obj");
	PBB_M = Model();
	PBB_M.LoadModel("Models/Pokebola/poke_abajo.obj");

	//Modelo de Pidove
	PidoveBase_M = Model();
	PidoveBase_M.LoadModel("Models/Pidove/pidovebase.obj");
	PidoveAIC_M = Model();
	PidoveAIC_M.LoadModel("Models/Pidove/pidove_ala_izq_cerrada.obj");
	PidoveADC_M = Model();
	PidoveADC_M.LoadModel("Models/Pidove/pidove_ala_der_cerrada.obj");
	PidoveAIA_M = Model();
	PidoveAIA_M.LoadModel("Models/Pidove/pidove_ala_izq_abierta.obj");
	PidoveADA_M = Model();
	PidoveADA_M.LoadModel("Models/Pidove/pidove_ala_der_abierta.obj");

	std::vector<std::string> skyboxFacesDia;
	skyboxFacesDia.push_back("Textures/Skybox/dia/left.png"); //Left
	skyboxFacesDia.push_back("Textures/Skybox/dia/right.png"); //Right
	skyboxFacesDia.push_back("Textures/Skybox/dia/bottom.png"); //bottom
	skyboxFacesDia.push_back("Textures/Skybox/dia/top.png"); //Up
	skyboxFacesDia.push_back("Textures/Skybox/dia/back.png"); //Back
	skyboxFacesDia.push_back("Textures/Skybox/dia/front.png"); //Front
	skyboxDia = Skybox(skyboxFacesDia);
	skyboxList.push_back(skyboxDia);

	std::vector<std::string> skyboxFacesTarde;
	skyboxFacesTarde.push_back("Textures/Skybox/tarde/left.png"); //Left
	skyboxFacesTarde.push_back("Textures/Skybox/tarde/right.png"); //Right
	skyboxFacesTarde.push_back("Textures/Skybox/tarde/bottom.png"); //bottom
	skyboxFacesTarde.push_back("Textures/Skybox/tarde/top.png"); //Up
	skyboxFacesTarde.push_back("Textures/Skybox/tarde/back.png"); //Back
	skyboxFacesTarde.push_back("Textures/Skybox/tarde/front.png"); //Front
	skyboxTarde = Skybox(skyboxFacesTarde);
	skyboxList.push_back(skyboxTarde);

	std::vector<std::string> skyboxFacesNoche;
	skyboxFacesNoche.push_back("Textures/Skybox/noche/left.png"); //Left
	skyboxFacesNoche.push_back("Textures/Skybox/noche/right.png"); //Right
	skyboxFacesNoche.push_back("Textures/Skybox/noche/bottom.png"); //bottom
	skyboxFacesNoche.push_back("Textures/Skybox/noche/top.png"); //Up
	skyboxFacesNoche.push_back("Textures/Skybox/noche/back.png"); //Back
	skyboxFacesNoche.push_back("Textures/Skybox/noche/front.png"); //Front
	skyboxNoche = Skybox(skyboxFacesNoche);
	skyboxList.push_back(skyboxNoche);


	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.6f, 0.6f,
		1.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0, uniformColor = 0;

	// Creacion de camaras y vistas de projeccion
	camera = Camera(glm::vec3(50.0f, 80.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	// Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		
		//Different timers for 1, 5 and 10 seconds
		if (now - oneST >= 1.0f) {
			//######## LOGICA DE CAMBIO DE TEXTURAS DE SCOREBOARD #######
			toffsetScoreboardV -= 0.124f;
			toffsetcountv += 1;
			if (toffsetcountv == 8) {//Termina de recorrer las filas
				toffsetcountu += 1; // contador para u
				toffsetcountv = 0; // resetea contador de v
				toffsetScoreboardV = 0.0f; //Empieza el offset desde arriba
				toffsetScoreboardU += 0.333f; // siguiente columna
			}
			if (toffsetcountu == 3) { //Si se pasa de las columnas de la textura
				toffsetcountu = 0; //resetea contador 
				toffsetcountv = 0;
				toffsetScoreboardU = 0.0;
				toffsetScoreboardV = 0.0;
			}
			//######################
			oneST = now;
		}

		if (now - fiveST >= 5.0f) {
			//engine->play2D("audio/pidove.wav", false);
			//engine->play2D("audio/coin.wav", false);
			fiveST = now;
		}
		if (now - tenST >= 10.0f) {
			skyboxI++;
			if (skyboxI == 3) {
				skyboxI = 0;
			}
			engine->play2D("audio/pidove.wav", false);
			tenST = now;
		}

		//############ VARIABLES QUE PUEDEN CAMBIAR DEPENDIENDO LA LISTA ####################
		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skyboxList[skyboxI].DrawSkybox(camera.calculateViewMatrix(), projection);
		// ########## ESTO NO TENDRA CAMBIOS RELEVANTES EN TIEMPO DE EJECUCION ################
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		//	información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// FLASH EN TIEMPO DE EJECUCION
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		//###################################


		//Inicializacion de apoyo
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glm::vec2 noOffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		// ##################################################


		// ################### ARMADO DEL ESCENARIO ########################
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pinball_M.RenderModel();

		//############## INICIO DE PIDOVE JERARQUICO #################
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-24.59f, 117.51f, 2.225f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PidoveBase_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.556f, 0.557f, 2.897f));
		model = glm::rotate(model, glm::radians(mainWindow.getPaletaZ()), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PidoveADC_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.556f, 0.557f, -2.897f));
		model = glm::rotate(model, glm::radians(mainWindow.getPaletaX()), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PidoveAIC_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.117f, 1.055f, -2.054));
		model = glm::rotate(model, glm::radians(mainWindow.getPaletaC()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//PidoveAIA_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.072f, 1.055f, 2.055f));
		model = glm::rotate(model, glm::radians(mainWindow.getPaletaC()), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//PidoveADA_M.RenderModel();
		//################# FIN DE PIDOVE JERARQUICO #################

		//############ EXTRAS Y SFX #################
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(31.55f, 53.884f, -13.894f));
		model = glm::rotate(model, glm::radians(16.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getResorte(), 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Resorte_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.657f, 62.38f, 2.595f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bumper_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.529f, 63.357f, -3.465f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bumper_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.337f, 61.769f, -4.566f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bumper_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.58f, 62.557f, 9.688f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getPaletaZ()), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flipper_M.RenderModel(); //Flipper Izquierdo Superior controlado por Z

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(24.908f, 54.961f, 5.612f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getPaletaX()), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flipper_M.RenderModel(); //Flipper Izquierdo Normal controlado por X

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.243f, 54.857f, -3.084f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getPaletaC()), glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flipper_M.RenderModel(); //Flipper Derecho Normal Controlado por C

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(38.0f + mainWindow.getI(), 42.838f + mainWindow.getJ(), -2.064f + mainWindow.getK()));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Moneda_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(28.684f, 54.559, -13.897f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getI(), mainWindow.getJ(), mainWindow.getK()));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Bola_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-14.625f, 65.811, 5.328));
		model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PBT_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-14.625f, 65.811, 5.328));
		model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PBB_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-22.315f, 76.122f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(8.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 5.593f, 19.242f));
		toffset = glm::vec2(toffsetScoreboardU, toffsetScoreboardV);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		scoreboardTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		if (mainWindow.getFlipper_sfx()) {
			engine->play2D("audio/flipper.wav", false);
			mainWindow.setFlipper_sfx();
		}
		if (mainWindow.getSpring1_sfx()) {
			engine->play2D("audio/spring1.wav", false);
			mainWindow.setSpring1_sfx();
		}
		if (mainWindow.getSpring2_sfx()) {
			engine->play2D("audio/spring2.wav", false);
			mainWindow.setSpring2_sfx();
		}
		if (mainWindow.getBumper_sfx()) {
			engine->play2D("audio/bumper.wav", false);
			mainWindow.setBumper_sfx();
		}
		if (mainWindow.getCoin_sfx()) {
			engine->play2D("audio/coin.wav", false);
			mainWindow.setCoin_sfx();
		}
		if (mainWindow.getPidove_sfx()) {
			engine->play2D("audio/pidove.wav", false);
			mainWindow.setPidove_sfx();
		}
		if (mainWindow.getBirdFlap_sfx()) {
			engine->play2D("audio/birdflap.wav", false);
			mainWindow.setBirdFlap_sfx();
		}
		//############ FIN DE EXTRAS Y SFX #################


		// USO DE TEXTURAS CON TRANSPARENCIA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.62f, 61.782f, -0.248f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(58.0f, 30.0f, 1.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(noOffset));
		glassTexture.UseTexture();
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	engine->drop();
	return 0;
}
