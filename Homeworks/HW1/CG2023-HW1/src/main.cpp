#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include "camera.h"
#include "opengl_context.h"
#include "utils.h"

#define ANGLE_TO_RADIAN(x) (float)((x)*M_PI / 180.0f) 
#define RADIAN_TO_ANGEL(x) (float)((x)*180.0f / M_PI) 

#define CIRCLE_SEGMENT 64

#define ROTATE_SPEED 1.0f
#define FLYING_SPEED ROTATE_SPEED / 20.f

#define RED 0.905f, 0.298f, 0.235f
#define BLUE 0.203f, 0.596f, 0.858f
#define GREEN 0.18f, 0.8f, 0.443f


void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // There are three actions: press, release, hold(repeat)
  if (action == GLFW_REPEAT) 
      return;

  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  /* TODO#4-1: Detect key-events, perform rotation or fly
   *       1. Use switch && case to find the key you want.
   *       2. Press "SPACE" for fly up, fly forward and wing rotate meanwhile. 
   *       3. Press "GLFW_KEY_LEFT" for turn left.
   *       4. Press "GLFW_KEY_RIGHT " for turn right.
   * Hint:
   *       glfw3's key list (https://www.glfw.org/docs/3.3/group__keys.html)
   *       glfw3's action codes (https://www.glfw.org/docs/3.3/group__input.html#gada11d965c4da13090ad336e030e4d11f)
   * Note:
   *       You should finish rendering your airplane first.
   *       Otherwise you will spend a lot of time debugging this with a black screen.
   */
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  //OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW1 - `your student id`"
   *        Ex. HW1 - 312550000 
   */
  glfwSetWindowTitle(window, "HW1 - 312553024");
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}

/* TODO#2: Render body, wing, tail
 * Hint:
 *       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
 *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
 *       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
 *       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
 *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
 * Note:
 *       You can refer to ppt "Draw Cylinder" page and `CIRCLE_SEGMENT`
 *       You can use color defined above
 */

void draw_cylinder(float radius, float height, int segments) {
  // Draw a cylinder with top and bottom faces and the specified parameters
  float angleIncrement = 2.0f * M_PI / segments;

  // Draw the side faces
  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i <= segments; i++) {
    float angle = static_cast<float>(i) * angleIncrement;
    float x = radius * cos(angle);
    float z = radius * sin(angle);

    glNormal3f(x, 0.0f, z);  // Define the normal for lighting

    float y1 = -height / 2.0f;
    float y2 = height / 2.0f;

    glVertex3f(x, y1, z);  // Bottom
    glVertex3f(x, y2, z);  // Top
  }
  glEnd();

  // Draw the top face
  glBegin(GL_POLYGON);
  glNormal3f(0.0f, 1.0f, 0.0f);  // Define the normal for lighting
  for (int i = 0; i < segments; i++) {
    float angle = static_cast<float>(i) * angleIncrement;
    float x = radius * cos(angle);
    float z = radius * sin(angle);
    glVertex3f(x, height / 2.0f, z);
  }
  glEnd();

  // Draw the bottom face
  glBegin(GL_POLYGON);
  glNormal3f(0.0f, -1.0f, 0.0f);  // Define the normal for lighting
  for (int i = 0; i < segments; i++) {
    float angle = static_cast<float>(i) * angleIncrement;
    float x = radius * cos(angle);
    float z = radius * sin(angle);
    glVertex3f(x, -height / 2.0f, z);
  }
  glEnd();
}

void render_body() {
  // Render the body (cylinder) with top and bottom faces
  glPushMatrix();
  glTranslatef(0.0f, 0.5f, 0.0f);            // Translate to the desired position
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);        // Rotate the body by 90 degrees around the X-axis
  glColor3f(BLUE);                            // Set the color to red
  glEnable(GL_NORMALIZE);
  draw_cylinder(0.5f, 4.0f, CIRCLE_SEGMENT);  // Render the body using drawCylinder
  glDisable(GL_NORMALIZE);
  glPopMatrix();
}

void draw_rectangle(float length, float width, float height) {
  // Half-dimensions for easier calculations
  float halfLength = length / 2.0f;
  float halfWidth = width / 2.0f;
  float halfHeight = height / 2.0f;

  // Draw the wing as a rectangular cuboid
  glBegin(GL_QUADS);

  // Front face
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-halfLength, -halfHeight, halfWidth);
  glVertex3f(halfLength, -halfHeight, halfWidth);
  glVertex3f(halfLength, halfHeight, halfWidth);
  glVertex3f(-halfLength, halfHeight, halfWidth);

  // Back face
  glNormal3f(0.0f, 0.0f, -1.0f);
  glVertex3f(-halfLength, -halfHeight, -halfWidth);
  glVertex3f(halfLength, -halfHeight, -halfWidth);
  glVertex3f(halfLength, halfHeight, -halfWidth);
  glVertex3f(-halfLength, halfHeight, -halfWidth);

  // Right face
  glNormal3f(1.0f, 0.0f, 0.0f);
  glVertex3f(halfLength, -halfHeight, halfWidth);
  glVertex3f(halfLength, -halfHeight, -halfWidth);
  glVertex3f(halfLength, halfHeight, -halfWidth);
  glVertex3f(halfLength, halfHeight, halfWidth);

  // Left face
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glVertex3f(-halfLength, -halfHeight, halfWidth);
  glVertex3f(-halfLength, -halfHeight, -halfWidth);
  glVertex3f(-halfLength, halfHeight, -halfWidth);
  glVertex3f(-halfLength, halfHeight, halfWidth);

  // Top face
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-halfLength, halfHeight, halfWidth);
  glVertex3f(halfLength, halfHeight, halfWidth);
  glVertex3f(halfLength, halfHeight, -halfWidth);
  glVertex3f(-halfLength, halfHeight, -halfWidth);

  // Bottom face
  glNormal3f(0.0f, -1.0f, 0.0f);
  glVertex3f(-halfLength, -halfHeight, halfWidth);
  glVertex3f(halfLength, -halfHeight, halfWidth);
  glVertex3f(halfLength, -halfHeight, -halfWidth);
  glVertex3f(-halfLength, -halfHeight, -halfWidth);

  glEnd();
}

