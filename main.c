#include <stdio.h>
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdbool.h>
#include <string.h>

#include "GCA.h"
#include "processes.h"
#include "file_strings.h"

FILE * logs;

//gets the paths for dolphin and the sound to be played
// and the key_button from config.txt
bool getPaths(char * dolphin, char * sound, char * key_button) {

    FILE * paths = fopen(CONFIGS_TXT, "r");
    if(paths == false) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "Unable to locate config.txt\n");
        fclose(logs);
        return false;
    }
    char buf[100];
    char * token;

    //skip the first 11 lines because they are just instructions
    for(int i = 0; i < 11; ++i)
        fgets(buf, 100, paths);
    
    //buf should contain the dolphin path
    fgets(buf, 100, paths);
    //token is now DOLPHIN_PATH
    token = strtok(buf, "=");
    //now it is the actual path
    token = strtok(NULL, "\n");
    if(token == NULL) {
        logs = fopen(LOGS_TXT, "a");
        printf("No Dolphin Path\n");
        fclose(logs);
        fclose(paths);
        return false;
    }
    strcpy(dolphin, token);
    //buf should contain the sound path
    fgets(buf, 100, paths);
    //token is now START_SOUND_PATH
    token = strtok(buf, "=");
    //now it is the actual path
    token = strtok(NULL, "\n");
    if(token == NULL) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "No sound file\n");
        fclose(logs);
        sound[0] = '\0';
        goto noSound;
    }
    strcpy(sound, token);
    noSound:
    //buf should contain the key_button
    fgets(buf, 100, paths);
    //token is now KEY_BUTTON
    token = strtok(buf, "=");
    //now it is the button code
    token = strtok(NULL, "\n");
    if(token == NULL) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "No KEY_BUTTON\n");
        fclose(logs);
        fclose(paths);
        return false;
    }
    strcpy(key_button, token);
    fclose(paths);
    return true;
}


int main(int argc, char * argv[]) {

    //use this code to hide the console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow( hWnd, SW_MINIMIZE );  //won't hide the window without SW_MINIMIZE
    ShowWindow( hWnd, SW_HIDE );

    char dolphin_path_buf[100];
    char sound_path_buf[100];
    char key_button_buf[100];
    char * dolphin_path;
    char * sound_path;
    int key_button;

    PROCESS_INFORMATION pi;


    //create a new logs.txt every time this program is opened again
    logs = fopen(LOGS_TXT, "w");
    fclose(logs);


    bool ret = getPaths(dolphin_path_buf, sound_path_buf, key_button_buf);
    if(!ret) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "Unable to read DOLPHIN_PATH or KEY_BUTTON\n");
        fclose(logs);
        return 1;
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

    start:
    //this will connect the adapter and read inputs until the KEY_BUTTON is pressed
    ret = Setup(key_button);
    //the function returns false if we were unable to open the adapter for some reason
    if(!ret)
        return 1;

    //now all we have to do is play the sound and open dolphin
    //--------------------------------------------------------------------
    //play the sound
    if(sound_path[0] != '\0')
        playSound(sound_path);
    //make dolphin the main window
    goDesktop();
    //here is where we start dolphin
    pi = createProcess(dolphin_path);
    //if the function failed, return
    if(pi.hProcess == NULL) {
        return 1;
    }
    //while dolphin is still running, just hang the process
    while(isProcessOpen(pi)) {
        Sleep(5000);
    }
    // Close process and thread handles.
    closeProcessAndThreadHandles(pi);
    //once dolphin is no longer running, branch to the start of main
    goto start; 
    return 0;

}


