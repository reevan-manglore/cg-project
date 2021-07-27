/*
    *created by Reevan
    *TODO to implement functionality to draw barriers using mouse function
    *TODO to implement will use dijakstras algo
    *TODO one more functionality which iam planning is to draw maze using  recursive division
    *TODO to fix window resizing problems
*/
#include <stdio.h>
#include <GL/glut.h>
#include <unordered_set>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <string>

//for allowing user only to choose set of predefined colors
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
    SILVER_PINK,
    MINION_YELLOW
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
struct Cell lastFocusedCell, tempCell, startingPoint = {3, 0}, endingPoint = {30, 10};
const int cellPerRow = (int)vw / cellSize; //TODO updates this whenever window resizes
bool isLeftButtonPressed = false;
bool didClickedStartPoint = false;
bool didClickedEndPoint = false;
int operation = 0;     //to deteremine what operration user choosed from the menu
std::vector<int> path; //used to store the nodes that need to be traversed to get shortest path

void toPaint(void); //function that stores what all graphics need to be painted on screen than draws whenever this function is called

void findShortestPath(void);

void clearPath(void);

bool isPathCalculated = false; //wether shortest path from starting point to ending point has once been calculated

bool toUpadatePathInRealTime = false;

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
        glColor4f(0.8196, 0.2901, 0.8313, 0.3);
    }
    else if (c == POPSTAR)
    {
        glColor3f(0.8039, 0.2941, 0.3882);
    }
    else if (c == SILVER_PINK)
    {
        glColor4f(0.7725, 0.6941, 0.6941, 0.45);
    }
    else if (c == MINION_YELLOW)
    {
        glColor3f(0.9568, 0.8823, 0.1921);
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

void drawBarrier() //used to draw all the walls in that is stored in the barrier set
{
    for (auto itr = barrier.begin(); itr != barrier.end(); itr++)
    {

        drawCell(*itr % cellPerRow * cellSize, *itr / cellPerRow * cellSize, cellSize, BLACK);
    }
}

//to determine which cell mouse is focusing upon updates everytime whenever mouse moves on left button clicked
void updateFousedCell(int x, int y) //this will take the actual values of mouse cordinates an convert it into normalizedl values
{
    cell.x = (int)x / cellSize; //this will give in which column mouse cursor is present like 0,1,2 and so on
    cell.y = (int)y / cellSize; //this will give in which row mouse cursor is present like 0,1,2,3.. and so on
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
            if (operation == 1 && !didClickedStartPoint && !didClickedEndPoint) //do this operation whenver starting point is not clicked and opeartion 1 is for drawing barriers
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

            if (operation == 2 && didClickedStartPoint) //opearation 2 for moving moving starting point
            {
                startingPoint.x = cell.x;
                startingPoint.y = cell.y;
            }
            else if (operation == 3 && didClickedEndPoint) //opearation 3 for moving destination point
            {

                endingPoint.x = cell.x;
                endingPoint.y = cell.y;
                if (isPathCalculated && barrier.find(getCellNumber(endingPoint.x, endingPoint.y)) == barrier.end()) //if shortest path from starting point to ending point has been aleredy been calculated then upadte path in real time
                {                                                                                                   //here iam giving conditon barrier.find() == barrrier.end() to ensure ending point does not lie upon barrier
                    clearPath();                                                                                    //clear path calcualated from starting point to ending point from starting point to ending point
                    findShortestPath();                                                                             //recalulate  shortest path from starting point to ending point
                }
            }
        }
    }

    //this code should strictly go here
    lastFocusedCell.x = cell.x; //for avoiding continous change of state of  cells
    lastFocusedCell.y = cell.y; //for avoiding continous change of state of  cells
}

