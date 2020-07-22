/** File for methods which involve reading custom user settings and paths
 */ 

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "parseConfig.h"
#include "file_strings.h"
#include "processes.h"

FILE * logs;

/** Description: gets the paths for dolphin and the sound to be played 
 *               and the key_button from config.txt
 * 
 * Parameters: dolphin - the char buffer which will contain the path to Dolphin
 *             sound - the char buffer which will contain the path to the sound
 *                     to be played
 *             keybutton - the char buffer which will contain the str version
 *                         of the hex code of the key button
 * 
 * Return: true on successful read, false on failure
 * 
 */
bool parseConfig(char * dolphin, char * sound, char * key_button) {

    FILE * paths = fopen(CONFIGS_TXT, "r");
    //temp file, because we might have to recreate txt if user didnt input path yet
    FILE * temp = fopen(TEMP_FILE, "w");
    if(paths == false) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "Unable to locate config.txt\n");
        fclose(logs);
        return false;
    }
    char buf[100];
    char * token;

    //copy first 11 lines because they are just instructions
    for(int i = 0; i < 11; ++i) {
        fgets(buf, 100, paths);
        fprintf(temp, "%s", buf);
    }

    //buf should contain the dolphin path
    fgets(buf, 100, paths);
    //token is now DOLPHIN_PATH
    token = strtok(buf, "=");
    //now it is the actual path
    token = strtok(NULL, "\n");
    if(token == NULL) {   
        token = getPathFromUser();
    }
    fprintf(temp, "DOLPHIN_PATH=%s\n", token);
    strcpy(dolphin, token);
    //buf should contain the sound path
    fgets(buf, 100, paths);
    fprintf(temp, "%s", buf);
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
    fprintf(temp, "%s", buf);
    //token is now KEY_BUTTON
    token = strtok(buf, "=");
    //now it is the button code
    token = strtok(NULL, "\n");
    if(token == NULL) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "No KEY_BUTTON\n");
        fclose(logs);
        fclose(temp);
        fclose(paths);
        return false;
    }
    strcpy(key_button, token);

    //now we copy the lines which explain what buttons are what
    for(int i = 0; i <17; ++i) {
        fgets(buf, 100, paths);
        fprintf(temp, "%s", buf);        
    }

    fclose(paths);
    fclose(temp);
    //remove(CONFIGS_TXT);
    //rename(TEMP_FILE, CONFIGS_TXT);
  
    return true;
}

