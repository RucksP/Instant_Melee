/** Header file for parseConfig.c
 *  parseConfig.c is a file for methods which involve reading custom user settings and paths
 */

#ifndef PARSECONFIG_H
#define PARSECONFIG_H

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
bool parseConfig(char * dolphin, char * sound, char * key_button);

#endif