void onButtonClick(int button, int state, int x, int y)
{
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

//here onwords iam implementing the core of this algorithm i.e path finding algorithm
std::unordered_set<int> visitedNodes;
const int verticalCellCount = (int)vh / cellSize;    //vh/cellSize give cell per column
int distances[cellPerRow * verticalCellCount] = {0}; //to store distance of each node from starting point

void initaliseDistances()
{
    for (int i = 0; i < cellPerRow * verticalCellCount; i++)
        distances[i] = INT_MAX;
}
bool isBarrier(int num)
{
    return barrier.find(num) != barrier.end();
}

bool isVisited(int node)
{
    return visitedNodes.find(node) != visitedNodes.end();
}

int getTopNode(int node)
{
    return (((node / cellPerRow - 1) * cellPerRow) + (node % cellPerRow));
}

int getBottomNode(int node)
{
    return (((node / cellPerRow + 1) * cellPerRow) + (node % cellPerRow));
}

void dijaskstra()
{

    std::queue<int> nextNode;                                                  //next node that acts as source node inorder to relax other nodes
    const int STARTING_POINT = startingPoint.y * cellPerRow + startingPoint.x; //here starting point does not accepts values here in terms of x and y axis here ite accepts cell number
    const int ENDING_POINT = endingPoint.y * cellPerRow + endingPoint.x;
    nextNode.push(STARTING_POINT);                        //starting node from where travesring begins
    int rightWeight, leftWeight, topWeight, bottomWeight; //to adjust weights based on position of starting point and ending point
    //its based upon nuber of observations
    initaliseDistances();
    distances[STARTING_POINT] = 0;
    if (STARTING_POINT < ENDING_POINT)
    {
        //to determine wethere statring appears first or ending point appears first along horizontal row then to adjust corresponding weights
        if ((STARTING_POINT) % cellPerRow < (ENDING_POINT) % cellPerRow)
        {
            rightWeight = 2;
            leftWeight = 3;
        }
        else
        {
            rightWeight = 3;
            leftWeight = 2;
        }
        topWeight = 4;
        bottomWeight = 1;
    }

    else
    {
        //to determine wethere statring appears first or ending point appears first along horizontal row then to adjust corresponding weights
        if ((STARTING_POINT) % cellPerRow < (ENDING_POINT) % cellPerRow)
        {
            rightWeight = 2;
            leftWeight = 3;
        }
        else
        {
            rightWeight = 3;
            leftWeight = 2;
        }
        topWeight = 1;
        bottomWeight = 4;
    }
    while (!nextNode.empty())
    {
        int node = nextNode.front();                                //get front node from queue
        nextNode.pop();                                             //delete front node
        visitedNodes.insert(node);                                  //all visited nodes
        int toRealax;                                               //node that needs to be relaxed
        toRealax = node + 1;                                        //get right node
        if (((node + 1) % cellPerRow != 0) && !isBarrier(toRealax)) //right node
        //here iam performing conditon by node + 1 beccuse all values of 1st column added by 1 will get the value in terms of multipples of xCount
        {

            if (distances[toRealax] > distances[node] + rightWeight)
            {
                distances[toRealax] = distances[node] + rightWeight;
                if (!isVisited(toRealax))
                    nextNode.push(toRealax);
            }
        }

        toRealax = getBottomNode(node);
        //here logic is bottom most row contain numbers up until xCount*yCount where yCount  = vh/cellPerRow
        if (toRealax < (cellPerRow * ((int)vh / cellSize)) && !isBarrier(toRealax)) //bottom node
        {
            if (distances[toRealax] > distances[node] + bottomWeight)
            {
                distances[toRealax] = distances[node] + bottomWeight;
                if (!isVisited(toRealax))
                    nextNode.push(toRealax);
            }
        }

        toRealax = node - 1;                                //get left node
        if (node % cellPerRow != 0 && !isBarrier(toRealax)) //left node node
        {

            if (distances[toRealax] > distances[node] + leftWeight)
            {
                distances[toRealax] = distances[node] + leftWeight;
                if (!isVisited(toRealax))
                    nextNode.push(toRealax);
            }
        }

        toRealax = getTopNode(node);
        if ((toRealax) >= 0 && !isBarrier(toRealax)) //top node
        {
            if (distances[toRealax] > distances[node] + topWeight)
            {
                distances[toRealax] = distances[node] + topWeight;
                if (!isVisited(toRealax))
                    nextNode.push(toRealax);
            }
        }
        ///this code will show all the visited nodes
        glClear(GL_COLOR_BUFFER_BIT); //clear the screen
        toPaint();                    //paint all the contents that are in paint function
        glutSwapBuffers();
        glutPostRedisplay();
    }
    findShortestPath(); //draw shortest path from starting point to ending point
    //inorder to avoid infinite looping so as soon as algorithm finshes its work make operation(from menu) = 0
    operation = 0;
}

void findShortestPath()
{
    std::vector<int> temp;                                                     //here since we are backtracking we need to store all the nodes in temp variable and then unwind all those contents from top position to path variable
    const int STARTING_POINT = startingPoint.y * cellPerRow + startingPoint.x; //here starting point does not accepts values here in terms of x and y axis here ite accepts cell number
    const int ENDING_POINT = endingPoint.y * cellPerRow + endingPoint.x;
    int leftNode, topNode, rightNode, bottomNode, currentNode = ENDING_POINT, smallestNode, smallestNodeVal;
    int count = 0; //in order to avoid infinite loops if any exists in the programme
    isPathCalculated = true;
    if (distances[ENDING_POINT] > 30000) //if the ending point is unrechable then return out of this function withouth performing any operation
    {
        return;
    }
    temp.push_back(ENDING_POINT); //starting point from where backtracking begins
    while (currentNode != STARTING_POINT)
    {
        count++;                   //to avoid infinite loops
        smallestNodeVal = INT_MAX; //initial assignment
        rightNode = currentNode + 1;
        bottomNode = getBottomNode(currentNode);
        leftNode = currentNode - 1;
        topNode = getTopNode(currentNode);
        if ((currentNode + 1) % cellPerRow != 0) //right node
        {

            if (distances[rightNode] < smallestNodeVal)
            {
                smallestNodeVal = distances[rightNode];
                smallestNode = rightNode;
            }
        }

        if (bottomNode < (cellPerRow * verticalCellCount)) //bottom node
        {

            if (distances[bottomNode] < smallestNodeVal)
            {
                smallestNodeVal = distances[bottomNode];
                smallestNode = bottomNode;
            }
        }

        if (currentNode % cellPerRow != 0) //left node node
        {

            if (distances[leftNode] < smallestNodeVal)
            {
                smallestNodeVal = distances[leftNode];
                smallestNode = leftNode;
            }
        }

        if (topNode >= 0) //top node
        {

            if (distances[topNode] < smallestNodeVal)
            {
                smallestNodeVal = distances[topNode];
                smallestNode = topNode;
            }
        }
        temp.push_back(smallestNode);
        currentNode = smallestNode;
        if (count >= 5000) //some big number inorder to avoid infinite loop
        {
            printf("exiting due to infinite loop\n");
            exit(1);
        }
    }
    while (!temp.empty()) //inorder to draw the path from starting point i.e output of backtracked result stored in temp
    {
        path.push_back(temp.back()); //here i will get least recently backtarcked path i.e it starts from starting point in contrast to ending point
        temp.pop_back();
        if (!toUpadatePathInRealTime) //in order to reduce the overhead of redrawing screen again and again  whenver user wants upadte of path in real time
        {
            glClear(GL_COLOR_BUFFER_BIT); //clear the screen
            toPaint();                    //paint all the contents that are in paint function
            glutSwapBuffers();
            glutPostRedisplay();
        }
    }
}

void toPaint()
{
    //if algorithm has been run then then vistedNodes by dijakstra algo is non empy so if this set is non empty then we draw visted nodes on screen
    if (!visitedNodes.empty())
    {
        for (auto i : visitedNodes)
        {
            if (i != getCellNumber(startingPoint.x, startingPoint.y) && i != getCellNumber(endingPoint.x, endingPoint.y))
                drawCell(i % cellPerRow * cellSize, i / cellPerRow * cellSize, cellSize, FUCHSIA);
        }
    }
    //draw the shortest path from starting point up to ending point
    if (!path.empty()) //if path has been found only then execute code
    {
        for (auto i : path)
            drawCell(i % cellPerRow * cellSize, i / cellPerRow * cellSize, cellSize, MINION_YELLOW); //in order to draw path from starting point to ending point
    }

    //draw all the walls
    drawBarrier();
    drawStartingPoint(startingPoint.x, startingPoint.y);
    drawEndingPoint(endingPoint.x, endingPoint.y);
    drawGrid(20); //draw grid of each cell size 20
}

void clearAllBarriers()
{
    barrier.clear();
}

void resetAllDistances() //reset all the calulated distances from statring node to every other node
{
    initaliseDistances(); //reset all distances to INT_MAX i.e state of distances during starting of programme
    visitedNodes.clear();
    path.clear();                    //clear the path from starting point to ending point
    isPathCalculated = false;        //becuase we are recalculating distances
    toUpadatePathInRealTime = false; //since we are recalculating the distances for entire board so update path in real time is not needed
}

void clearPath() //reset path traced from starting point to ending point
{
    path.clear();
}

void display()
{
    int temp = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    toPaint();
    if (operation == 4)
    {
        dijaskstra();
    }
    glutSwapBuffers();
    glutPostRedisplay();
}

void chooseOperation(int operationCode)
{

    switch (operationCode)
    {
    case 1:
        resetAllDistances();
        operation = 1;
        break;
    case 2:
        resetAllDistances(); //becuase dijakstra algorithm is single source algorithm so when source node changes then entire distances need to be recalculated
        operation = 2;
        break;
    case 3:
        operation = 3;
        if (isPathCalculated) //if once path has aleredy been calculated then we need to recalculate shortest path from starting node to ending node in real time
            toUpadatePathInRealTime = true;
        break;
    case 4:
        operation = 4;
        break;
    case 5: //command to clear the entire contents of board except for starting point and ending point
        clearAllBarriers();
        resetAllDistances();
        isPathCalculated = false;
        toUpadatePathInRealTime = false;
        break;
    default:
        operation = 0;
        break;
    }
}

void welcomeScreen() //welcome screen
{
    char title[] = "Visualization Of Dijkstra's Algorithm";
    char teamMemberTitle[] = "Team Members:";
    std::string names[3] = {"Reevan   4SO18CS095", "Rakshith 4SO18CS093", "Rayan    4SO18CS094"};
    glClear(GL_COLOR_BUFFER_BIT);
    setColor(POPSTAR);
    glRasterPos2f(250, 650);
    for (char i : title)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i); //main progarmme title
    }

    setColor(POPSTAR);
    glRasterPos2f(50, 600);
    for (char i : teamMemberTitle)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i); //team member names title
    }

    for (int i = 0; i < 3; i++) //draw all team members names
    {
        glRasterPos2f(65, 600 - 20 * (i + 1));
        for (int j = 0; j < names[i].length(); j++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, names[i][j]);
    }

    glRasterPos2f(250, 400);
    for (char i : "Usage Manual")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    drawCell(300, 380, 20, GOLDEN_GATE_BRIDGE);
    setColor(PACIFIC_BLUE);
    glRasterPos2f(300 + 40, 340 - 35);
    for (char i : "Starting Point")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    drawCell(300, 340, 20, EMERALD);
    setColor(PACIFIC_BLUE);
    glRasterPos2f(300 + 40, 380 - 35);
    for (char i : "Ending Point")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    drawCell(300, vh - 280, 20, FUCHSIA);
    setColor(PACIFIC_BLUE);
    glRasterPos2f(300 + 40, 280 - 20);
    for (char i : "Visited Nodes")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    drawCell(300, vh - 280 + 40, 20, MINION_YELLOW);
    setColor(PACIFIC_BLUE);
    glRasterPos2f(300 + 40, 240 - 15);
    for (char i : "Shortest Path From Starting Point To Ending Point")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    drawCell(300, vh - 280 + 80, 20, BLACK);
    setColor(PACIFIC_BLUE);
    glRasterPos2f(300 + 40, 200 - 15);
    for (char i : "Wall")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    setColor(PACIFIC_BLUE);
    glRasterPos2f(250, 140);
    for (char i : "Mouse Right Click To Choose  Operations")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    setColor(PACIFIC_BLUE);
    glRasterPos2f(50, 100);
    for (char i : "Mouse Left Click And Move To Draw Walls Or To Move Starting Point Or Ending Point")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    setColor(DARK_ORCHID);
    glRasterPos2f(250, 40);
    for (char i : "Hit   \"  SPACE BAR  \"   To Continue")
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);

    glutSwapBuffers();
    glutPostRedisplay();
}

void onSpaceBarHit(unsigned char key, int, int)
{
    if (key == ' ')
    {
        glutDisplayFunc(display);
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(vw, vh);
    glutCreateWindow("Path Finding Visualization");
    glEnable(GL_BLEND);                                //inorder to add transperrency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //inorder to add transpsrency
    init();
    glutDisplayFunc(welcomeScreen);
    glutMouseFunc(onButtonClick);
    glutMotionFunc(onMouseMove); //updates mouse position only when some mouse mouse button is clicked an moved
    glutCreateMenu(chooseOperation);
    glutAddMenuEntry("Draw walls", 1);
    glutAddMenuEntry("Move starting point", 2);
    glutAddMenuEntry("Move destination point", 3);
    glutAddMenuEntry("Run dijakstra algorithm", 4);
    glutAddMenuEntry("Clear Board", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutKeyboardFunc(onSpaceBarHit); //for exiting welcome screen
    glutMainLoop();
    return 0;
}
