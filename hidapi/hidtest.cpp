// currently only reads from the keyboard, does not display anything
// useful yet


#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

unsigned char nothingPressed[] = { 0x01, 0x00, 0xff, 0xfe, 0x3b, 0xeb, 0x6e,
0x4f, 0xba, 0xc3, 0x59, 0xaa, 0xe0, 0x6d, 0xc1, 0xb7, 0x76 };

char decode_beyondtel(unsigned char* buf, int res) {
    char key = '\0';

    if (res != 17) {
	printf("Invalid package length, cannot decode\n");
	return -1;
    }

    for (int i=0; i<4; i++) {
	if (buf[i] != nothingPressed[i]) {
	    if (i == 1) {
		switch (buf[i]) {
		case 0x01:
		    key = 'c';
		    break;
		case 0x07:
		    key = 'O';
		    break;
		case 0x13:
		    key = 'l';
		    break;
		case 0x0d:
		    key = 'r';
		    break;
		case 0x04:
		    key = '1';
		    break;
		case 0x03:
		    key = '2';
		    break;
		case 0x02:
		    key = '3';
		    break;
		case 0x0a:
		    key = '4';
		    break;
		case 0x09:
		    key = '5';
		    break;
		case 0x08:
		    key = '6';
		    break;
		case 0x10:
		    key = '7';
		    break;
		case 0x0f:
		    key = '8';
		    break;
		case 0x0e:
		    key = '9';
		    break;
		case 0x16:
		    key = '*';
		    break;
		case 0x15:
		    key = '0';
		    break;
		case 0x14:
		    key = '#';
		    break;
		default:
		    key = '?';
		}
	    } else {
		printf("Invalid package received, cannot decode\n");
		return -1;
	    }
	}
    }
    return key;
}

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

	struct hid_device_info *devs, *cur_dev;
	
	if (hid_init())
		return -1;

	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;	
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;
	

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	handle = hid_open(0x04b4, 0x0302, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);
	
	// Try to read from the device. There shoud be no
	// data here, but execution should not block.
	res = hid_read(handle, buf, 17);

	// Toggle LED (cmd 0x80). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 17);
	if (res < 0) {
		printf("Unable to write()\n");
		printf("Error: %ls\n", hid_error(handle));
	}
	

	// Request state (cmd 0x81). The first byte is the report number (0x1).

#ifdef fo0
	unsigned char buf2[] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x06, 0xbe, 0x40, 0x06, 0x7d, 0xc7, 0x60, 0xd7, 0xc7,
	0x00, 0x00, 0x00, 0x00, 0x00 };
#endif

unsigned char start0[] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char start1[] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x06,
0xbe, 0x40, 0x06, 0x7d, 0xe8, 0x6c, 0x70, 0xc7, 0x00, 0x10, 0x10, 0x10,
0x10, 0x10};

#ifdef beyondtel_start
	hid_write(handle, start0, 17);
		   usleep(500*1000); // just trying, might be useless
		   usleep(500*1000); // just trying, might be useless
	hid_write(handle, start1, 17);
#endif

#ifdef c2215
unsigned char buf3[] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xbe,
0x40, 0x06, 0x7d, 0xc7, 0x60, 0xd7, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00};
unsigned char buf4[] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00};
unsigned char buf5[] = { 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00};

	hid_write(handle, buf3, 17);
	
	hid_write(handle, buf4, 17);
	
	hid_write(handle, buf5, 17);
#endif

	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().

	char last_key = 0;

	while (1) {
	    res = 0;
	    while (res == 0) {
		    res = hid_read(handle, buf, sizeof(buf));
		    if (res == 0)
			    printf("waiting...\n");
		    if (res < 0)
			    printf("Unable to read()\n");
		    #ifdef WIN32
		    Sleep(50);
		    #else
		    usleep(50*1000);
		    #endif
	    }

	    // Print out the returned buffer.
	    char key_pressed = decode_beyondtel(buf, res);
	    if (key_pressed != last_key) {
		if (key_pressed)
		    printf("key pressed: %c\n", key_pressed);
		last_key = key_pressed;
	    }

	    if (key_pressed == -1) {
		printf("Data read:\n   ");
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	    }
	}

	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

	return 0;
}
