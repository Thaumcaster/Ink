#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <random>

#define GL_RGB GL_NOP

const int WWIDTH = 1000, WHEIGHT = 1000;
enum drawStates{ point, line, bezier, circle, easyCircle, triangle, quadrilateral, parallelogram };

int drawState = point;
bool filled = false;
bool erase = false;
bool freeDraw = false;
bool randomColor = false;
bool cycleColor = false;
bool fileFound = true;

float c[] = {1,1,1};
float ec[] = {0,0,0};
float pc[] = {1,1,1};
int previousPoint[2];
int currentPoint[2];

int linePoints[2][2];
int pPoints[2][2];
int curvePoints[5][2]; // First and second points are start and end, other 3 are reserved for control points
int cPoints[2][2];
int tPoints[3][2];
int qPoints[4][2];
int cCount = 0;
int tCount = 0;
int qCount = 0;
int lineCount = 0;
int pCount = 0;
int controlCount = 0;
int curveCount = 0;
int brushSize = 3;
int cycleCount = 0;
int SLEEP_TIME = 500; // Time in milliseconds

void help(){
    std::cout << "Colors" << "\n";
    std::cout << "-----------------------" << "\n";
    std::cout << "[`]: Random color toggle" << "\n" << "[~]: Cycle color toggle" << "\n" << "[1]: Red" << "\n" 
              << "[2]: Green" << "\n" << "[3]: Blue" << "\n" << "[4]: Yellow" << "\n" 
              << "[5]: Magenta" << "\n" << "[6]: Cyan" << "\n" << "[7]: Alt Red" << "\n" 
              << "[8]: Alt Green" << "\n" << "[9]: Alt Blue" << "\n" << "[0]: White" 
              << "\n" << "\n";

    std::cout << "Function Keys" << "\n";
    std::cout << "-----------------------" << "\n";
    std::cout << "While not a key, you can screenshot part of your window by moving the application over that portion" << "\n"
              << "of the screen, minimizing it, then reopening it. Immeditely save the image to ensure the window does" << "\n"
              << "not refresh the page and remove the image. A refresh before saving the image is likely and not consistent.";
    std::cout << "[ ]: Quits the program"<< "\n" << "[q]: Saves the window contents into program directory" << "\n" 
              << "[w]: Clears the screen" << "\n" << "[t]: Loads image from program directory" << "\n"
              << "[a]: Displays help text (what you are reading now)" << "\n" << "[s]: Toggles shape-fill" << "\n" 
              << "[d]: Decreases brush size by 1" << "\n" << "[D]: Decreases brush size by 10" << "\n" 
              << "[f]: Increases brush size by 1" << "\n" << "[F]: Increases brush size by 10" << "\n"
              << "[g]: Decreases color timer by 1" << "\n" << "[G]: Decreases color timer by 10" << "\n" 
              << "[h]: Increases color timer by 1" << "\n" << "[H]: Increases color timer by 10"
              << "\n" << "\n";

    std::cout << "Drawing Keys" << "\n";
    std::cout << "-----------------------" << "\n";
    std::cout << "[z]: Bezier curves with one, two, and three control points. Changes depending on how many times you press the key." << "\n" 
              << "[x]: Point draw" << "\n" << "[X]: Line draw (free compatible)" << "\n" 
              << "[c]: Cirle draw (free/shape-fill compatible)" << "\n" << "[C]: Easy cirle draw (free (uses a point previously placed or (0,0))/shape-fill compatible)" << "\n"
              << "[v]: Triangle draw (free/shape-fill compatible)" << "\n"
              << "[b]: Quadrilateral draw (free/shape-fill compatible)" << "\n" << "[B]: Parallelogram draw (free/shape-fill compatible)" << "\n"
              << "[e]: Eraser toggle (Changes color to black then to previous color upon toggle)" << "\n" 
              << "[r]: Free draw" << "\n" 
              << "[S]: Colors entire screen with the selected color" << "\n"
              << "\n" << "\n";

}

