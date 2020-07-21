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
    bool ret = PlaySound(sound_path, NULL, SND_FILENAME);
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
    bool  ret = CreateProcess(process_path, NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
    if(!ret) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "opening process \"%s\" failed with error (%d)\n", process_path, GetLastError());
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

/**Description: goes to the Desktop
*
*Parameters: None
*
*Return: None
*/
void goDesktop() {
    INPUT ip1;
    ip1.type = INPUT_KEYBOARD;
    ip1.ki.wScan = 0; // hardware scan code for key
    ip1.ki.time = 0;
    ip1.ki.dwExtraInfo = 0;

    INPUT ip2;
    ip2.type = INPUT_KEYBOARD;
    ip2.ki.wScan = 0; // hardware scan code for key
    ip2.ki.time = 0;
    ip2.ki.dwExtraInfo = 0;

    // Press the Windows key
    ip1.ki.wVk = 0x5B; // virtual-key code for the Windows key
    ip1.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip1, sizeof(INPUT));

    // Press the "d" key
    ip2.ki.wVk = 0x44; // virtual-key code for the "d" key
    ip2.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip2, sizeof(INPUT));

    // Release the Windows key
    ip1.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip1, sizeof(INPUT));

    // Release the "d" key
    ip2.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip2, sizeof(INPUT));

    return;
}

