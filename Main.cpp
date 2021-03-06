#define WINVER 0x0500
#include <windows.h>
#include <Tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <limits>

using namespace std;

//Version
#define VERSION 1.2

//ERROR cases
#define noKeyPress 0
#define noKeyRelease 1
#define windowNotFound 2
#define noForeground 3
#define batError 4

//delay between key press and key release
#define inputDelay 1

//global variables
int interval;
TCHAR windowtext[MAX_PATH];
HWND window;


//Error handling
void Error(int err)
{
    switch(err)
    {
        case noKeyPress:
            cout << "\nFailed to send key. Aborting!" << endl;
            break;
        case noKeyRelease:
            cout << "\nFailed to release key. Aborting!" << endl;
            break;
        case windowNotFound:
            cout << "\nWindow not found!" << endl;
            break;
        case noForeground:
            cout << "\nFailed to put window to foreground. Aborting!" << endl;
            break;
        case batError:
            cout << "\nThe program is not running or the batfile is corrupt!" << endl;
            break;
    }
}

//Custom sleep function
bool Wait(long seconds)
{
    //Loop here because we need to check for user input
	for(int i = 0; i<seconds; i++)
    {
        if(GetAsyncKeyState(VK_ESCAPE))
        {
            return false;
        }
        Sleep(1000);
    }
    return true;
}

void Info()
{
    cout << "\n\t\t*************************\n";
    cout <<   "\t\t*   ~~ NEVER AFK ~~     *\n" ;
    cout <<   "\t\t*      BETA v. "
                      << VERSION << "      *\n" ;
    cout <<   "\t\t*      <carlmats>       *\n";
    cout <<   "\t\t************************* \n\n";
}

void GiveInfo(bool newKeystroke)
{
    static int NOT = 0;
    system("CLS");
    Info();
    if(newKeystroke)
    {
        cout << endl
             << "Window: " << windowtext << endl
             << "Interval: " << interval << " sec" << endl
             << "Simulated keystrokes: " << ++NOT << endl
             << endl
             << "Press ESC to exit..." << endl;
    }
}

//Search for the user specified window
static bool FindProgram(const char* program)
{
    // Get first window on desktop
    HWND firstwindow = FindWindowEx(NULL, NULL, NULL, NULL);
    window = firstwindow;

    // We need to get the console title in case we
    // accidentally match the search word with it
    // instead of the intended target window.
    TCHAR consoletitle[MAX_PATH];
    GetConsoleTitle(consoletitle, MAX_PATH);

    while(1)
    {
        // Check window title for a match
        GetWindowText(window, windowtext, MAX_PATH);
        if (strstr(windowtext, program) != NULL && strcmp(windowtext, consoletitle) != 0)
                break;

        // Get next window
        window = FindWindowEx(NULL, window, NULL, NULL);
        if (window == NULL || window == firstwindow)
        {
            Error(windowNotFound);
            return 0;
        }
    }

    cout << "Window found : " << windowtext << endl;

    return 1;
}

int ValidInputInt()
{
    int result;
    cin >> result;

    if(!cin)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Enter a number please." << endl;
        return -1;
    }
    else
        return result;
}

//Handle user input
void InputHandler()
{
    char prog[100];

    do{
        cout <<  "\nName the window that should receive the key simulations: " ;
        cin.getline(prog, 100);
    }while(!FindProgram(prog) );

    do{
        cout << "\nChoose interval between key simulations: ";
        interval = ValidInputInt();
    }while(interval<0);
}

bool InputHandler(const char* inputWindowText, int inputInterval )
{
    if(!FindProgram(inputWindowText ))
    {
        Error(batError);
        return false;
    }
    strcpy(windowtext, inputWindowText);
    interval = inputInterval;
    return true;
}

bool PostKey(UINT key)
{

    if(PostMessage(window, WM_KEYDOWN, key, 0) == 0)
    {
        Error(noKeyPress);
        return false;
    }

    else
    {
        Sleep(inputDelay*1000);   //Sleep between key press and release
        if(PostMessage(window, WM_KEYUP, key, 0) == 0)
        {
            Error(noKeyRelease);
            return false;
        }

    }

    return true;
}



int main(int argc, char *argv[])
{
    bool running=true;

    Info();
    if(argc < 2)
    {
        InputHandler();
    }
    else
    {
        if(!InputHandler(argv[1],atoi(argv[2])) )
            running = false;
    }

    if(interval < inputDelay)      //We are already waiting inputDelay(s) between press and release
        interval = inputDelay;

    while(running)
    {
        if(!PostKey(0x20))  //Press space, change this to whatever key you want
            running = false;
        else
            GiveInfo(true);

        if(!Wait(interval-inputDelay))
            running = false;
    }

    cout << "\nThe program will now exit...\n";
    Sleep(5000);

    return 0;

}




