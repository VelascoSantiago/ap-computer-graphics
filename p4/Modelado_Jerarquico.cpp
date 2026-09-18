#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders
#include "Shader.h"

void Inputs(GLFWwindow *window);

const GLint WIDTH = 1200, HEIGHT = 800;

// For Keyboard
float movX = 0.0f,
	  movY = 0.0f,
	  movZ = -5.0f,
	  rot = 0.0f;

// For model
float hombro = 0.0f;
float forearm = 0.0f;
float hand = 0.0f;
float finger = 0.0f;
float finger_tip = 0.0f;
float thumb = 0.0f;
float thumb_tip = 0.0f;

int main()
{
	glfwInit();
	// Verificaci�n de compatibilidad
	//  Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Modelado jerarquico", nullptr, nullptr);

	int screenWidth, screenHeight;

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	// Verificaci�n de errores de creacion  ventana
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
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

	// Set up vertex data (and buffer(s)) and attribute pointers
	// use with Orthographic Projection

	// use with Perspective Projection
	float vertices[] = {
		-0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		0.5f,

		-0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		-0.5f,

		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,

		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,

		-0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		0.5f,
		-0.5f,
		-0.5f,
		-0.5f,

		-0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		0.5f,
		-0.5f,
		0.5f,
		-0.5f,
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);

	// Enlazar  Vertex Array Object
	glBindVertexArray(VAO);

	// 2.- Copiamos nuestros arreglo de vertices en un buffer de vertices para que OpenGL lo use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Posicion
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	glm::mat4 projection = glm::mat4(1);

	projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f); // FOV, Radio de aspecto,znear,zfar

	// le doy un color único a toda la pieza que estoy dibujando
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{

		Inputs(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.Use();
		glm::mat4 model = glm::mat4(1);
		glm::mat4 view = glm::mat4(1);
		glm::mat4 modelTemp = glm::mat4(1.0f);	// Temp guardo la posición del hombro
		glm::mat4 modelTemp2 = glm::mat4(1.0f); // Temp

		// View set up
		view = glm::translate(view, glm::vec3(movX, movY, movZ));
		view = glm::rotate(view, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");
		GLint uniformColor = ourShader.uniformColor;

		glUniformMatrix4fv(projecLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		glBindVertexArray(VAO);

		// 1. Model Bicep
        model = glm::rotate(model, glm::radians(hombro), glm::vec3(0.0f, 0.0, 1.0f));
        glm::mat4 pivotBicep = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f)); // Centro del bicep
        model = glm::scale(pivotBicep, glm::vec3(3.0f, 1.0f, 1.0f));
        color = glm::vec3(0.0f, 1.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36); // A

        // 2. Model Forearm
        // Partimos del centro del bicep (pivotBicep) y nos movemos a su extremo (1.5 en X)
        model = glm::translate(pivotBicep, glm::vec3(1.5f, 0.0f, 0.0f)); 
        model = glm::rotate(model, glm::radians(forearm), glm::vec3(0.0f, 1.0, 0.0f));
        glm::mat4 pivotForearm = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f)); // Centro del antebrazo
        model = glm::scale(pivotForearm, glm::vec3(2.0f, 1.0f, 1.0f));
        color = glm::vec3(1.0f, 0.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36); // B

        // 3. Model Hand
        // Partimos del centro del antebrazo y nos movemos a la muñeca
        model = glm::translate(pivotForearm, glm::vec3(1.0f, 0.0f, 0.0f)); 
        model = glm::rotate(model, glm::radians(hand), glm::vec3(1.0f, 0.0f, 0.0f)); 
        glm::mat4 pivotHand = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f)); // ¡MATRIZ CLAVE! Centro de la mano
        model = glm::scale(pivotHand, glm::vec3(1.0f, 1.0f, 1.0f));
        color = glm::vec3(0.0f, 0.0f, 1.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36); // C

        // ----------------- RAMIFICACIÓN: DEDOS -----------------

        // 4. Model Finger Base (Dedo normal)
        // Partimos de la mano (pivotHand), nos movemos al extremo y un poco hacia abajo en Y
        model = glm::translate(pivotHand, glm::vec3(0.7f, 0.3f, 0.0f)); 
		model = glm::rotate(model, glm::radians(finger), glm::vec3(0.0f, 0.0f, 1.0f)); 
        glm::mat4 pivotFinger = model; // Guardamos el origen de este dedo para su propia punta
        model = glm::scale(pivotFinger, glm::vec3(0.4f, 0.2f, 0.8f));
        color = glm::vec3(1.0f, 1.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36); // D

        // 5. Model Finger Tip (Punta del dedo)
        // Partimos de la base del dedo (pivotFinger), nos movemos hacia adelante
        model = glm::translate(pivotFinger, glm::vec3(0.4f, -0.0f, 0.0f)); 
		model = glm::rotate(model, glm::radians(finger_tip), glm::vec3(0.0f, 0.0f, 1.0f)); 
        model = glm::scale(model, glm::vec3(0.4f, 0.2f, 0.8f)); // Mismo tamaño que la base para que encaje
        color = glm::vec3(1.0f, 0.0f, 1.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36); // E

        // 6. Model Thumb Base (Pulgar)
        // VOLVEMOS a partir de la mano (pivotHand), no del dedo anterior. Nos movemos hacia arriba en Y.
        model = glm::translate(pivotHand, glm::vec3(0.5f, -0.3f, 0.0f)); 
		model = glm::rotate(model, glm::radians(thumb), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 pivotThumb = model; // Guardamos el origen del pulgar
        model = glm::scale(pivotThumb, glm::vec3(0.4f, 0.2f, 0.8f));
        color = glm::vec3(1.0f, 1.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36); // F

        // 7. Model Thumb Tip (Punta del pulgar)
        model = glm::translate(pivotThumb, glm::vec3(0.4f, 0.0f, 0.0f)); 
		model = glm::rotate(model, glm::radians(thumb_tip), glm::vec3(0.0f, 0.0f, 1.0f)); 
        model = glm::scale(model, glm::vec3(0.4f, 0.2f, 0.8f));
        color = glm::vec3(1.0f, 0.0f, 1.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36); // G

        glBindVertexArray(0);



		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return EXIT_SUCCESS;
}

void Inputs(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // GLFW_RELEASE
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movX += 0.08f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movX -= 0.08f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		movY += 0.08f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		movY -= 0.08f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		movZ -= 0.08f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		movZ += 0.08f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rot += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rot -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		hombro += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		hombro -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		forearm += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		forearm -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		hand += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		hand -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		finger -= 0.5f;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		thumb += 0.5f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		finger += 0.5f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		thumb -= 0.5f;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		finger_tip -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		thumb_tip += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		finger_tip += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		thumb_tip -= 1.0f;

	{
		hombro = 0.0f;
		forearm = 0.0f;
		hand = 0.0f;
	}
}

//