void render_wings() {
  // Render the wings of airplane
  glPushMatrix();
  glTranslatef(2.0f, 0.5f, 0.0f);             // Translate to the desired position
  glColor3f(RED);                            // Set the color to red
  glEnable(GL_NORMALIZE);
  draw_rectangle(4.0f, 1.0f, 0.5f);  // Render the body using drawCylinder
  glDisable(GL_NORMALIZE);
  glPopMatrix();

  // Render the wings of airplane
  glPushMatrix();
  glTranslatef(-2.0f, 0.5f, 0.0f);  // Translate to the desired position
  glColor3f(RED);                    // Set the color to red
  glEnable(GL_NORMALIZE);
  draw_rectangle(4.0f, 1.0f, 0.5f);  // Render the body using drawCylinder
  glDisable(GL_NORMALIZE);
  glPopMatrix();
}

void draw_triangle(float bottomEdge, float height1, float height2) {
  glBegin(GL_TRIANGLES);
  // Face 1
  glNormal3f(0.0f, 0.0f, 1.0f);                   // Normal pointing along the Z-axis
  glVertex3f(0.0f, 0.0f, 0.0f);                   // Top vertex
  glVertex3f(bottomEdge / 2.0f, 0.0f, height1);   // Bottom-left vertex
  glVertex3f(-bottomEdge / 2.0f, 0.0f, height1);  // Bottom-right vertex

  // Face 2
  glNormal3f(0.0f, 0.0f, 1.0f);                   // Normal pointing along the Z-axis
  glVertex3f(0.0f, 0.0f, 0.0f);                   // Top vertex
  glVertex3f(0.0f, -height2, height1);            // Bottom-left vertex
  glVertex3f(bottomEdge / 2.0f, 0.0f, height1);   // Bottom-right vertex

  // Face 3
  glNormal3f(0.0f, 0.0f, 1.0f);                   // Normal pointing along the Z-axis
  glVertex3f(0.0f, 0.0f, 0.0f);                   // Top vertex
  glVertex3f(0.0f, -height2, height1);            // Bottom-left vertex
  glVertex3f(-bottomEdge / 2.0f, 0.0f, height1);   // Bottom-right vertex

  // Face 4
  glNormal3f(0.0f, 0.0f, 1.0f);                   // Normal pointing along the Z-axis
  glVertex3f(-bottomEdge / 2.0f, 0.0f, height1);  // Top vertex
  glVertex3f(0.0f, -height2, height1);            // Bottom-left vertex
  glVertex3f(bottomEdge / 2.0f, 0.0f, height1);   // Bottom-right vertex
  glEnd();
}

void render_tail() {
  // Render the tail of the airplane
  glPushMatrix();
  // Translate to the correct position relative to the body
  glTranslatef(0.0f, 0.5f, 2.0f);
  // Rotate the tail if needed
  // glRotatef(angle, 1.0f, 0.0f, 0.0f);  // Rotate the tail around the X-axis

  // Set the color (e.g., GREEN or your desired color)
  glColor3f(GREEN);

  // Enable normalization
  glEnable(GL_NORMALIZE);

  // Draw the tail as a tetrahedron (adjust dimensions as needed)
  draw_triangle(2.0f, 1.0f, 0.5f);

  // Disable normalization if not needed elsewhere
  glDisable(GL_NORMALIZE);

  glPopMatrix();
}


void light() {
  GLfloat light_specular[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_diffuse[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_ambient[] = {0.4, 0.4, 0.4, 1};
  GLfloat light_position[] = {50.0, 75.0, 80.0, 1.0};
  // z buffer enable
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  // set light property
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();

  // Init Camera helper
  Camera camera(glm::vec3(0, 5, 10));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.getProjectionMatrix());
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camera.getViewMatrix());


//#ifndef DISABLE_LIGHT   
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    light();
//#endif

    /* TODO#4-2: Update 
     *       You may update position and orientation of airplane here or not.
     *       Feel free to not follow TA's structure. However, don't violate the spec. 
     * 
     * Hint: 
     * Note:
     *       You can use `ROTATE_SPEED` and `FLYING_SPEED` as the speed constant. 
     *       If the rotate/flying speed is too slow or too fast, please change `ROTATE_SPEED` or `FLYING_SPEED` value.
     *       You should finish keyCallback first.
     */

    // Render a white board
    glPushMatrix();
    glScalef(3, 1, 3);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-5.0f, 0.0f, -5.0f);
    glVertex3f(-5.0f, 0.0f, 5.0f);
    glVertex3f(5.0f, 0.0f, -5.0f);
    glVertex3f(5.0f, 0.0f, 5.0f);
    glEnd();
    glPopMatrix();
    

    /* TODO#3: Render the airplane    
     *       1. Render the body.
     *       2. Render the wings.(Don't forget to assure wings rotate at the center of body.)
     *       3. Render the tail.
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml) 
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     * Note:
     *       You may implement functions for drawing components of airplane first
     *       You should try and think carefully about changing the order of rotate and translate
     */

    // printf("Render!");
    render_body();
    render_wings();
    render_tail();
    

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
