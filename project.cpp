/*
    *created by Reevan
    *TODO to implement functionality to draw barriers using mouse function
    *TODO to implement will use dijakstras algo
    *TODO one more functionality which iam planning is to draw maze using  recursive division
    *TODO to fix window resizing problems
*/
#include <stdio.h>
#include <unordered_set>
#include <GL/glut.h>

//for allowing user only to choose set of predefined colors //feature is pending for  to completion
enum Colors
{
    BLACK,
    GOLDEN_GATE_BRIDGE,
    PACIFIC_BLUE,
    CARIBBEAN_GREEN,
    EMERALD,
    DARK_ORCHID,
    FUCHSIA,
    POPSTAR,
};

struct Cell
{
    int x = 0;
    int y = 0;
} cell;

//globals
//20 * 20 cell in an 500*500 viewport will give 24 cell'd grid
const int vw = 800;
const int vh = 700;
const int cellSize = 20;

int mouseX = 0;
int mouseY = 0;
std::unordered_set<int> barrier; //does searching insertion delation operation within Ω(1) complexity
struct Cell lastFocusedCell, tempCell,startingPoint = {3,10};
int cellPerRow = (int)vw / cellSize; //TODO updates this whenever window resizes
bool isLeftButtonPressed = false;

void init()
{
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, vw, 0, vh);
    glMatrixMode(GL_MODELVIEW);
}

void setColor(Colors c)
{
    if (c == BLACK)
    {
        glColor3f(0.0, 0.0, 0.0);
    }
    else if (c == GOLDEN_GATE_BRIDGE)
    {
        glColor3f(0.7215, 0.2745, 0.1882);
    }
    else if (c == PACIFIC_BLUE)
    {
        glColor3f(0.1137, 0.6509, 0.7725);
    }
    else if (c == CARIBBEAN_GREEN)
    {
        glColor3f(0.2745, 0.8156, 0.6);
    }
    else if (c == EMERALD)
    {
        glColor3f(0.0509, 0.7843, 0.4901);
    }
    else if (c == DARK_ORCHID)
    {
        glColor3f(0.6, 0.2941, 0.8313);
    }
    else if (c == FUCHSIA)
    {
        glColor3f(0.8196, 0.2901, 0.8313);
    }
    else if (c == POPSTAR)
    {
        glColor3f(0.8039, 0.2941, 0.3882);
    }
}

void drawCell(int x, int y, int length, Colors color)
{
    setColor(color);
    glBegin(GL_POLYGON);
    glVertex2f(x, vh - y);
    glVertex2f(x + length, vh - y); //becauses cordinates in opengl drawing starts form bottom left
    glVertex2f(x + length, vh - y - length);
    glVertex2f(x, vh - y - length);
    glEnd();
}

void drawGrid(int offset)
{
    setColor(CARIBBEAN_GREEN);
    glBegin(GL_LINES);
    // //draws horizontal lines
    for (int i = 0; i < vh; i += offset)
    {
        glVertex2f(0, i);
        glVertex2f(vw, i);
    }
    //draws vertical lines
    for (int i = 0; i < vw; i += offset)
    {
        glVertex2f(i, 0);
        glVertex2f(i, vh);
    }
    glEnd();
}

void drawStartingPoint(int cellStartX, int cellStartY)
{
  
    drawCell(cellStartX, cellStartY, cellSize, GOLDEN_GATE_BRIDGE);
}

void drawEndingPoint()
{
    //some code
}

//to determine which cell mouse is focusing upon updates everytime whenever mouse moves on left button clicked
void updateFousedCell()
{
    cell.x = (int)mouseX / cellSize; //this will give in which column mouse cursor is present like 0,1,2 and so on
    cell.y = (int)mouseY / cellSize; //this will give in which row mouse cursor is present like 0,1,2,3.. and so on
    // if ((tempCell.x - cell.x != 0) || (tempCell.y - cell.y != 0))
    // {
    //     lastFocusedCell.x = tempCell.x;
    //     lastFocusedCell.y = tempCell.y;
    //     tempCell.x = cell.x;
    //     tempCell.y = cell.y;
    // }
}

void onMouseMove(int x, int y)
{
    if (x > 0 && x < vw && y > 0 && y < vh)
    {
        if (isLeftButtonPressed == true)
        {
            updateFousedCell();
            mouseX = x;
            mouseY = y;
            if (barrier.find(cell.y * cellPerRow + cell.x) == barrier.end() && (lastFocusedCell.x != cell.x || lastFocusedCell.y != cell.y))
            {                                                 //if barrier cell is not alredy present in set
                barrier.insert(cell.y * cellPerRow + cell.x); //combine cellX and cellY into one equivalent value
            }
            else if (lastFocusedCell.x != cell.x || lastFocusedCell.y != cell.y)
            {
                barrier.erase(cell.y * cellPerRow + cell.x); //remove that cell if its allredy present in the set
            }
            // printf("mouseX = %d mouseY = %d\ncell num x = %d  y = %d\n", mouseX, mouseY, cell.x, cell.y);
        }
    }
    //this code should strictly go here
    lastFocusedCell.x = cell.x; //for avoiding continous change of state of  cells
    lastFocusedCell.y = cell.y; //for avoiding continous change of state of  cells
}

void onButtonClick(int button, int state, int x, int y)
{ //there are some errors need to fix these errors
    printf("mouse button pressed\n");
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//whenever mouse left button is pressed
    {
        isLeftButtonPressed = true;
        updateFousedCell();


    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)//whenever mouse left button is relased
    {
        isLeftButtonPressed = false;
    }
};

void display()
{
    int temp = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    //this code is only for testing purpose and does not belong to main application
    for (auto itr = barrier.begin(); itr != barrier.end(); itr++)
    {

        drawCell(*itr % cellPerRow * cellSize, *itr / cellPerRow * cellSize, cellSize, BLACK);
    }
    drawGrid(20);
    drawStartingPoint(startingPoint.x * cellSize, startingPoint.y * cellSize);
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(vw, vh);
    glutCreateWindow("Path Finding Visualization");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(onButtonClick);
    glutMotionFunc(onMouseMove); //updates mouse position only when some mouse mouse button is clicked an moved
    glutMainLoop();
}
