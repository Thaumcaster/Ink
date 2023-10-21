#include <iostream>
#include <GL/glut.h>

int wwidth = 800; // Initial window width
int wheight = 600; // Initial window height

void on_resize(int w, int h) {
    wwidth = w;
    wheight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert mouse coordinates to match OpenGL's coordinate system
        int oglX = x;
        int oglY = wheight - y; // Adjust y-coordinate to match OpenGL's coordinate system

        // Print the clicked coordinates
        std::cout << "Clicked at: X=" << oglX << ", Y=" << oglY << std::endl;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Point Drawer");

    // Set initial viewport and projection matrix
    glViewport(0, 0, wwidth, wheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, wwidth, 0, wheight);
    glMatrixMode(GL_MODELVIEW);

    // Register callback functions
    glutReshapeFunc(on_resize);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}
