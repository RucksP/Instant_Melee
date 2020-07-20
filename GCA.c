
#include <libusb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "GCC.h"
#include "GCA.h"
#include "file_strings.h"

FILE * logs;

libusb_device **devices;
libusb_device_handle * adapter_handle;
libusb_device * adapter;
libusb_context * context = NULL;
ssize_t cnt;

/** the adapter passes inputs as an array of 37 unsigned 8 bit integers
 * indexes  1-9  consist of the inputs for CONTROLLER_1
 * indexes 10-18 consist of the inputs for CONTROLLER_2
 * indexes 19-27 consist of the inputs for CONTROLLER_3
 * indexes 28-36 consist of the inputs for CONTROLLER_4
 */
static uint8_t controller_payload[37];
static uint8_t controller_payload_swap[37];
static uint8_t swap_temp[37];
static int controller_payload_size = 0;
static uint8_t controller_types[4] = {CONTROLLER_NONE, CONTROLLER_NONE, CONTROLLER_NONE, CONTROLLER_NONE};
int adapter_status = 0;
static uint8_t endpoint_in = 0;
static uint8_t endpoint_out = 0;
bool readInputs = false;
//if no key_button was passed just use start
int key_button;

/** Description: swaps the values of load1 and load2.
 *               Warning: Only works for arrays of size 37
 *
 * Parameters:
 * load1 - an array of 8 bit unsigned integers
 * load2 - an array of 8 bit unsigned integers
 * 
 * Return: None
 */
void swap( uint8_t * load1, uint8_t * load2) {
    uint8_t temp;
    for(int i = 0; i < 37; ++i) {
        temp = load2[i];
        load2[i] = load1[i];
        load1[i] = temp;
    }
}

/** Description: copies the values from load1 into load2.
 *               Warning: Only works for arrays of size 37
 *
 * Parameters:
 * load1 - an array of 8 bit unsigned integers which we want to copy from
 * load2 - an array of 8 bit unsigned integers which we want to copy to
 * 
 * Return: None
 */
void copy(uint8_t * load1, uint8_t * load2) {
    for(int i = 0; i < 37; ++i) {
        load2[i] = load1[i];
    }
}

/** Description: begins reading from the adapter and stops reading when
 *               key_button has been pressed
 *
 * Parameters: None
 * 
 * Return: None
 */
static void Read() {
    int payload_size = 0;
    // this while loop should run at the polling rate of the adapter
    // is there a better way to read inputs than this while loop?
    while(readInputs) {

        //this command reads inputs from the adapter
        int err = libusb_interrupt_transfer(adapter_handle, endpoint_in, controller_payload_swap, sizeof(controller_payload_swap), &payload_size, 16);
        if (err) {
            logs = fopen(LOGS_TXT, "a");
            fprintf(logs,"adapter libusb read failed: err=%s", libusb_error_name(err));
            fclose(logs);
        }
        {
            swap(controller_payload, controller_payload_swap);
            controller_payload_size = payload_size;
        }

        //read the button input
        //only read from controller 1 
        struct GCC pad;
        pad = Input(CONTROLLER_1);
        if((pad.button & key_button) == key_button) 
            readInputs = false;
        //Sleep(150);
    }
    
}

/** Description: adds the GC adapter and begins reading from it
 *
 * Parameters:
 * device - a libusb_device* for the device we want to start reading from.
 *          it should be a GC adapter
 * 
 * Return: None
 */
