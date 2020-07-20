#include "GCC.h"

enum ControllerPorts
{
    CONTROLLER_1,
    CONTROLLER_2,
    CONTROLLER_3,
    CONTROLLER_4,
};

struct GCC;

/**Description: Main function for reading inputs from the GCC
 *              searches for an adapter from the list of connected
 *              USB devices, then calls addAdapter(). Function closes
 *              after KEY_BUTTON is pressed
 * 
 * Parameters: None
 * 
 * Return: true if successful, false if not
 */
bool Setup(int key);

/** Description: checks which buttons were pressed for the designated controller
 *              at calling time
 *
 * Parameters:
 * port - the port of the controller whose inputs we want to read
 * 
 * Return: a GCC struct which contains which buttons were pressed
 *         from the controller at the assigned port
 */
struct GCC Input(int port);