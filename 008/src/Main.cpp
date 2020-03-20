#define STB_IMAGE_IMPLREMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "constants.h"
#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

// Window dimensions
const float toRadians = 3.1415926f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture steelTexture;
Texture concreteTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat lastFrameTime = 0.0f;

// Vertex Shader
static const char *vShader = "shaders/basics.vsh";

// Fragment Shader
static const char *fShader = "shaders/basics.fsh";

void calcAverageNormals(unsigned int *indices, unsigned int indiceCount, GLfloat *vertices,
			unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset) {
  for (size_t i = 0; i < indiceCount; i += 3) {
    unsigned int in0 = indices[i + 0] * vLength;
    unsigned int in1 = indices[i + 1] * vLength;
    unsigned int in2 = indices[i + 2] * vLength;
    glm::vec3 v1(vertices[in1 + 0] - vertices[in0 + 0],
		 vertices[in1 + 1] - vertices[in0 + 1],
		 vertices[in1 + 2] - vertices[in0 + 2]);
    glm::vec3 v2(vertices[in2 + 0] - vertices[in0 + 0],
		 vertices[in2 + 1] - vertices[in0 + 1],
		 vertices[in2 + 2] - vertices[in0 + 2]);
    glm::vec3 normal = cross(v1, v2);
    normal = glm::normalize(normal);

    in0 += normalOffset;
    in1 += normalOffset;
    in2 += normalOffset;
    vertices[in0 + 0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
    vertices[in1 + 0] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
    vertices[in2 + 0] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
  }
  for (size_t i = 0; i < verticeCount / vLength; i ++) {
    unsigned int nOffset = i * vLength + normalOffset;
    glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
    vec = glm::normalize(vec);
    vertices[nOffset + 0] = vec.x;
    vertices[nOffset + 1] = vec.y;
    vertices[nOffset + 2] = vec.z;
  }
}

void createObjects() {
  unsigned int indices[] = {
			    0, 3, 1,
			    1, 3, 2,
			    2, 3, 0,
			    0, 1, 2
  };
  GLfloat vertices[] = {
			// x     y     z        u     v       nx    ny    nz
			-1.0f, -1.0f, -0.6f,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			0.0f, -1.0f, 1.0f,     0.5f, 0.0f,   0.0f, 0.0f, 0.0f, 
			1.0f, -1.0f, -0.6f,     1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			0.0f, 1.0f, 0.0f,      0.5f, 1.0f,   0.0f, 0.0f, 0.0f
  };
  calcAverageNormals(indices, 12, vertices, 32, 8, 5);
  
  unsigned int cubeIndices[] = {
				0, 1, 2,
				2, 1, 3,
				6, 7, 5,
				6, 5, 4,

				9, 15, 11,
				15, 9, 13,
				8, 10, 14,
				12, 8, 14,

				20, 17, 16,
				20, 21, 17,
				18, 19, 22,
				22, 19, 23
  };
  GLfloat cubeVertices[] = {
			    -1.0, -1.0,  1.0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, 1.0,  1.0,     0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  -1.0,  1.0,    1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  1.0,  1.0,     1.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, -1.0, -1.0,    1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, 1.0, -1.0,     1.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  -1.0, -1.0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  1.0, -1.0,     0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 

			    -1.0, -1.0,  1.0,    0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, 1.0,  1.0,     0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  -1.0,  1.0,    1.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  1.0,  1.0,     1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, -1.0, -1.0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, 1.0, -1.0,     0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  -1.0, -1.0,    1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  1.0, -1.0,     1.0f, 1.0f,   0.0f, 0.0f, 0.0f,
			    
			    -1.0, -1.0,  1.0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, 1.0,  1.0,     0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  -1.0,  1.0,    1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  1.0,  1.0,     1.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, -1.0, -1.0,    1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    -1.0, 1.0, -1.0,     1.0f, 1.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  -1.0, -1.0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 
			    1.0,  1.0, -1.0,     0.0f, 1.0f,   0.0f, 0.0f, 0.0f
  };
  calcAverageNormals(cubeIndices, 36, cubeVertices, 192, 8, 5);

  Mesh *obj1 = new Mesh();
  obj1->createMesh(vertices, indices, 32, 12);
  meshList.push_back(obj1);

  Mesh *obj2 = new Mesh();
  obj2->createMesh(vertices, indices, 32, 12);
  meshList.push_back(obj2);
  
  Mesh *cube1 = new Mesh();
  cube1->createMesh(cubeVertices, cubeIndices, 192, 36);
  meshList.push_back(cube1);

  Mesh *cube2 = new Mesh();
  cube2->createMesh(cubeVertices, cubeIndices, 192, 36);
  meshList.push_back(cube2);

  Mesh *cube3 = new Mesh();
  cube3->createMesh(cubeVertices, cubeIndices, 192, 36);
  meshList.push_back(cube3);
  
}

void createShaders() {
  Shader *shader1 = new Shader();
  shader1->createFromFiles(vShader, fShader);
  shaderList.push_back(*shader1);
}

int main(int argc, char *argv[])
{
  mainWindow = Window(SCREEN_WIDTH, SCREEN_HEIGHT);
  mainWindow.initialize();

  createObjects();
  createShaders();

  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.1f);

  brickTexture = Texture("textures/brick-pixel.png");
  brickTexture.loadTexture();
  steelTexture = Texture("textures/steel.png");
  steelTexture.loadTexture();
  concreteTexture = Texture("textures/clay-pixel.png");
  concreteTexture.loadTexture();

  shinyMaterial = Material(1.0f, 32);
  dullMaterial = Material(0.3f, 4);


  mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
  			       0.1f, 0.3f,
  			       2.0f, -1.0f, -2.0f);

  unsigned int pointLightCount = 0;
  pointLights[0] = PointLight(0.0f, 1.0f, 0.0f,
			      0.1f, 1.0f,
			      -4.0f, 0.0f, 0.0f,
			      0.3f, 0.2f, 0.1f);
  pointLightCount ++;
  
  GLuint uniformProjection = 0;
  GLuint uniformModel = 0;
  GLuint uniformView = 0;
  GLuint uniformEyePosition = 0;
  GLuint uniformSpecularIntensity = 0;
  GLuint uniformShininess = 0;
  glm::mat4 projection = glm::perspective(45.0f,
					  mainWindow.getBufferWidth() /
					  mainWindow.getBufferHeight(),
					  0.1f, 100.0f);

  // loop until window closed
  while (!mainWindow.getShouldClose()) {
    GLfloat currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    
    // Get and handle user input events
    glfwPollEvents();

    camera.keyControl(mainWindow.getKeys(), deltaTime);
    camera.mouseControl(mainWindow.getXchange(), mainWindow.getYchange());

    // clear window
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderList[0].useShader();
    uniformModel = shaderList[0].getModelLocation();
    uniformProjection = shaderList[0].getProjectionLocation();
    uniformView = shaderList[0].getViewLocation();
    uniformEyePosition = shaderList[0].getEyePosition();
    uniformSpecularIntensity = shaderList[0].getSpecularIntensityLocation();
    uniformShininess = shaderList[0].getShininessLocation();

    shaderList[0].setDirectionalLight(&mainLight);
    shaderList[0].setPointLights(pointLights, pointLightCount);
    //    mainLight.useLight(uniformAmbientIntensity, uniformAmbientColor,
    //		       uniformDiffuseIntensity, uniformDirection);
    
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateView()));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
    
    glm::mat4 model(1.0);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    // model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.useTexture();
    dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    // meshList[0]->renderMesh();

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
    // model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));    
    concreteTexture.useTexture();
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    // meshList[1]->renderMesh();

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.useTexture();
    dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[2]->renderMesh();

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, -10.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.useTexture();
    dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);    
    meshList[3]->renderMesh();

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(6.0f, 0.0f, -2.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    concreteTexture.useTexture();
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);    
    meshList[4]->renderMesh();
    
    glUseProgram(0);

    if (currentTime - lastFrameTime >= 1.0f / FPS) {
      mainWindow.swapBuffers();
      lastFrameTime = currentTime;
    }
    lastTime = currentTime;
  }
  
  return 0;
}