static void addAdapter(libusb_device* device) {

    //get the endpoints
    //we do this because im not sure if endpoints are the same between Mayflash and Nintendo Adapters
    struct libusb_config_descriptor* config = NULL;
    libusb_get_config_descriptor(device, 0, &config);
    for (uint8_t ic = 0; ic < config->bNumInterfaces; ic++)
    {
        const struct libusb_interface* interfaceContainer = &config->interface[ic];
        for (int i = 0; i < interfaceContainer->num_altsetting; i++)
        {
            const struct libusb_interface_descriptor* interface = &interfaceContainer->altsetting[i];
            for (uint8_t e = 0; e < interface->bNumEndpoints; e++)
            {
                const struct libusb_endpoint_descriptor* endpoint = &interface->endpoint[e];
                if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN)
                    endpoint_in = endpoint->bEndpointAddress;
                else
                    endpoint_out = endpoint->bEndpointAddress;
            }
        }
    }
    libusb_free_config_descriptor(config);

    //begin reading from the adapter
    int tmp = 0;
    unsigned char payload = 0x13;
    libusb_interrupt_transfer(adapter_handle, endpoint_out, &payload, sizeof(payload), &tmp, 16);
    readInputs = true;
    adapter_status = 1;
    Read();
    
    
}

/** Description: this function checks whether the device we are
 *               looking for is a GC adapter in addition to making
 *               sure we have access to it
 *
 * Parameters:
 * device - a libusb_device* for the device we want to check is a GC adapter
 * 
 * Return: true if the passed device is a GC adapter and if we have access to the device,
 *         false if otherwise
 */
static bool checkValidAccess(libusb_device* device) {

     struct libusb_device_descriptor desc;
     int ret = libusb_get_device_descriptor(device, &desc);
     if(ret) {
         logs = fopen(LOGS_TXT, "a");
         fprintf(logs, "libusb_get_device_descriptor failed with error: %d", ret);
         fclose(logs);
         return false;
     }

    //check if this device is a GC adapter
     if(desc.idVendor != 0x057e || desc.idProduct != 0x0337) {
         //wrong device
         return false;
     }

     ret = libusb_open(device, &adapter_handle);
     if (ret == LIBUSB_ERROR_ACCESS) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "Lazy_Melee does not have access to this device\n");
        fclose(logs);
     }     
     if (ret) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "libusb_open failed to open device with error = %d\n", ret);
        fclose(logs);
        return false;
     }

    //check for kernel bs
    ret = libusb_kernel_driver_active(adapter_handle, 0);
    if (ret == 1)
    {
         ret = libusb_detach_kernel_driver(adapter_handle, 0);
         if (ret != 0 && ret != LIBUSB_ERROR_NOT_SUPPORTED) {
            logs = fopen(LOGS_TXT, "a");
            fprintf(logs, "libusb_detach_kernel_driver failed with error: %d\n", ret);
            fclose(logs);
            return false;
         }
    }

     // this split is needed so that we don't avoid claiming the interface when
     // detaching the kernel driver is successful
    if (ret != 0 && ret != LIBUSB_ERROR_NOT_SUPPORTED)
    {
         libusb_close(adapter_handle);
         adapter_handle = NULL;
         return false;
    }

    // This call makes Nyko-brand (and perhaps other) adapters work.
    // However it returns LIBUSB_ERROR_PIPE with Mayflash adapters.
    const int transfer = libusb_control_transfer(adapter_handle, 0x21, 11, 0x0001, 0, NULL, 0, 1000);
    if (transfer < 0) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "libusb_control_transfer failed with error: %d\n", transfer);
        fclose(logs);
    }
    //claim the interface
    ret = libusb_claim_interface(adapter_handle, 0);
    if(ret) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "libusb_claim_interface failed with error: %d\n", ret);
        fclose(logs);
        libusb_close(adapter_handle);
        adapter_handle = NULL;
        return false;
    }

    return true;

}


/** Description: checks which buttons were pressed for the designated controller
 *              at calling time
 *
 * Parameters:
 * port - the port of the controller whose inputs we want to read
 * 
 * Return: a GCC struct which contains which buttons were pressed
 *         from the controller at the assigned port
 */
struct GCC Input(int port) {

    struct GCC pad = {};

    if(adapter_handle == NULL || adapter_status == 0) {
        //return an empty pad
        return pad;
    }

