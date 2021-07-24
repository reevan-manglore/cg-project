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
#include <algorithm>

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

enum MouseMovementAxis
{
    NONE,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT

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
struct Cell lastFocusedCell, tempCell, startingPoint = {3, 10}, endingPoint = {30, 10};
int cellPerRow = (int)vw / cellSize; //TODO updates this whenever window resizes
bool isLeftButtonPressed = false;
bool didClickedStartPoint = false;
bool didClickedEndPoint = false;
int operation = 0; //to deteremine what operration user choosed from the menu

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

/*
    accepts cellX and ceellY and then it gives the single numbr which represnts in ehich cell number currenty
    point resides
*/
int getCellNumber(int cellX, int cellY)
{
    return (cellY * cellPerRow + cellX);
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

MouseMovementAxis getCurrentAxis(int mouseX, int mouseY)
{
    static int lastMouseX = 0, lastMouseY = 0;
    MouseMovementAxis axis = NONE;
    if ((mouseX - lastMouseX > 0) && (mouseY - lastMouseY > 0))
    {
        axis = BOTTOM_RIGHT;
        printf("we are moving %s\n", "BOTTOM_RIGHT ");
    }
    if ((mouseX - lastMouseX < 0) && (mouseY - lastMouseY > 0))
    {
        axis = BOTTOM_LEFT;
        printf("we are moving %s\n", " BOTTOM_LEFT");
    }
    if ((mouseX - lastMouseX > 0) && (mouseY - lastMouseY < 0))
    {
        axis = TOP_RIGHT;
        printf("we are moving %s\n", "TOP_RIGHT ");
    }
    else if ((mouseX - lastMouseX < 0) && (mouseY - lastMouseY < 0))
    {
        axis = TOP_LEFT;
        printf("we are moving %s\n", "TOP_LEFT ");
    }
    else if ((mouseX - lastMouseX < 0))
    {
        axis = LEFT;
        printf("we are moving %s\n", "LEFT ");
    }
    else if ((mouseX - lastMouseX > 0))
    {
        axis = RIGHT;
        printf("we are moving %s\n", "RIGHT ");
    }
    else if ((mouseY - lastMouseY < 0))
    {
        axis = TOP;
        printf("we are moving %s\n", " TOP");
    }
    else if ((mouseY - lastMouseY > 0))
    {
        axis = BOTTOM;
        printf("we are moving %s\n", " BOTTOM");
    }
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    return axis;
}

void drawStartingPoint(int cellX, int cellY)
{
    //here previous cell is used to store position of start point the instant before the start point  changes its cell position
    static int previouscellX, previouscellY;
    //if start point not on top of barrier
    if (barrier.find(getCellNumber(cellX, cellY)) == barrier.end())
    {
        //here cellStart struct gives values in terms of cell number but each cell is cellSize apart so we multiply cellStart with cellsize
        drawCell(cellX * cellSize, cellY * cellSize, cellSize, GOLDEN_GATE_BRIDGE);
        previouscellX = cellX;
        previouscellY = cellY;
    }
    //if its in top then draw start point in nearest cell to barrier
    else
    {
        drawCell(previouscellX * cellSize, previouscellY * cellSize, cellSize, GOLDEN_GATE_BRIDGE);
    }
}

void drawEndingPoint(int cellEndX, int cellEndY)
{
    //here previous cell is used to store position of end point the instant before the end point  changes its cell position
    static int previouscellX, previouscellY;
    //if end point not on top of barrier
    if (barrier.find(getCellNumber(cellEndX, cellEndY)) == barrier.end())
    {
        //here cellEnd struct gives values in terms of cell number but each cell is cellSize apart so we multiply cellEnd with cellsize
        drawCell(cellEndX * cellSize, cellEndY * cellSize, cellSize, EMERALD);
        previouscellX = cellEndX;
        previouscellY = cellEndY;
    }
    //if its in top then draw end point in nearest cell to barrier
    else
    {
        drawCell(previouscellX * cellSize, previouscellY * cellSize, cellSize, EMERALD);
    }
}

//to determine which cell mouse is focusing upon updates everytime whenever mouse moves on left button clicked
void updateFousedCell(int x, int y) //this will take the actual values of mouse cordinates an convert it into normalizedl values
{
    cell.x = (int)x / cellSize; //this will give in which column mouse cursor is present like 0,1,2 and so on
    cell.y = (int)y / cellSize; //this will give in which row mouse cursor is present like 0,1,2,3.. and so on
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
            updateFousedCell(x, y);
            mouseX = x;
            mouseY = y;
            if ( operation == 1 && !didClickedStartPoint && !didClickedEndPoint) //do this operation whenver starting point is not clicked and opeartion 1 is for drawing barriers
            {
                //if barrier cell is not alredy present in set and here lastFocuesd cell is to avoid continous drawing of barrier points
                if (barrier.find(getCellNumber(cell.x, cell.y)) == barrier.end() && (lastFocusedCell.x != cell.x || lastFocusedCell.y != cell.y))
                {
                    //if barrier points are not on top of starting point and ending point only then draw barriers
                    if ((getCellNumber(cell.x, cell.y) != getCellNumber(startingPoint.x, startingPoint.y)) && (getCellNumber(cell.x, cell.y) != getCellNumber(endingPoint.x, endingPoint.y)))
                        barrier.insert(cell.y * cellPerRow + cell.x); //combine cellX and cellY into one equivalent value
                }
                else if (lastFocusedCell.x != cell.x || lastFocusedCell.y != cell.y)
                {
                    barrier.erase(cell.y * cellPerRow + cell.x); //remove that cell if its allredy present in the set
                }
            }
            // printf("mouseX = %d mouseY = %d\ncell num x = %d  y = %d\n", mouseX, mouseY, cell.x, cell.y);

            //temp code should be modified
            if (operation == 2 && didClickedStartPoint) //opearation 2 for moving moving starting point
            {
                startingPoint.x = cell.x;
                startingPoint.y = cell.y;
            }
            else if (operation == 3 && didClickedEndPoint) //opearation 3 for moving destination point
            {
                endingPoint.x = cell.x;
                endingPoint.y = cell.y;
            }
        }
    }
    printf("%d\n", getCurrentAxis(x, y)); //code to delete
    //this code should strictly go here
    lastFocusedCell.x = cell.x; //for avoiding continous change of state of  cells
    lastFocusedCell.y = cell.y; //for avoiding continous change of state of  cells
}