void copy(float destination[], float source[]){
    for (int i = 0; i < 3; i++){
        destination[i] = source[i];
    }
}

void drawPoint(float x_, float y_){
    glPointSize(brushSize);
    glColor3fv(c);
    glBegin(GL_POINTS);
        glVertex2i(x_,y_);
    glEnd();
}

void drawAdjPoint(float x_, float y_){
    glColor3fv(c);
    glBegin(GL_POINTS);
        glVertex2i(x_,y_);
    glEnd();
}

void drawLine(int v1[2], int v2[2]){
    glPointSize(brushSize);
    glLineWidth(brushSize);
    glColor3fv(c);
    glBegin(GL_LINES);
        glVertex2i(v1[0],v1[1]);
        glVertex2i(v2[0],v2[1]);
    glEnd();
}

void drawAdjLine(int v1[2], int v2[2]){
    glColor3fv(c);
    glBegin(GL_LINES);
        glVertex2i(v1[0],v1[1]);
        glVertex2i(v2[0],v2[1]);
    glEnd();
}

void drawAdjLine(int v1x, int v1y, int v2x, int v2y){
    glColor3fv(c);
    glBegin(GL_LINES);
        glVertex2i(v1x,v1y);
        glVertex2i(v2x,v2y);
    glEnd();
}

void drawCurve(){
    if (controlCount == 1){
        for (int i=0; i<=1001; ++i){
            float t = (i/1002.0);
            float x = (std::pow(1-t,2)*curvePoints[0][0]) + 
                      (2*t*(1-t)*curvePoints[2][0]) + 
                      (std::pow(t,2)*curvePoints[1][0]);

            float y = (std::pow(1-t,2)*curvePoints[0][1]) + 
                      (2*t*(1-t)*curvePoints[2][1]) + 
                      (std::pow(t,2)*curvePoints[1][1]);

            glColor3fv(c);
            glPointSize(brushSize);
            glBegin(GL_POINTS);
                glVertex2f(x,y);
            glFlush();
            glutPostRedisplay();
            glEnd();
        }
    }
    if (controlCount == 2){
        for (int i=0; i<=1001; ++i){
            float t = (i/1002.0);
            float x = (std::pow(1-t,3)*curvePoints[0][0]) + 
                      (3*t*std::pow(1-t,2)*curvePoints[2][0]) + 
                      (3*(1-t)*std::pow(t,2)*curvePoints[3][0]) + 
                      (std::pow(t,3)*curvePoints[1][0]);

            float y = (std::pow(1-t,3)*curvePoints[0][1]) + 
                      (3*t*std::pow(1-t,2)*curvePoints[2][1]) + 
                      (3*(1-t)*std::pow(t,2)*curvePoints[3][1]) + 
                      (std::pow(t,3)*curvePoints[1][1]);

            glColor3fv(c);
            glPointSize(brushSize);
            glBegin(GL_POINTS);
                glVertex2f(x,y);
            glFlush();
            glutPostRedisplay();
            glEnd();
        }
    }
    if (controlCount == 3){
        for (int i=0; i<=1001; ++i){
            float t = (i/1002.0);
            float x = (std::pow(t,4)*curvePoints[0][0]) + 
                      (4*std::pow(t,3)*(1-t)*curvePoints[2][0]) + 
                      (6*std::pow(t,2)*std::pow(1-t,2)*curvePoints[3][0]) + 
                      (4*t*std::pow(1-t,3)*curvePoints[4][0]) + 
                      (std::pow(1-t,4)*curvePoints[1][0]);

            float y = (std::pow(t,4)*curvePoints[0][1]) + 
                      (4*std::pow(t,3)*(1-t)*curvePoints[2][1]) + 
                      (6*std::pow(t,2)*std::pow(1-t,2)*curvePoints[3][1]) + 
                      (4*t*std::pow(1-t,3)*curvePoints[4][1]) + 
                      (std::pow(1-t,4)*curvePoints[1][1]);

            glColor3fv(c);
            glPointSize(brushSize);
            glBegin(GL_POINTS);
                glVertex2f(x,y);
            glFlush();
            glutPostRedisplay();
            glEnd();
        }
    }
}