    int payload_size = 0;
    uint8_t controller_payload_copy[37];

    {
        copy(controller_payload, controller_payload_copy);
        payload_size = controller_payload_size;
    }

    
    //if the transfer was incomplete
    if(payload_size != sizeof(controller_payload_copy) ||
        controller_payload_copy[0] != LIBUSB_DT_HID) {
            logs = fopen(LOGS_TXT, "a");
            fprintf(logs, "error reading payload (size: %d, type: %02x)\n", payload_size,
              controller_payload_copy[0]);
            fclose(logs);
    }

    else {
        uint8_t type = controller_payload_copy[1 + (9*port)] >> 4;
        if(type != 0 && controller_types[port] == CONTROLLER_NONE) {
            logs = fopen(LOGS_TXT, "a");
            fprintf(logs, "New device connected to Port %d of Type: %02x\n", port + 1,
                 controller_payload_copy[1 + (9 * port)]);
            fclose(logs);
        }
        
        controller_types[port] = type;

        if(controller_types[port] != CONTROLLER_NONE) {
            //this contains the values for if ABXY or the DPAD were pressed for this controller
            uint8_t b1 = controller_payload_copy[1 + (9 * port) + 1];
            //this contains the values for if LRZ or Start were pressed for this controller
            uint8_t b2 = controller_payload_copy[1 + (9 * port) + 2];
        
            if (b1 & (1 << 0))
                pad.button |= PAD_BUTTON_A;
            if (b1 & (1 << 1))
                pad.button |= PAD_BUTTON_B;
            if (b1 & (1 << 2))
                pad.button |= PAD_BUTTON_X;
            if (b1 & (1 << 3))
                pad.button |= PAD_BUTTON_Y;

            if (b1 & (1 << 4))
                pad.button |= PAD_BUTTON_LEFT;
            if (b1 & (1 << 5))
                pad.button |= PAD_BUTTON_RIGHT;
            if (b1 & (1 << 6))
                pad.button |= PAD_BUTTON_DOWN;
            if (b1 & (1 << 7))
                pad.button |= PAD_BUTTON_UP;

            if (b2 & (1 << 0))
                pad.button |= PAD_BUTTON_START;
            if (b2 & (1 << 1))
                pad.button |= PAD_TRIGGER_Z;
            if (b2 & (1 << 2))
                pad.button |= PAD_TRIGGER_R;
            if (b2 & (1 << 3))
                pad.button |= PAD_TRIGGER_L;

        }
    }
    return pad;

}


/**Description: Main function for reading inputs from the GCC
 *              searches for an adapter from the list of connected
 *              USB devices, then calls addAdapter(). Function closes
 *              after KEY_BUTTON is pressed
 * 
 * Parameters: None
 * 
 * Return: true if successful, false if not
 */
bool Setup(int key) {

    //set the key_button as 
    key_button = key;

    //init context
    int ret = libusb_init(&context);
    if(ret < 0) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "failed to initiate context");
        fclose(logs);
    }
    //get device list
    cnt = libusb_get_device_list(context, &devices);
    if(cnt < 0) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "error in getting device list");
        fclose(logs);
    }
    //iterate through all of our connected devices
    for(ssize_t i = 0; i < cnt; ++i) {
        //check if this device is the one we are looking for
        if(checkValidAccess(devices[i])) {
            //if so add the adapter, only the first one though
            addAdapter(devices[i]);
            break;
        }

    }

    if(adapter_status == 0) {
        logs = fopen(LOGS_TXT, "a");
        fprintf(logs, "did not open adapter\n");
        fclose(logs);
        return false;
    }



    //release all handles and relinquish control of the device to prevent memory leaks
    //and to let other programs take control of the adapter
    libusb_release_interface(adapter_handle, 0);
    libusb_close(adapter_handle);
    libusb_free_device_list(devices, 1); //free the list, unref the devices in it
	libusb_exit(context);
    return true;

}