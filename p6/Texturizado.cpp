#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
GLuint loadTexture(const char *path);
void applyWrap(GLuint tex, GLint wrap);
void applyUVPreset(GLuint vbo, GLfloat *vertices, int preset);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ---- Estado de los experimentos (se cambian con teclas) ----
// Presets de UV: {u0, v0, u1, v1}
// Orden de vertices: inf-izq(u0,v0) inf-der(u1,v0) sup-der(u1,v1) sup-izq(u0,v1)
const int NUM_PRESETS = 6;
GLfloat uvPresets[NUM_PRESETS][4] =
{
	{  0.0f,  0.0f, 1.0f, 1.0f },  // 1: normal
	{  0.0f,  0.0f, 0.5f, 0.5f },  // 2: decimales (solo un cuarto de la textura)
	{  0.25f, 0.25f, 0.75f, 0.75f },// 3: decimales (recorte central)
	{  0.0f,  0.0f, 2.0f, 2.0f },  // 4: mayores a 1
	{ -1.0f, -1.0f, 1.0f, 1.0f },  // 5: negativos
	{  0.0f,  0.0f, 4.0f, 4.0f },  // 6: mucho mayor a 1
};
int  uvPreset = 0;
bool uvDirty = false;

GLint wrapModes[4] = { GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER };
const char *wrapNames[4] = { "GL_REPEAT", "GL_MIRRORED_REPEAT", "GL_CLAMP_TO_EDGE", "GL_CLAMP_TO_BORDER" };
int  wrapIdx = 0;
bool wrapDirty = false;

bool useBlend = false; // false = recorte con discard, true = mezcla alfa (blending)

// ---- Lista de imagenes disponibles (agrega aqui tus rutas) ----
const char *imagePaths[] =
{
	"images/checker_Tex.png",
	"images/window.png",
	"images/lupa.png",
	"images/pikachu.jpeg",
};
const int NUM_IMAGES = sizeof(imagePaths) / sizeof(imagePaths[0]);
GLuint textures[16];   // maximo 16 imagenes
bool showBg = true;    // mostrar/ocultar el quad de fondo (tecla G)
int bgIdx = 0;         // imagen del quad de fondo (tecla N)
int fgIdx = 1;         // imagen del quad del frente (tecla M)

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Texturizado", nullptr, nullptr);
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// Para el modo "blend"
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// Posiciones | Colores | UV (las UV se reescriben con applyUVPreset)
	GLfloat vertices[] =
	{
		-0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f,
	};

	GLuint indices[] =
	{
		0, 1, 3,
		1, 2, 3
	};

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// GL_DYNAMIC_DRAW porque vamos a modificar las UV en tiempo real
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Texturas
	stbi_set_flip_vertically_on_load(true);
	for (int i = 0; i < NUM_IMAGES; i++)
		textures[i] = loadTexture(imagePaths[i]);
	if (fgIdx >= NUM_IMAGES) fgIdx = NUM_IMAGES - 1;

	applyUVPreset(VBO, vertices, uvPreset);

	std::cout << "Controles:\n"
		<< "  1-6 : presets de UV (1 normal, 2-3 decimales, 4 y 6 >1, 5 negativos)\n"
		<< "  Z/X/C/V : REPEAT / MIRRORED_REPEAT / CLAMP_TO_EDGE / CLAMP_TO_BORDER\n"
		<< "  B : alternar discard (recorte) / blending (mezcla alfa)\n"
		<< "  N : cambiar imagen de fondo | M : cambiar imagen del frente\n"
		<< "  G : mostrar/ocultar la imagen de fondo\n"
		<< "  WASD + mouse : camara, ESC : salir\n" << std::endl;

	GLint modelLoc  = glGetUniformLocation(lampShader.Program, "model");
	GLint viewLoc   = glGetUniformLocation(lampShader.Program, "view");
	GLint projLoc   = glGetUniformLocation(lampShader.Program, "projection");
	GLint cutoffLoc = glGetUniformLocation(lampShader.Program, "alphaCutoff");

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();

		// Aplicar cambios pedidos por teclado
		if (uvDirty)
		{
			applyUVPreset(VBO, vertices, uvPreset);
			uvDirty = false;
		}
		if (wrapDirty)
		{
			for (int i = 0; i < NUM_IMAGES; i++)
				applyWrap(textures[i], wrapModes[wrapIdx]);
			std::cout << "Wrap: " << wrapNames[wrapIdx] << std::endl;
			wrapDirty = false;
		}

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lampShader.Use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Modo de transparencia
		if (useBlend)
		{
			glEnable(GL_BLEND);
			glUniform1f(cutoffLoc, 0.0f);   // no descartar nada, solo mezclar
		}
		else
		{
			glDisable(GL_BLEND);
			glUniform1f(cutoffLoc, 0.1f);   // descartar pixeles casi transparentes
		}

		glBindVertexArray(VAO);

		// 1) Primero lo opaco: el checker, mas grande y al fondo
		glm::mat4 model(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.3f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[bgIdx]);
		if (showBg)
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// 2) Al final lo transparente, al frente
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindTexture(GL_TEXTURE_2D, textures[fgIdx]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(NUM_IMAGES, textures);
	glfwTerminate();

	return 0;
}