void onButtonClick(int button, int state, int x, int y)
{ //there are some errors need to fix these errors
    // printf("mouse button pressed\n");
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) //whenever mouse left button is pressed
    {
        isLeftButtonPressed = true;
        updateFousedCell(x, y);
        //if mouse point is clicked at starting point (inorder to move starting point)
        if (cell.x == startingPoint.x && cell.y == startingPoint.y) //wehenver user clicks starting point set boolean value to true
        {
            didClickedStartPoint = true;
        }
        else if (cell.x == endingPoint.x && cell.y == endingPoint.y) //wehenver user clicks starting point set boolean value to true
        {
            didClickedEndPoint = true;
        }
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) //whenever mouse left button is relased
    {
        isLeftButtonPressed = false;
        //wehenver user relases  pressed left button. set  boolean value to false (if its set to true)
        didClickedStartPoint = false;
        didClickedEndPoint = false;
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
    drawStartingPoint(startingPoint.x, startingPoint.y);
    drawEndingPoint(endingPoint.x, endingPoint.y); //tempory code to delete
    glutSwapBuffers();
    glutPostRedisplay();
}

void chooseOperation(int operationCode)
{

    switch (operationCode)
    {
    case 1:
        operation = 1;
        break;
    case 2:
        operation = 2;
        break;
    case 3:
        operation = 3;
        break;
    case 4:
        operation = 4;
        break;
    case 5:
        operation = 5;
    default:
        operation = 0;
        break;
    }
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
    glutCreateMenu(chooseOperation);
    glutAddMenuEntry("Draw walls", 1);
    glutAddMenuEntry("Move starting point", 2);
    glutAddMenuEntry("Move destination point", 3);
    glutAddMenuEntry("Run dijakstra algorithm", 4);
    glutAddMenuEntry("Help", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
}
