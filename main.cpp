#include <iostream>
#include <fstream>
#include <chrono>


#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "glhelper.h"
#include "mesh.h"
#include "camera.h"


#define TINYOBJLOADER_IMPLEMENTATION 
#include "external/tiny_obj_loader.h"

// main obj
GLuint program_id;
GLuint VAO;
GLuint n_elements;
GLuint texture_id;

//quad
GLuint program_id_quad;
GLuint VAO_quad;
GLuint n_elements_quad;
GLuint texture_id_quad;
GLuint texture_heightmap;
GLuint textures_quad;

//background
GLuint program_id_background;
GLuint VAO_background;
GLuint n_elements_background;
GLuint texture_id_background;


//framebuffer
GLuint framebufferinit = 0;
GLuint renderedtexture = 1;
GLuint depthrenderbuffer = 2;

//temps
std::chrono::time_point<std::chrono::system_clock> start, end;

// camera
Camera cam;
Camera cam2;

//transform feedback
GLuint program_tf_id;
GLuint VBO[4]; // 0 and 1 are for the position 2 for the normal and 3 for the texcoor
GLuint query;
GLuint n_points;

void init()
{
  // create main obj version 1 avant la transform feedback
  /*{
    std::cout << "Creation of main object" << std::endl; 
    program_id = glhelper::create_program_from_file("shaders/simple.vs", "shaders/simple.fs");
    Mesh m = Mesh::load_from_file("data/Frankie/Frankie3.obj");
    m.apply_matrix(glm::mat4(
          1., 0., 0., 0.,
          0., 1., 0., 0.,
          0., 0., 1., 0.,
          0., 0., 0., 1.));
    n_elements=m.size_element();
    VAO = m.load_to_gpu();
    texture_id = glhelper::load_texture("data/Frankie/flyingsquirrel_skin_col.png");
  }*/
   // create transform feedback
 {
    std::cout << "Creation of main object" << std::endl; 
    //Maillage Frankie
    program_id = glhelper::create_program_from_file("shaders/simple.vs", "shaders/simple.fs");
    
    Mesh m = Mesh::load_from_file("data/Frankie/Frankie3.obj");
    n_elements=m.size_element();
    m.apply_matrix(glm::mat4(
                          0.2, 0., 0., 0.,
                          0., 0.2, 0., 0.,
                          0., 0., 0.2, 0.,
                          0., 0., 0., 1.));
    auto pos = m.position();
    auto normal = m.normal();
    auto texcoord = m.texcoord();
    n_points = pos.size()/3;
    texture_id = glhelper::load_texture("data/Frankie/flyingsquirrel_skin_col.png");

    // create transform feedback
    auto content =  glhelper::read_file("shaders/tf.vs");
    auto shader_id = glhelper::compile_shader(content.c_str(), GL_VERTEX_SHADER);
    program_tf_id = glCreateProgram();
    glAttachShader(program_tf_id, shader_id);

    const GLchar* attributes[] = {"pos"};
    glTransformFeedbackVaryings(program_tf_id, 1, attributes, GL_SEPARATE_ATTRIBS);
    glLinkProgram(program_tf_id);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.idx().size() * sizeof(GLuint), m.idx().data(), GL_STATIC_DRAW);

    glGenBuffers(4, VBO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), pos.data(), GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), pos.data(), GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, texcoord.size() * sizeof(GLfloat), texcoord.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenQueries(1, &query);
  }
   

  // create quad
  {
    //program_id_quad = glhelper::create_program_from_file("shaders/quad.vs", "shaders/quad.fs");
    program_id_quad = glhelper::create_program_from_file("shaders/basic.vs", "shaders/basic.gs", "shaders/basic.fs");
    Mesh quad = Mesh::create_grid(2);
    quad.apply_matrix(glm::mat4(
                          1., 0., 0., 0.,
                          0., 1., 0., 0.,
                          0., 0., 1., 0.,
                          0., 0., 0., 1.));
    n_elements_quad = quad.size_element();
    VAO_quad = quad.load_to_gpu();
    texture_id_quad = glhelper::load_texture("data/water-normal.png");
    texture_heightmap = glhelper::load_texture("data/water-heightmap.jpg");
   
  }

  // create background
  {
    program_id_background = glhelper::create_program_from_file("shaders/quad.vs", "shaders/background.fs");
    Mesh quad_background = Mesh::create_grid(2);
    glm::mat4 rotation = glm::mat4(
                          1., 0., 0., 0.,
                          0., cos(M_PI/2.), -sin(M_PI/2.), 0.,
                          0., sin(M_PI/2.), cos(M_PI/2.), 0.,
                          0., 0., 0., 1.) * glm::mat4(
                            cos(M_PI_2),-sin(M_PI_2),0,0,
                            sin(M_PI_2),cos(M_PI_2),0,0,
                            0,0,1,0,
                            0,0,0,1
                          ) ;
    quad_background.apply_matrix(rotation);
    n_elements_background = quad_background.size_element();
    VAO_background = quad_background.load_to_gpu();
    texture_id_background = glhelper::load_texture("data/sky.png");
    
   
  }



  {
    //initialisation framebuffer pour capturer une texure qui nous permettra de creer le reflet de frankie et du ciel dans l eau
  glGenFramebuffers(1, &framebufferinit);
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferinit);
  glGenTextures(1, &renderedtexture);
  glBindTexture(GL_TEXTURE_2D, renderedtexture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, cam.width(), cam.height(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedtexture, 0);
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cam.width(), cam.height());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout<<"error";
  }

  start = std::chrono::system_clock::now();

  glUseProgram(program_id_quad);
  textures_quad = glGetUniformLocation(program_id_quad, "textureSampler");
  glUniform1i(textures_quad,1);
  textures_quad = glGetUniformLocation(program_id_quad, "textureHeight");
  glUniform1i(textures_quad, 2);
  textures_quad = glGetUniformLocation(program_id_quad, "texturefromfbo");
  glUniform1i(textures_quad, 3);
 
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  glEnable(GL_DEPTH_TEST);            
}

