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
  const char* filename;//name of file to input
} ShaderInfo;


//create the shaders for your program
GLuint initShaders(ShaderInfo* shaders){
  
  ShaderInfo* shade=shaders;
  
  vector<GLuint> shadeList;//initialize list of shaders
  
  while(shade->type != GL_NONE){//loop through all the shaders in the list
    shadeList.push_back(createShader(shade->type,inputShader(shade->filename)));//adding shaders into the list
   // parseAttribUniform(attribList,uniformList,shade->AttribList,shade->UniformList);//makes list of attribute names
    ++shade;//incrementation
  }
  
  GLuint program=createProgram(shadeList);//creates the program linking to all the shaders
  
 	glUseProgram(program);
  
  glm::mat4 view;
  view = glm::lookAt(//position and direction of camera
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

//this funtion loads the shader from the vertex, fragments shaders 
const GLchar* inputShader(const char* filename){

  FILE* fshade = fopen(filename, "rb");//opens file
  
  if(!fshade){//check to see if file is opened
    fprintf(stderr,"unable to open file '%s'\n",filename);
    return NULL;
  }
  
  //neat way to get the length of the file
  fseek(fshade, 0, SEEK_END);
  long filesize=ftell(fshade);
  fseek(fshade, 0, SEEK_SET);
  
  
  //allocates memory for the file and read in the file 
  GLchar* shadingSource= new GLchar[filesize+1];//
  fread(shadingSource, 1, filesize, fshade);
  
  
  if(ftell(fshade) == 0){//checks to see if the file is empty
    fprintf(stderr, "File '%s' is empty.\n",filename);
    return NULL;
  }

  fclose(fshade);//closes file
  
  shadingSource[filesize] = 0;//neat way to set a '\0' at end of file
  
  return const_cast<const GLchar*>(shadingSource);//overloads the const so the value with change per file  
  
  //NOTE: if the file is unable to open or is empty this function will segmentation fault your program
}

//this function create your shader
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
    fprintf(stderr,"\nCompile failure in %u shader: %s\n Error message:\n%s\n",type,shadeInfo,infoLog);//prints information need to debug shaders
    delete[] infoLog;//memory management
  }
  return shader;//self explanatory
}

//this function creates the shading program we are going to link the shader too
GLuint createProgram(const vector<GLuint> shadeList){

  GLuint program = glCreateProgram();//creates your program
  
  for(GLuint i=0;i<shadeList.size();i++){glAttachShader(program,shadeList[i]);}//attaches shaders to program

  glBindAttribLocation(program, 0, "in_position");//binds the location an attribute to a program
  glBindAttribLocation(program, 1, "in_color");//binds the location an attribute to a program
  glLinkProgram(program);//links program to your program //weird
  
  GLint linkStatus;//status for linking variable
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);//returns the status of linking the program into the variable
  
  if(!linkStatus){//checks to see if your program linked to the program
    GLint logSize;//variable for size of the debug info
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);//returns the linking status into the variable
    
    GLchar *infoLog = new GLchar[logSize+1];//allocating memory for the debug info
    glGetProgramInfoLog(program,logSize,&logSize,infoLog);//returns the error messages into the variable infoLog
    
    fprintf(stderr,"\nShader linking failed: %s\n",infoLog);//prints your linking failed
    delete[] infoLog;//memory management
    
    for(GLuint i=0;i<shadeList.size();i++){glDeleteShader(shadeList[i]);}//memory management
  }
  return program;//self explanatory
}

#endif
