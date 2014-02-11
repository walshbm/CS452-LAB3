#ifndef INITSHADERS_H_
#define INITSHADERS_H_
//functions for loading in shaders
#include <SDL2/SDL.h>
#include <GL/glew.h>
//#include "GL/freeglut.h"
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstdio>
#include <iostream>
using namespace std;

//function prototypes
GLuint createShader(GLenum type, const GLchar* shadeSource);
const GLchar* inputShader(const char* filename);
GLuint createProgram(const vector<GLuint> shadeList);
void transform(GLuint program);


typedef struct{
  GLenum type;// GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
  const char* filename;//file that is input
} ShaderInfo;


//create the shaders for your program
GLuint initShaders(ShaderInfo* shaders){
  
  ShaderInfo* shade=shaders;
  
  vector<GLuint> shadeList;//initializes the list of shaders
  
  while(shade->type != GL_NONE){
    shadeList.push_back(createShader(shade->type,inputShader(shade->filename)));//adds shaders into the list
   // parseAttribUniform(attribList,uniformList,shade->AttribList,shade->UniformList);//makes list of attribute names
    ++shade;
  }
  
  GLuint program=createProgram(shadeList);//creates a program to link all of the shaders
  
 	glUseProgram(program);
  
  glm::mat4 view;
  view = glm::lookAt(//initializes position and direction of camera
 	  		glm::vec3(0.0f, 0.0f, 50.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
  GLint tempLoc = glGetUniformLocation(program, "viewMatrix");//Matrix that handles the camera movement
  glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &view[0][0]);
  
  glm::mat4 mainProjMatrix;
  mainProjMatrix = glm::perspective(57.0,1.0,.1,500.0);//Matrix that handle the orthographic or perspective viewing
  tempLoc = glGetUniformLocation(program, "Matrix");
  glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &mainProjMatrix[0][0]);
  
  return program;
   
}

//funtion loads the shader from the vertex, fragments shaders 
const GLchar* inputShader(const char* filename){

  FILE* fshade = fopen(filename, "rb");
  
  if(!fshade){//checks to see if the file is already opened
    fprintf(stderr,"unable to open file '%s'\n",filename);
    return NULL;
  }
  
  fseek(fshade, 0, SEEK_END);
  long filesize=ftell(fshade);
  fseek(fshade, 0, SEEK_SET);
  
  
  //allocates memory for the file and reads in the file 
  GLchar* shadingSource= new GLchar[filesize+1];//
  fread(shadingSource, 1, filesize, fshade);
  
  
  if(ftell(fshade) == 0){//checks to see if the file is empty
    fprintf(stderr, "File '%s' is empty.\n",filename);
    return NULL;
  }

  fclose(fshade);
  
  shadingSource[filesize] = 0;//way to put a '\0' at the end of the file
  
  return const_cast<const GLchar*>(shadingSource);//overloads the const so the value with change per file  
  
  //NOTE: if the file is unable to open or is empty this function will segmentation fault your program
}

//this function creates your shader
GLuint createShader(GLenum type, const GLchar* shadeSource){
  
  GLuint shader = glCreateShader(type);//create shader based on type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
  glShaderSource(shader, 1, &shadeSource, NULL);//loads the source code of the file into the shader
  
  glCompileShader(shader);//compiles a shader object
  
  GLint compileStatus;//status of the compilation variable
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);//returns the comiple status into the variable
  
  if(!compileStatus){//checks to see if the shader compiled
    GLint logSize;//variable for size of the debug info
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);//returns the size of the the source file into the variable
    
    GLchar* infoLog = new GLchar[logSize+1];//allocating memory for the debug info
    glGetShaderInfoLog(shader,logSize,&logSize,infoLog);//returns the error messages into the variable infoLog
    
    const char *shadeInfo= NULL;//char array for what shader that is having an error
    switch(type){//way to get what shader has the error
      case GL_VERTEX_SHADER: shadeInfo = "vertex"; break;
      case GL_GEOMETRY_SHADER_EXT: shadeInfo = "geometric"; break;
      case GL_FRAGMENT_SHADER: shadeInfo = "fragment"; break;
    }
    fprintf(stderr,"\nCompile failure in %u shader: %s\n Error message:\n%s\n",type,shadeInfo,infoLog);//prints information needed to debug shaders
    delete[] infoLog;
  }
  return shader;
}

//this function creates the shading program we are going to link the shader too
GLuint createProgram(const vector<GLuint> shadeList){

  GLuint program = glCreateProgram();//creates the program
  
  for(GLuint i=0;i<shadeList.size();i++){glAttachShader(program,shadeList[i]);}//attaches shaders to the program

  glBindAttribLocation(program, 0, "in_position");//binds the location an attribute to a program
  glBindAttribLocation(program, 1, "in_color");//binds the location an attribute to a program
  glLinkProgram(program);//links program to the program
  
  GLint linkStatus;//status for linking variable
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);//returns the status of linking the program into the variable
  
  if(!linkStatus){//checks to see if your program linked to the program
    GLint logSize;//variable for size of the debug info
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);//returns the linking status into the variable
    
    GLchar *infoLog = new GLchar[logSize+1];//allocating memory for the debug info
    glGetProgramInfoLog(program,logSize,&logSize,infoLog);//returns the error messages into the variable infoLog
    
    fprintf(stderr,"\nShader linking failed: %s\n",infoLog);//prints your linking failed
    delete[] infoLog;
    
    for(GLuint i=0;i<shadeList.size();i++){glDeleteShader(shadeList[i]);}
  }
  return program;
}

#endif
