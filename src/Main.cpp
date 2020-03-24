//
// Created by jlebas01 on 15/01/2020.
//

#include <iostream>
#include <GL/glew.h>
#include <graphic/SDLWindowManager.hpp>
#include <object/Sphere.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <unistd.h>
#include <iostream>
#include <tool/Program.hpp>
//#include <glimac/Program.hpp>
//#include <glimac/FilePath.hpp>


int main(int argc, char *argv[]) {

    // Initialize SDL and open a window
    int window_width = 800;
    int window_height = 800;
    graphic::SDLWindowManager windowManager(window_width, window_height, "Interpolation");
    // windowManager(width, height, title)

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if (GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    //INITIALIZATION

    /*----------------------SHADER-----------------------*/

    tool::Program program = tool::loadProgram("../shaders/3D.vs.glsl",
                                              "../shaders/normal.fs.glsl");
    program.use();


    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    object::Sphere sphere = object::Sphere(1, 32, 16);
    object::Sphere sphere2 = object::Sphere(1, 32, 16);
    object::Sphere interpol = object::Sphere(1, 32, 16);

    /* Set the sphere 1 */
    glm::vec<3, GLfloat> center1 = sphere.getCenter();
    sphere.translator(center1.x - 0.8, center1.y - 0.7, center1.z);

    /* Set the sphere 2 */
    glm::vec<3, GLfloat> center2 = sphere2.getCenter();
    sphere2.translator(center2.x + 0.5, center2.y + 0.6, center2.z + 0.5);
    sphere2.dilator(1.5);


    /*--------------------Matrix-------------------------*/

    GLint indexMVPMatrix = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLint indexMVMatrix = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    GLint indexNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;
    glm::mat4 MVPMatrix;

    bool done = false;

    sphere.update();

    interpol.update();

    sphere2.update();


    // Application loop:
    GLfloat nb_step = 80;
    GLfloat alpha = 0.0;
    while (!done) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Event loop:
        SDL_Event e;
        while (windowManager.pollEvent(e)) {
            if (e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }

        /* Draw the interpol sphere in red */

        program = tool::loadProgram("../shaders/3D.vs.glsl",
                                      "../shaders/normal.fs.glsl");
        program.use();

        ProjMatrix = glm::perspective(glm::radians(70.f), (float) (window_width / window_height), 0.1f, 100.f);
        MVMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0, -5));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        MVPMatrix = ProjMatrix * MVMatrix;
        glUniformMatrix4fv(indexMVPMatrix, 1, GL_FALSE, value_ptr(MVPMatrix));
        glUniformMatrix4fv(indexMVMatrix, 1, GL_FALSE, value_ptr(MVMatrix));
        glUniformMatrix4fv(indexNormalMatrix, 1, GL_FALSE, value_ptr(NormalMatrix));
        interpol = sphere;


        interpol.interpolator(sphere2, alpha);
        interpol.update();
        interpol.render();

        /* Draw init sphere in purple */

        program = tool::loadProgram("../shaders/3D.vs.glsl","../shaders/normal3.fs.glsl");
        program.use();

        ProjMatrix = glm::perspective(glm::radians(70.f), (float) (window_width / window_height), 0.1f, 100.f);
        MVMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0, -5));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        MVPMatrix = ProjMatrix * MVMatrix;
        glUniformMatrix4fv(indexMVPMatrix, 1, GL_FALSE, value_ptr(MVPMatrix));
        glUniformMatrix4fv(indexMVMatrix, 1, GL_FALSE, value_ptr(MVMatrix));
        glUniformMatrix4fv(indexNormalMatrix, 1, GL_FALSE, value_ptr(NormalMatrix));

        sphere.update();
        sphere.render();

        /* Draw objective sphere in yellow*/

        program = tool::loadProgram("../shaders/3D.vs.glsl","../shaders/normal2.fs.glsl");
        program.use();

        ProjMatrix = glm::perspective(glm::radians(70.f), (float) (window_width / window_height), 0.1f, 100.f);
        MVMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0, -5));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        MVPMatrix = ProjMatrix * MVMatrix;
        glUniformMatrix4fv(indexMVPMatrix, 1, GL_FALSE, value_ptr(MVPMatrix));
        glUniformMatrix4fv(indexMVMatrix, 1, GL_FALSE, value_ptr(MVMatrix));
        glUniformMatrix4fv(indexNormalMatrix, 1, GL_FALSE, value_ptr(NormalMatrix));

        sphere2.update();
        sphere2.render();

        usleep(80000);

        if (alpha <= 1.0) {
            std::cout << alpha << std::endl;
            alpha += 1.0 / nb_step;
        } else {
            sleep(5);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;
        }

        // Update the display with double buffering
        windowManager.swapBuffers();
    }

    std::exit(EXIT_SUCCESS);
}
