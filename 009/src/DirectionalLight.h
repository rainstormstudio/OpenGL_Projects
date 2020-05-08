#pragma once
#include "Light.h"

class DirectionalLight : public Light {
public:
  DirectionalLight();

  DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight,
		   GLfloat red, GLfloat green, GLfloat blue,
		   GLfloat aIntensity, GLfloat dIntensity,
		   GLfloat xDir, GLfloat yDir, GLfloat zDir);

  void useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
		GLuint diffuseIntensityLocation, GLuint directionLocation);

  glm::mat4 calculateLightTransform();
  
  ~DirectionalLight();
  
private:
  glm::vec3 direction;
};