// Carga una textura y elige el formato segun los canales de la imagen
GLuint loadTexture(const char *path)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	int w, h, nrChannels;
	unsigned char *data = stbi_load(path, &w, &h, &nrChannels, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)      format = GL_RED;
		else if (nrChannels == 3) format = GL_RGB;
		else if (nrChannels == 4) format = GL_RGBA;

		// Imagenes RGB con ancho no multiplo de 4 necesitan esto
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Textura cargada: " << path << " (" << w << "x" << h << ", " << nrChannels << " canales)" << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture: " << path << std::endl;
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // los mipmaps no aplican en MAG

	return tex;
}

// Cambia el modo de wrapping de una textura (S y T)
void applyWrap(GLuint tex, GLint wrap)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	// Color visible para CLAMP_TO_BORDER (magenta)
	GLfloat border[] = { 1.0f, 0.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
}

// Reescribe las UV de los 4 vertices en el arreglo y las sube al VBO
void applyUVPreset(GLuint vbo, GLfloat *vertices, int preset)
{
	GLfloat u0 = uvPresets[preset][0], v0 = uvPresets[preset][1];
	GLfloat u1 = uvPresets[preset][2], v1 = uvPresets[preset][3];

	// Cada vertice tiene 8 floats; las UV estan en los indices 6 y 7
	vertices[0 * 8 + 6] = u0; vertices[0 * 8 + 7] = v0; // inf-izq
	vertices[1 * 8 + 6] = u1; vertices[1 * 8 + 7] = v0; // inf-der
	vertices[2 * 8 + 6] = u1; vertices[2 * 8 + 7] = v1; // sup-der
	vertices[3 * 8 + 6] = u0; vertices[3 * 8 + 7] = v1; // sup-izq

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 8 * sizeof(GLfloat), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::cout << "UV preset " << (preset + 1) << ": (" << u0 << "," << v0 << ") -> (" << u1 << "," << v1 << ")" << std::endl;
}

void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)        keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}

	if (action == GLFW_PRESS)
	{
		// Presets de UV: teclas 1..6
		if (key >= GLFW_KEY_1 && key < GLFW_KEY_1 + NUM_PRESETS)
		{
			uvPreset = key - GLFW_KEY_1;
			uvDirty = true;
		}
		// Modos de wrapping
		if (key == GLFW_KEY_Z) { wrapIdx = 0; wrapDirty = true; }
		if (key == GLFW_KEY_X) { wrapIdx = 1; wrapDirty = true; }
		if (key == GLFW_KEY_C) { wrapIdx = 2; wrapDirty = true; }
		if (key == GLFW_KEY_V) { wrapIdx = 3; wrapDirty = true; }
		// Cambiar imagen de fondo (N) y de frente (M)
		if (key == GLFW_KEY_N)
		{
			bgIdx = (bgIdx + 1) % NUM_IMAGES;
			std::cout << "Fondo: " << imagePaths[bgIdx] << std::endl;
		}
		if (key == GLFW_KEY_M)
		{
			fgIdx = (fgIdx + 1) % NUM_IMAGES;
			std::cout << "Frente: " << imagePaths[fgIdx] << std::endl;
		}
		// Mostrar/ocultar el fondo
		if (key == GLFW_KEY_G)
		{
			showBg = !showBg;
			std::cout << "Fondo: " << (showBg ? "VISIBLE" : "OCULTO") << std::endl;
		}
		// Discard vs blending
		if (key == GLFW_KEY_B)
		{
			useBlend = !useBlend;
			std::cout << "Transparencia: " << (useBlend ? "BLENDING" : "DISCARD") << std::endl;
		}
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}