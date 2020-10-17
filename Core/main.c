/** Main file for instant_melee.exe
 * 
 * This program scans your GameCube Adapter for an input from CONTROLLER_1
 * once key_button is pressed, Project Slippi will launch
 * 
 */

#include <stdio.h>
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdbool.h>

#include "GCA.h"
#include "processes.h"
#include "file_strings.h"
#include "parseConfig.h"

FILE * logs;

int main(int argc, char * argv[]) {

    //use this code to hide the console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow( hWnd, SW_MINIMIZE );  //won't hide the window without SW_MINIMIZE
    ShowWindow( hWnd, SW_HIDE );

    bool ret;
    char dolphin_path_buf[100];
    char sound_path_buf[100];
    char key_button_buf[10];
    char * dolphin_path;
    char * sound_path;
    int key_button;

    PROCESS_INFORMATION pi;

    //create a new logs.txt every time this program is opened again
    logs = fopen(LOGS_TXT, "w");
    fclose(logs);

    //check if this is the first time the program was opened
    if( isConfigInit() ) {
        ret = parseConfig(dolphin_path_buf, sound_path_buf, key_button_buf);
        if(!ret) {
            return 1;
        }
    } else {    
        ret = initConfig(dolphin_path_buf, sound_path_buf, key_button_buf);
        if(!ret) {
            return 1;
        }
    }

    dolphin_path = dolphin_path_buf;
    sound_path = sound_path_buf;
    //convert string to int
    key_button = strtol(key_button_buf, NULL, 0);
    if(key_button == 0) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "Unable to convert KEY_BUTTON \"%s\" to int\n", key_button_buf);
        fclose(logs);
        return 1;
    }

    vars_initalized:
    //this will connect the adapter and read inputs until the key_button is pressed
    ret = Setup(key_button);
    //the function returns false if we were unable to open the adapter for some reason
    if(!ret)
        return 1;

    //now all we have to do is play the sound and open dolphin
    //--------------------------------------------------------------------
    
    //play the sound if there is one
    if(sound_path[0] != '\0')
        playSound(sound_path);

    //here is where we start dolphin
    pi = createProcess(dolphin_path);
    //if the function failed, return
    if(pi.hProcess == NULL) {
        return 1;
    } 

    //while dolphin is still running, just hang instant_melee
    while(isProcessOpen(pi)) {
        Sleep(5000);
    }
    // Close process and thread handles.
    closeProcessAndThreadHandles(pi);
    
    //once dolphin is no longer running, branch to the earliest point in main
    //where most variables were initialized
    goto vars_initalized;

    return 0;

}