void display(){
    
}

void init(){
    gluOrtho2D(0,WWIDTH,0,WHEIGHT);
}

void save(){
    int* buffer = new int[WWIDTH * WHEIGHT * 3];
    glReadPixels(0, 0, WWIDTH, WHEIGHT, GL_BGR, GL_UNSIGNED_BYTE, buffer);
    FILE *out = fopen("image.tga", "w");
    short TGAhead[] = {0, 2, 0, 0, 0, 0, WWIDTH, WHEIGHT, 24};
    fwrite(&TGAhead, sizeof(TGAhead), 1, out);
    fwrite(buffer, 3 * WWIDTH * WHEIGHT, 1, out);
    fclose(out);
}

void load(){
    FILE *out = fopen("image.tga", "r");
    if (out){
    short TGAhead[] = {0, 2, 0, 0, 0, 0, WWIDTH, WHEIGHT, 24};
    int* buffer = new int[WWIDTH * WHEIGHT * 3];
    fread(&TGAhead, sizeof(TGAhead), 1, out);
    fread(buffer, 3 * WWIDTH * WHEIGHT, 1, out);
    glDrawPixels(WWIDTH, WHEIGHT, GL_BGR, GL_UNSIGNED_BYTE, buffer);
    fclose(out);
    } else {
        std::cout << "File not found" << "\n";
        fileFound = false;
    }
}

