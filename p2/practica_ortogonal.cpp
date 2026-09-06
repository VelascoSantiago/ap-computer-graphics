#include<iostream>

//#define GLEW_STATIC

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// Shaders
#include "Shader.h"

const GLint WIDTH = 800, HEIGHT = 600;


int main() {
	glfwInit();

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecciones y transformaciones basicas", nullptr, nullptr);

	int screenWidth, screenHeight;

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD" << std::endl;
		return EXIT_FAILURE;
	}


	// Define las dimensiones del viewport
	glViewport(0, 0, screenWidth, screenHeight);


	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	// enable alpha support
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Build and compile our shader program
	Shader ourShader("Shader/core.vs", "Shader/core.frag");


	// Constante de escala: evita el error de olvidar multiplicar
	// alguna coordenada por 500 (bug que tenían las versiones anteriores)
	const float S = 500.0f;

	// Set up vertex data (and buffer(s)) and attribute pointers
	// use with Orthographic Projection
	GLfloat vertices[] = {
		-0.5f * S, -0.5f * S,  0.5f * S,  1.0f, 0.0f, 0.0f,//Front (rojo)
		 0.5f * S, -0.5f * S,  0.5f * S,  1.0f, 0.0f, 0.0f,
		 0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 0.0f, 0.0f,
		 0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 0.0f, 0.0f,
		-0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 0.0f, 0.0f,
		-0.5f * S, -0.5f * S,  0.5f * S,  1.0f, 0.0f, 0.0f,

		-0.5f * S, -0.5f * S, -0.5f * S,  0.0f, 1.0f, 0.0f,//Back (verde)
		 0.5f * S, -0.5f * S, -0.5f * S,  0.0f, 1.0f, 0.0f,
		 0.5f * S,  0.5f * S, -0.5f * S,  0.0f, 1.0f, 0.0f,
		 0.5f * S,  0.5f * S, -0.5f * S,  0.0f, 1.0f, 0.0f,
		-0.5f * S,  0.5f * S, -0.5f * S,  0.0f, 1.0f, 0.0f,
		-0.5f * S, -0.5f * S, -0.5f * S,  0.0f, 1.0f, 0.0f,

		 0.5f * S, -0.5f * S,  0.5f * S,  0.0f, 0.0f, 1.0f,//Right (azul)
		 0.5f * S, -0.5f * S, -0.5f * S,  0.0f, 0.0f, 1.0f,
		 0.5f * S,  0.5f * S, -0.5f * S,  0.0f, 0.0f, 1.0f,
		 0.5f * S,  0.5f * S, -0.5f * S,  0.0f, 0.0f, 1.0f,
		 0.5f * S,  0.5f * S,  0.5f * S,  0.0f, 0.0f, 1.0f,
		 0.5f * S, -0.5f * S,  0.5f * S,  0.0f, 0.0f, 1.0f,

		-0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 1.0f, 0.0f,//Left (amarillo)
		-0.5f * S,  0.5f * S, -0.5f * S,  1.0f, 1.0f, 0.0f,
		-0.5f * S, -0.5f * S, -0.5f * S,  1.0f, 1.0f, 0.0f,
		-0.5f * S, -0.5f * S, -0.5f * S,  1.0f, 1.0f, 0.0f,
		-0.5f * S, -0.5f * S,  0.5f * S,  1.0f, 1.0f, 0.0f,
		-0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 1.0f, 0.0f,

		-0.5f * S, -0.5f * S, -0.5f * S,  0.0f, 1.0f, 1.0f,//Bottom (cian)
		 0.5f * S, -0.5f * S, -0.5f * S,  0.0f, 1.0f, 1.0f,
		 0.5f * S, -0.5f * S,  0.5f * S,  0.0f, 1.0f, 1.0f,
		 0.5f * S, -0.5f * S,  0.5f * S,  0.0f, 1.0f, 1.0f,
		-0.5f * S, -0.5f * S,  0.5f * S,  0.0f, 1.0f, 1.0f,
		-0.5f * S, -0.5f * S, -0.5f * S,  0.0f, 1.0f, 1.0f,

		-0.5f * S,  0.5f * S, -0.5f * S,  1.0f, 0.2f, 0.5f,//Top (rosa)
		 0.5f * S,  0.5f * S, -0.5f * S,  1.0f, 0.2f, 0.5f,
		 0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 0.2f, 0.5f,
		 0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 0.2f, 0.5f,
		-0.5f * S,  0.5f * S,  0.5f * S,  1.0f, 0.2f, 0.5f,
		-0.5f * S,  0.5f * S, -0.5f * S,  1.0f, 0.2f, 0.5f,
	};


	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Enlazar Vertex Array Object
	glBindVertexArray(VAO);

	// Copiamos nuestro arreglo de vertices en un buffer de vertices para que OpenGL lo use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Posicion
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glm::mat4 projection = glm::mat4(1);

	// Proyeccion ortografica en coordenadas de pixeles de pantalla
	projection = glm::ortho(0.0f, (GLfloat)screenWidth, 0.0f, (GLfloat)screenHeight, 0.1f, 1000.0f);//Izq,Der,Fondo,Alto,Cercania,Lejania

	// Angulo de rotacion, se ira incrementando cada frame para animar el giro
	GLfloat angle = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Render
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.Use();

		glm::mat4 model = glm::mat4(1);
		glm::mat4 view = glm::mat4(1);

		// --- ROTACION EN DOS EJES PARA VER VARIAS CARAS A LA VEZ ---
		// Con una sola rotacion (solo X, o solo Y) normalmente solo alcanzas
		// a revelar UNA cara extra (Top o Right). Combinando X y Y revelas
		// simultaneamente 3 caras: Front, Top/Bottom y Right/Left,
		// dando el clasico efecto "isometrico" de ver la esquina del cubo.
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // inclina arriba/abajo
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // gira izquierda/derecha

		// Traslada la escena para que el cubo (centrado en el origen)
		// caiga dentro del area visible de glm::ortho, que va de
		// (0,0) a (screenWidth, screenHeight) en pantalla.
		view = glm::translate(view, glm::vec3(screenWidth / 2, screenHeight / 2, -700.0f));

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");

		glUniformMatrix4fv(projecLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return EXIT_SUCCESS;
}