static void display_callback()
{
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  glm::mat4 mvp = cam.projection() * cam.view();
  glm::mat4 mvp2 = cam.projection() * cam2.view();

  end = std::chrono::system_clock::now();
  float time = std::chrono::duration_cast<std::chrono::duration<float>>(start - end).count();
  

  // display obj sur le nouveau fbo
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferinit);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, cam2.width(), cam2.height());
    glUseProgram(program_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(VAO);
    GLint mvp_id = glGetUniformLocation(program_id, "MVP"); 
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp2[0][0]);
    glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);
  }

   // display background sur le nouveau fbo
  {
    glm::mat4 mvp3 = cam2.projection() * glm::mat4( 
               5.0, 0., 0., 0.,
               0., 5., 0.,0.,
              0., 0., 1.0, 0.1,
               0., 0., 0., 1.)*cam2.view()  ;
   
    glUseProgram(program_id_background);
    glBindTexture(GL_TEXTURE_2D, texture_id_background);
    glBindVertexArray(VAO_background);
    GLint mvp_id = glGetUniformLocation(program_id_background, "MVP"); 
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp3[0][0]);
    GLint myUniformLocation2 = glGetUniformLocation(program_id_quad, "time");
    glUniform1f(myUniformLocation2, time);
    glDrawElements(GL_TRIANGLES, n_elements_background, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);
  }
  
   // display background
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, cam.width(), cam.height());
    glm::mat4 mvp3 = cam2.projection() * glm::mat4( 
               5.0, 0., 0., 0.,
               0., 5., 0.,0.,
              0., 0., 1.0, 0.1,
               0., 0., 0., 1.)*cam2.view()  ;
    glUseProgram(program_id_background);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id_background);
    glBindVertexArray(VAO_background);
    GLint mvp_id = glGetUniformLocation(program_id_background, "MVP"); 
    GLint myUniformLocation2 = glGetUniformLocation(program_id_quad, "time");
    glUniform1f(myUniformLocation2, time);
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp3[0][0]);
    glDrawElements(GL_TRIANGLES, n_elements_background, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);
  }
  // displacement with tf
  {	
    glEnable(GL_RASTERIZER_DISCARD);
    glUseProgram(program_tf_id);
    GLint myUniformLocationtf = glGetUniformLocation(program_tf_id, "time");
    glUniform1f(myUniformLocationtf, time);
    glBindVertexArray(VAO);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[1]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,  VBO[0]);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,0,0);
    glBeginTransformFeedback(GL_POINTS);
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);                
    glDrawArrays(GL_POINTS, 0, n_points);
    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);                         
    GLuint n;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &n);              
    glFlush();
    glDisableVertexAttribArray(1);                                                
    glDisable(GL_RASTERIZER_DISCARD);
    std::swap(VBO[0], VBO[1]);
  }

  // Draw Frankie
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, cam.width(), cam.height());
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(program_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint mvp_id = glGetUniformLocation(program_id, "MVP");
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]); 
    glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0); 
  }


    // display quad
  {
    glUseProgram(program_id_quad);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id_quad);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_heightmap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, renderedtexture);
    glBindVertexArray(VAO_quad);
    GLint mvp_id = glGetUniformLocation(program_id_quad, "MVP");
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]); 
    GLint camposition_id = glGetUniformLocation(program_id_quad, "POS_CAM");
    glUniform3fv(camposition_id, 1, &cam.position()[0]);
    GLint myUniformLocation2 = glGetUniformLocation(program_id_quad, "time");
    glUniform1f(myUniformLocation2, time);
    glDrawElements(GL_TRIANGLES, n_elements_quad, GL_UNSIGNED_INT, 0);   
  }

  glBindVertexArray(0);
  glutSwapBuffers ();
}

static void keyboard_callback(unsigned char key, int, int)
{
  switch (key)
  {
    case 'q':
    case 'Q':
    case 27:
      exit(0);
  }
  glutPostRedisplay();
}

static void reshape_callback(int width, int height)
{
  cam.common_reshape(width,height);

  glViewport(0,0, width, height); 
  glutPostRedisplay();
}


static void mouse_callback (int button, int action, int x, int y)
{
  cam.common_mouse(button, action, x, y);

  glutPostRedisplay();
}

static void motion_callback(int x, int y)
{
  cam.common_motion(x, y);
  glutPostRedisplay();
}

static void timer_callback(int)
{
  glutTimerFunc(25, timer_callback, 0);
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  glutInitContextVersion(3, 3); 
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(cam.width(), cam.height());
  glutCreateWindow("opengl");
  glutDisplayFunc(display_callback);
  glutMotionFunc(motion_callback);
  glutMouseFunc(mouse_callback);
  glutKeyboardFunc(keyboard_callback);
  glutReshapeFunc(reshape_callback);
  glutTimerFunc(25, timer_callback, 0);

  glewExperimental=true;
  glewInit();

  init();
  glutMainLoop();

  return 0;
}