void keyboard(unsigned char key, int x_, int y_){
    switch (key){
        case '`':
            if (cycleColor)
                keyboard('~',0,0);
            randomColor = !randomColor;
            if (randomColor){
                copy(pc, c);
                copy(c, ec);
                std::cout << "Random color enabled" << "\n";
            } else{
                copy(c, pc);
                std::cout << "Random color disabled" << "\n";
            }
            break;
        case '~':
            if (randomColor)
                keyboard('`',0,0);
            cycleColor = !cycleColor;
            if (cycleColor){
                copy(pc, c);
                copy(c, ec);
                std::cout << "Cycle color enabled" << "\n";
            } else{
                copy(c, pc);
                std::cout << "Cycle color disabled" << "\n";
            }
            break;
        case 'b':
            drawState = quadrilateral;
            std::cout << "Quadrilateral drawing enabled" << "\n";
            break;
        case 'B':
            drawState = parallelogram;
            std::cout << "Parallelogram drawing enabled" << "\n";
            break;
        case 'c':
            drawState = circle;
            std::cout << "Circle drawing enabled" << "\n";
            break;
        case 'C':
            drawState = easyCircle;
            std::cout << "Easy Circle drawing enabled" << "\n";
            break;
        case 'e':
            erase = !erase;
            if (erase){
                copy(pc, c);
                copy(c, ec);
                std::cout << "Eraser toggle enabled" << "\n";
            } else{
                copy(c, pc);
                std::cout << "Eraser toggle disabled" << "\n";
            }
            break;
        case 'r':
            freeDraw = !freeDraw;
            if (freeDraw){
                std::cout << "Free toggle enabled" << "\n";
            } else{
                std::cout << "Free toggle disabled" << "\n";
            }
            break;
        case 'x':
            drawState = point;
            std::cout << "Point draw enabled" << "\n";
            break;
        case 'a':
            help();
            break;
        case 'X':
            drawState = line;
            std::cout << "Line drawing enabled" << "\n";
            break;
        case 's':
            filled = !filled;
            if (filled)
                std::cout << "Fill mode enabled" << "\n";
            else
                std::cout << "Fill mode disabled" << "\n";
            break;
        case ' ':
            std::cout << "Exiting Ink" << "\n";
            exit(0);
            break;
        case 'S':
            glFlush();
            glColor3fv(c);
            glBegin(GL_POLYGON);
                glVertex2i(0, 0);
                glVertex2i(0, WHEIGHT);
                glVertex2i(WWIDTH, WHEIGHT);
                glVertex2i(WWIDTH, 0);
            glEnd();
            glFlush();
            glutPostRedisplay();
            std::cout << "Window re-colored" << "\n";
            break;
        case 'q':
            try{
                save();
                std::cout << "Image Saved" << std::endl;
            } 
            catch (std::exception& exc){
                std::cout << "Exception has occurred!" << std::endl;
            }
            break;
        case 't':
            try{
                glFlush();
                load();
                if (fileFound){
                    std::cout << "Image Loaded" << std::endl;
                    glFlush();
                    glutPostRedisplay();
                }
            } 
            catch (std::exception& exc){
                std::cout << "Exception has occurred!" << std::endl;
            }
            break;
        case 'z':
            if (controlCount == 0){
                drawState = bezier;
                std::cout << "Quadratic Bezier drawing enabled" << "\n";
                controlCount++;
            } else
            if (controlCount == 1){
                drawState = bezier;
                std::cout << "Cubic Bezier drawing enabled" << "\n";
                controlCount++;
            } else
            if (controlCount == 2){
                drawState = bezier;
                std::cout << "Quartic Bezier drawing enabled" << "\n";
                controlCount++;
            } else
            if (controlCount == 3){
                drawState = bezier;
                std::cout << "Quadratic Bezier drawing enabled" << "\n";
                controlCount = 1;
            }
            break;
        case 'v':
            drawState = triangle;
            std::cout << "Triangle drawing enabled" << "\n";
            break;
        case 'w':
            glFlush(); glClear(GL_COLOR_BUFFER_BIT);
            std::cout << "Clearing screen" << "\n";
            glFlush(); glClear(GL_COLOR_BUFFER_BIT);
            break;
        case '0':
            c[0] = 1;
            c[1] = 1;
            c[2] = 1;
            std::cout << "Color is now set to White" << "\n";
            break;
        case '1':
            c[0] = 1;
            c[1] = 0;
            c[2] = 0;
            std::cout << "Color is now set to Red" << "\n";
            break;
        case '2':
            c[0] = 0;
            c[1] = 1;
            c[2] = 0;
            std::cout << "Color is now set to Green" << "\n";
            break;
        case '3':
            c[0] = 0;
            c[1] = 0;
            c[2] = 1;
            std::cout << "Color is now set to Blue" << "\n";
            break;
        case '4':
            c[0] = 1;
            c[1] = 1;
            c[2] = 0;
            std::cout << "Color is now set to Yellow" << "\n";
            break;
        case '5':
            c[0] = 1;
            c[1] = 0;
            c[2] = 1;
            std::cout << "Color is now set to Magenta" << "\n";
            break;
        case '6':
            c[0] = 0;
            c[1] = 1;
            c[2] = 1;
            std::cout << "Color is now set to Cyan" << "\n";
            break;
        case '7':
            c[0] = 1;
            c[1] = .5;
            c[2] = .5;
            std::cout << "Color is now set to Alt Red" << "\n";
            break;
        case '8':
            c[0] = .5;
            c[1] = 1;
            c[2] = .5;
            std::cout << "Color is now set to Alt Green" << "\n";
            break;
        case '9':
            c[0] = .5;
            c[1] = .5;
            c[2] = 1;
            std::cout << "Color is now set to Alt Blue" << "\n";
            break;
        case 'd':
            if ((brushSize - 1) > 0)
                brushSize--;
            std::cout << "Brush Size = " << brushSize << "\n";
            break;
        case 'f':
            brushSize++;
            std::cout << "Brush Size = " << brushSize << "\n";
            break;
        case 'D':
            if ((brushSize - 10) > 0)
                brushSize -= 10;
            else
                brushSize == 1;
            std::cout << "Brush Size = " << brushSize << "\n";
            break;
        case 'F':
            brushSize += 10;
            std::cout << "Brush Size = " << brushSize << "\n";
            break;
        case 'g':
            if ((SLEEP_TIME - 1) > 0)
                SLEEP_TIME--;
            std::cout << "Color Timer = " << SLEEP_TIME << "\n";
            break;
        case 'h':
            SLEEP_TIME++;
            std::cout << "Color Timer = " << SLEEP_TIME << "\n";
            break;
        case 'G':
            if ((SLEEP_TIME - 10) > 0)
                SLEEP_TIME -= 10;
            else
                SLEEP_TIME == 1;
            std::cout << "Color Timer = " << SLEEP_TIME << "\n";
            break;
        case 'H':
            SLEEP_TIME += 10;
            std::cout << "Color Timer = " << SLEEP_TIME << "\n";
            break;
        default:
            std::cout << "Invalid key" << "\n";
            break;

    }
}

