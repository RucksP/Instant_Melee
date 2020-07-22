/** Header file for processes.c
 * processes.c is a file which contains methods for interacting with Windows Processes 
 */

#ifndef PROCESSES_H
#define PROCESSES_H

#include <windows.h>

/**Description: Plays a sound file from the given path
*
*Parameters: 
*sound_path - a string which contains the path
*             to the sound file to be played.
*
*Return: returns true if successful, false if not
*/
bool playSound(char * sound_path);

/**Description: checks if a process is open
*
*Parameters: 
*pi - the PROCESS_INFORMATION structure for the process
*     you want to check is still open
*
*Return: returns true if the process if open, false if not
*/
PROCESS_INFORMATION createProcess(char * process_path);

/**Description: checks if a process is open
*
*Parameters: 
*pi - the PROCESS_INFORMATION structure for the process
*     you want to check is still open
*
*Return: true if the process if open, false if not
*/
bool isProcessOpen(PROCESS_INFORMATION pi);

/**Description: closes the process and thread handles for the
*               given process
*
*Parameters: None
*
*Return: None
*/
void closeProcessAndThreadHandles(PROCESS_INFORMATION pi);

char * getPathFromUser();

#endif