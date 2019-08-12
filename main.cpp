#include <iostream>
#include <switch.h>
#include <time.h> // To make spawn value more random
using namespace std;
bool gameOver;
//Changed the width an height to better match the Switch's screen size.
const int width = 77;
const int height = 41;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirecton { STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirecton dir;
void Setup()
{
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
}
void Draw()
{
    for (int i = 0; i < width+2; i++)
    printf("#");
    printf("\n");

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                printf("#");
            if (i == y && j == x)
                printf("\033[0;32mO\033[0m");//Make snake green
            else if (i == fruitY && j == fruitX)
                printf("F");
            else
            {
                bool print = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        printf("\033[0;32mo\033[0m");//Make snake green
                        print = true;
                    }
                }
                if (!print)
                    printf(" ");
            }

            if (j == width - 1)
                printf("#");
        }
        printf("\n");
    }

    for (int i = 0; i < width+2; i++)
    printf("#");
    printf("\n");
    printf("Score:%i\n", score);
}
void Input()
{
	//Scan input for switch buttons
	hidScanInput();
	u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	//Check if statements if key was pressed this frame
    //Can't use switch statement for input with libnx
	if (kDown)
    {
		if(kDown & KEY_PLUS)
		{
			gameOver = true;
		}
		if(kDown & KEY_DLEFT)
		{
			dir = LEFT;
		}
		if(kDown & KEY_DRIGHT)
		{
			dir = RIGHT;
		}
		if(kDown & KEY_DUP)
		{
			dir = UP;
		}
		if(kDown & KEY_DOWN)
		{
			dir = DOWN;
		}
    }
}
void Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir)
    {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }
	//Changed the consequences for walking in to a wall to better suit the screen size of the switch
    //if (x > width || x < 0 || y > height || y < 0)
      //gameOver = true;
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY)
    {
        srand(time(0)); // Random seed value for rand based on time
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }
}

//Reset code for switch
void Reset()
{
	gameOver = false;//New game so reset game over state
	fill_n(tailX, 100, 0); //Reset the tail length
	fill_n(tailY, 100, 0); //Reset the tail length
	nTail = 0; //Reset the tail length
	dir = STOP; //Reset the movement direction
}

int main()
{
	consoleInit(NULL); //Init the switch console
	Restart:
    Setup();
    while (!gameOver)
    {
        Draw();
		consoleUpdate(NULL); //Update the console output
		svcSleepThread(100); //This game speed is more managable on the switch
        Input();
        Logic();
    }
	//Output the score and the instructions to start again on death
	printf("Game over!\nScore:%i\nMinus to exit or plus to play again.", score);
	consoleUpdate(NULL);
	//Game over logic. Changed from the code written by NULLx76 because that boots the user back in to hb menu on the switch
	while(true)
	{
	hidScanInput();
	u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	//When plus is pressed reset the vars and go to the restart label
	if(kDown & KEY_PLUS)
	{
		Reset();
		goto Restart;
	}
	//When minus is pressed return zero and go back to the homebrew menu
	if(kDown & KEY_MINUS)
	{
		goto Exit;
	}
	}
	Exit:
	consoleExit(NULL);
    return 0;
}