void mouse(int button, int state, int x_, int y_){
    glFlush();
    if (freeDraw && drawState == point){
        previousPoint[0] = x_;
        previousPoint[1] = WHEIGHT-y_;
        glPointSize(1);
        drawAdjPoint(x_,WHEIGHT-y_);
    }
    if (!freeDraw && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        switch (drawState){
            case point:
                previousPoint[0] = x_;
                previousPoint[1] = WHEIGHT-y_;
                drawPoint(x_,WHEIGHT-y_);
                break;
            case line:
                if (lineCount < 2){
                    drawPoint(x_, WHEIGHT-y_);
                    linePoints[lineCount][0] = x_;
                    linePoints[lineCount][1] = WHEIGHT-y_;
                    lineCount++;
                }
                if (lineCount == 2){
                    drawLine(linePoints[0], linePoints[1]);
                    lineCount = 0;
                }
                break;
            case bezier:
                if (curveCount < controlCount+2){
                    if (curveCount < 2)
                        drawPoint(x_, WHEIGHT-y_);
                    curvePoints[curveCount][0] = x_;
                    curvePoints[curveCount][1] = WHEIGHT-y_;
                    curveCount++;
                }
                if (curveCount == controlCount+2){
                    drawCurve();
                    curveCount = 0;
                }
                break;
            case circle:
                if (cCount < 2){
                    cPoints[cCount][0] = x_;
                    cPoints[cCount][1] = WHEIGHT-y_;
                    cCount++;
                }
                if (cCount == 2){
                    // Equation of radius: (x-x0)^2 + (y-y0)^2 = r^2
                    int x, y;
                    int r = std::pow(std::pow(cPoints[1][0] - cPoints[0][0], 2) + std::pow(cPoints[1][1] - cPoints[0][1], 2), .5);
                    glColor3fv(c);
                    glPointSize(brushSize);
                    if (filled)
                        glBegin(GL_TRIANGLE_FAN);
                    else
                        glBegin(GL_POINTS);
                    for(float d = 0; d<360;d+=.2){
                        x = cPoints[0][0]+sin(d)*r;
                        y = cPoints[0][1]+cos(d)*r;
                        glVertex2f(x,y);
                    }
                    glEnd();
                    cCount = 0;
                }
                break;
            case easyCircle:
                glColor3fv(c);
                glPointSize(1);
                if (filled)
                    glBegin(GL_TRIANGLE_FAN);
                else
                    glBegin(GL_POINTS);
                int x,y;
                for(float d = 0; d<360;d+=.2){
                    x = x_+sin(d)*brushSize;
                    y = WHEIGHT-y_+cos(d)*brushSize;
                    glVertex2f(x,y);
                }
                glEnd();
                cCount = 0;
                break;
            case triangle:
                if (tCount < 3){
                    int temp = brushSize;
                    brushSize = 1;
                    drawPoint(x_, WHEIGHT-y_);
                    brushSize = temp;
                    tPoints[tCount][0] = x_;
                    tPoints[tCount][1] = WHEIGHT-y_;
                    tCount++;
                }
                if (tCount > 1){
                    drawLine(tPoints[tCount-2], tPoints[tCount-1]);
                    if (tCount == 3 && !filled){
                        drawLine(tPoints[0], tPoints[2]);
                        tCount = 0;
                    }       
                }
                if (tCount == 3 && filled){
                    glPointSize(brushSize);
                    glLineWidth(brushSize);
                    glColor3fv(c);
                    glBegin(GL_POLYGON);
                        glVertex2i(tPoints[0][0], tPoints[0][1]);
                        glVertex2i(tPoints[1][0], tPoints[1][1]);
                        glVertex2i(tPoints[2][0], tPoints[2][1]);
                    glEnd();
                    tCount = 0;
                }
                break;
            case quadrilateral:
                if (qCount < 4){
                    int temp = brushSize;
                    brushSize = 1;
                    drawPoint(x_, WHEIGHT-y_);
                    brushSize = temp;
                    qPoints[qCount][0] = x_;
                    qPoints[qCount][1] = WHEIGHT-y_;
                    qCount++;
                }
                if (qCount > 1){
                    drawLine(qPoints[qCount-2], qPoints[qCount-1]);
                    if (qCount == 4 && !filled){
                        drawLine(qPoints[0], qPoints[3]);
                        qCount = 0;
                    }       
                }
                if (qCount == 4 && filled){
                    glPointSize(brushSize);
                    glLineWidth(brushSize);
                    glColor3fv(c);
                    glBegin(GL_POLYGON);
                        glVertex2i(qPoints[0][0], qPoints[0][1]);
                        glVertex2i(qPoints[1][0], qPoints[1][1]);
                        glVertex2i(qPoints[2][0], qPoints[2][1]);
                        glVertex2i(qPoints[3][0], qPoints[3][1]);
                    glEnd();
                    qCount = 0;
                }
                break;
            case parallelogram:
                if (pCount < 2){
                    glPointSize(1);
                    drawAdjPoint(x_, WHEIGHT-y_);
                    pPoints[pCount][0] = x_;
                    pPoints[pCount][1] = WHEIGHT-y_;
                    pCount++;
                }
                if (pCount == 2){
                    int x2,x3,y2,y3;
                    int xsep = std::abs(pPoints[0][0] - pPoints[1][0]);
                    int ysep = std::abs(pPoints[0][1] - pPoints[1][1]);
                    if (pPoints[0][0] > pPoints[1][0]){
                        x2 = pPoints[0][0] - xsep;
                        x3 = pPoints[1][0] + xsep;
                    } else {
                        x2 = pPoints[0][0] + xsep;
                        x3 = pPoints[1][0] - xsep;
                    }
                    if (pPoints[0][1] > pPoints[1][1]){
                        y3 = pPoints[0][1] - ysep;
                        y2 = pPoints[1][1] + ysep;
                    } else {
                        y3 = pPoints[0][1] + ysep;
                        y2 = pPoints[1][1] - ysep;
                    }
                    int p2[] = {x2,y2};
                    int p3[] = {x3,y3};
                    glPointSize(1);
                    drawAdjPoint(x2,y2);
                    drawAdjPoint(x3,y3);
                    glLineWidth(brushSize);
                    glColor3fv(c);
                    if (filled){
                        glBegin(GL_POLYGON);
                            glVertex2i(pPoints[0][0], pPoints[0][1]);
                            glVertex2i(x3,y3);
                            glVertex2i(pPoints[1][0], pPoints[1][1]);
                            glVertex2i(x2, y2);
                            glVertex2i(pPoints[0][0], pPoints[0][1]);
                        glEnd();
                    } else {
                        drawLine(pPoints[0], p3);
                        drawLine(p3,pPoints[1]);
                        drawLine(pPoints[1], p2);
                        drawLine(p2, pPoints[0]);
                    }
                    pCount = 0;
                }
                break;
        }
        glFlush();
    }
}

