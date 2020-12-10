/** File which contains methods for interacting with Windows Processes
 */

#include <windows.h>
#include <stdbool.h>
#include <stdio.h>

#include "file_strings.h"

FILE * logs;

/**Description: Plays a sound file from the given path
*
*Parameters: 
*sound_path - a string which contains the path
*             to the sound file to be played.
*
*Return: returns true if successful, false if not
*/
bool playSound(char * sound_path) {
    
    bool ret = PlaySound(sound_path, NULL, (SND_FILENAME | SND_ASYNC));
    if(!ret) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "could not play sound\n");
        fclose(logs);
        return false;
    }
    return true;
}

/**Description: Opens a process with the given path
*
*Parameters: 
*process_path - a string which contains the path
*               to the process to be opened.
*
*Return: returns the PROCESS_INFORMATION structure that receives
*        identification information about the new process
*/
PROCESS_INFORMATION createProcess(char * process_path) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;   
    // set the size of the structures
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    bool  ret = CreateProcess(NULL, process_path, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
    if(!ret) {
        DWORD error = GetLastError();
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "opening process \"%s\" failed with error (%d)\n", process_path, error);
        fclose(logs);
        return pi;
    }
    return pi;
}

/**Description: checks if a process is open
*
*Parameters: 
*pi - the PROCESS_INFORMATION structure for the process
*     you want to check is still open
*
*Return: true if the process if open, false if not
*/
bool isProcessOpen(PROCESS_INFORMATION pi) {
    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    if(exit_code == STILL_ACTIVE)
        return true;
    else
        return false;
}

/**Description: closes the process and thread handles for the
*               given process
*
*Parameters: 
*pi - the PROCESS_INFORMATION structure for the process
*     you to close threads and process handles for
*
*Return: None
*/
void closeProcessAndThreadHandles(PROCESS_INFORMATION pi) {
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}


/**Description: prompts the user for the path to a file
*
*Parameters: None
*
*Return: the path to the file as a string
*/
char * getPathFromUser() {
    
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "executables (*.exe*)\0*.exe*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = "Locate Dolphin.exe";
    ofn.lpstrDefExt = "";

    char * filePath;

    if(GetOpenFileName(&ofn))
        filePath = fileName;

    return filePath;
}