void mouseFree(int x_, int y_){
    glFlush();
    if (freeDraw){
        switch (drawState){
            case point:
                currentPoint[0] = x_; 
                currentPoint[1] = WHEIGHT-y_;
                drawLine(currentPoint, previousPoint);
                previousPoint[0] = x_; 
                previousPoint[1] = WHEIGHT-y_;
                break;
            case easyCircle:
                currentPoint[0] = x_; 
                currentPoint[1] = WHEIGHT-y_;
                drawLine(currentPoint, previousPoint);
                break;
            case circle:
                int x, y;
                //int r = brushSize;
                glColor3fv(c);
                glPointSize(1);
                if (filled)
                    glBegin(GL_TRIANGLE_FAN);
                else
                    glBegin(GL_POINTS);
                for(float d = 0; d<360;d+=.2){
                    x = x_+sin(d)*brushSize;
                    y = y_+cos(d)*brushSize;
                    //std::cout << "x,y = " << x <<", " << y << "\n";
                    glVertex2f(x,WHEIGHT-y);
                }
                glEnd();
                break;
            case triangle:
                int tempt[3][2];
                for (int i = 0; i < 3; i++){
                    float rt = brushSize * std::sqrt(((float)std::rand()/(float)RAND_MAX));
                    float thetat = ((float)std::rand()/(float)RAND_MAX) * 2 * M_PI;
                    float randtx = x_ + rt * std::cos(thetat);
                    float randty = y_ + rt * std::sin(thetat);
                    tempt[i][0] = randtx;
                    tempt[i][1] = WHEIGHT-randty;
                }
                glColor3fv(c);
                glPointSize(1);
                glLineWidth(1);
                if (filled){
                    glLineWidth(brushSize);
                    glColor3fv(c);
                    glBegin(GL_POLYGON);
                        glVertex2i(tempt[0][0], tempt[0][1]);
                        glVertex2i(tempt[1][0], tempt[1][1]);
                        glVertex2i(tempt[2][0], tempt[2][1]);
                    glEnd();
                } else {
                    glColor3fv(c);
                    drawAdjLine(tempt[0], tempt[1]);
                    drawAdjLine(tempt[1], tempt[2]);
                    drawAdjLine(tempt[2], tempt[0]);
                }
                break;
            case quadrilateral:
                int tempq[4][2];
                for (int i = 0; i < 4; i++){
                    float rq = brushSize * std::sqrt(((float)std::rand()/(float)RAND_MAX));
                    float thetaq = ((float)std::rand()/(float)RAND_MAX) * 2 * M_PI;
                    float randqx = x_ + rq * std::cos(thetaq);
                    float randqy = y_ + rq * std::sin(thetaq);
                    tempq[i][0] = randqx;
                    tempq[i][1] = WHEIGHT-randqy;
                }
                glColor3fv(c);
                glPointSize(1);
                glLineWidth(1);
                if (filled){
                    glLineWidth(brushSize);
                    glColor3fv(c);
                    glBegin(GL_POLYGON);
                        glVertex2i(tempq[0][0], tempq[0][1]);
                        glVertex2i(tempq[1][0], tempq[1][1]);
                        glVertex2i(tempq[2][0], tempq[2][1]);
                        glVertex2i(tempq[3][0], tempq[3][1]);
                    glEnd();
                } else {
                    glColor3fv(c);
                    drawAdjLine(tempq[0], tempq[1]);
                    drawAdjLine(tempq[1], tempq[2]);
                    drawAdjLine(tempq[2], tempq[3]);
                    drawAdjLine(tempq[3], tempq[0]);
                }
                break;
            case parallelogram:
                if (filled){
                    glLineWidth(1);
                    glColor3fv(c);
                    glBegin(GL_POLYGON);
                        glVertex2i(x_ - brushSize/2, WHEIGHT-y_ - brushSize/2);
                        glVertex2i(x_ + brushSize/2, WHEIGHT-y_ - brushSize/2);
                        glVertex2i(x_ + brushSize/2, WHEIGHT-y_ + brushSize/2);
                        glVertex2i(x_ - brushSize/2, WHEIGHT-y_ + brushSize/2);
                    glEnd();
                } else {
                    glLineWidth(1);
                    glColor3fv(c);
                    drawAdjLine(x_ - brushSize/2, WHEIGHT-y_ - brushSize/2, x_ + brushSize/2, WHEIGHT-y_ - brushSize/2);
                    drawAdjLine(x_ + brushSize/2, WHEIGHT-y_ + brushSize/2, x_ - brushSize/2, WHEIGHT-y_ + brushSize/2);
                    drawAdjLine(x_ - brushSize/2, WHEIGHT-y_ - brushSize/2, x_ - brushSize/2, WHEIGHT-y_ + brushSize/2);
                    drawAdjLine(x_ + brushSize/2, WHEIGHT-y_ + brushSize/2, x_ + brushSize/2, WHEIGHT-y_ - brushSize/2);
                }
                break;
            case line:
                float rl = brushSize * std::sqrt(((float)std::rand()/(float)RAND_MAX));
                float thetal = ((float)std::rand()/(float)RAND_MAX) * 2 * M_PI;
                float randlx = x_ + rl * std::cos(thetal);
                float randly = y_ + rl * std::sin(thetal);
                int templ2[2] = {x_, WHEIGHT-y_};
                int templ[2] = {(int)randlx, (int)(WHEIGHT-randly)};
                glPointSize(1);
                glLineWidth(1);
                drawAdjLine(templ2, templ);
                break;
        }
    }
}

void colorTimer(int value){
    if (randomColor){
        int number = ((float)std::rand()/(float)RAND_MAX) * 10;
        switch (number){
            case 0:
                keyboard('0',0,0);
                break;
            case 1:
                keyboard('1',0,0);
                break;
            case 2:
                keyboard('2',0,0);
                break;
            case 3:
                keyboard('3',0,0);
                break;
            case 4:
                keyboard('4',0,0);
                break;
            case 5:
                keyboard('5',0,0);
                break;
            case 6:
                keyboard('6',0,0);
                break;
            case 7:
                keyboard('7',0,0);
                break;
            case 8:
                keyboard('8',0,0);
                break;
            case 9:
                keyboard('9',0,0);
                break;
        }
    }
    if (cycleColor){
        cycleCount = (cycleCount+1) % 10;
        switch (cycleCount){
            case 0:
                keyboard('0',0,0);
                break;
            case 1:
                keyboard('1',0,0);
                break;
            case 2:
                keyboard('2',0,0);
                break;
            case 3:
                keyboard('3',0,0);
                break;
            case 4:
                keyboard('4',0,0);
                break;
            case 5:
                keyboard('5',0,0);
                break;
            case 6:
                keyboard('6',0,0);
                break;
            case 7:
                keyboard('7',0,0);
                break;
            case 8:
                keyboard('8',0,0);
                break;
            case 9:
                keyboard('9',0,0);
                break;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(SLEEP_TIME, colorTimer, 0); // Animates screen given a specified time "refresh rate"
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitWindowSize(WWIDTH,WHEIGHT);
    glutCreateWindow("Ink");
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    
    init();
    help();

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseFree);
    glutDisplayFunc(display);
    glutTimerFunc(SLEEP_TIME, colorTimer, 0); // Animates screen given a specified time "refresh rate"
    glutMainLoop();

    return 0;
}