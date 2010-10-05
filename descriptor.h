
#include "PL3/shellcode_egghunt.h"
#include "PL3/default_payload_3_41.h"
#include "PL3/default_payload_3_15.h"
#include "PL3/default_payload_3_01.h"
#include "PL3/dump_lv2.h"


#define MAGIC_NUMBER		0x50, 0x53, 0x46, 0x72, 0x65, 0x65, 0x64, 0x6d

#if defined (FIRMWARE_3_41)
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x33, 0xe7, 0x20
#elif defined (FIRMWARE_3_15)
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x33, 0xda, 0x10
#elif defined (FIRMWARE_3_01)
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x32, 0x06, 0x40
#else
#error "You must specify the target firmware." \
  " define a supported FIRMWARE_X_YZ in config.h and recompile."
#endif /* FIRMWARE_X_YZ */


#ifdef USE_JIG
#define default_shellcode shellcode_egghunt
#define default_shellcode_macro shellcode_egghunt_macro

#if defined (FIRMWARE_3_41)
#define default_payload default_payload_3_41
#define default_payload_macro default_payload_3_41_macro
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xee
#define SHELLCODE_ADDR_LOW	0x70
#elif defined (FIRMWARE_3_15)
#define default_payload default_payload_3_15
#define default_payload_macro default_payload_3_15_macro
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xde
#define SHELLCODE_ADDR_LOW	0x30
#elif defined (FIRMWARE_3_01)
#define default_payload default_payload_3_01
#define default_payload_macro default_payload_3_01_macro
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xFB
#define SHELLCODE_ADDR_LOW	0xC8
#endif /* FIRMWARE_X_YZ */


#define SHELLCODE_PAGE		0x80, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00
#define SHELLCODE_DESTINATION	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW
#define SHELLCODE_PTR 		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x08
#define SHELLCODE_ADDRESS	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x18

#define PORT1_NUM_CONFIGS	4

#else /* USE_JIG */

#define default_shellcode shellcode_egghunt
#define default_shellcode_macro shellcode_egghunt_macro
#define default_payload dump_lv2
#define default_payload_macro dump_lv2_macro

#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x00
//#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x00
//#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00
#define SHELLCODE_ADDR_LOW	0x00

#define SHELLCODE_PAGE		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW
#define SHELLCODE_DESTINATION	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x20
#define SHELLCODE_PTR 		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x28
#define SHELLCODE_ADDRESS	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x38

#define PORT1_NUM_CONFIGS	100

#endif /* USE_JIG */

#define PORT1_DESC_LEN		0xf00
#define PORT1_DESC_LEN_HI	((PORT1_DESC_LEN >> 8) & 0xFF)
#define PORT1_DESC_LEN_LO	(PORT1_DESC_LEN & 0xFF)

#define PORT3_DESC_LEN		0xa4d
#define PORT3_DESC_LEN_HI	((PORT3_DESC_LEN >> 8) & 0xFF)
#define PORT3_DESC_LEN_LO	(PORT3_DESC_LEN & 0xFF)

const uint8_t PROGMEM jig_response[64] = {
	SHELLCODE_PTR,
	SHELLCODE_ADDRESS,
	RTOC_TABLE,
	default_shellcode_macro,
};

const uint8_t PROGMEM HUB_Device_Descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x09, 0x00, 0x01, 0x08,
	0xAA, 0xAA, 0xCC, 0xCC, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x01,
};

const uint8_t PROGMEM HUB_Config_Descriptor[] = {
	// Config
	0x09, 0x02, 0x19, 0x00, 0x01, 0x01, 0x00, 0xe0,
	0x32,
	// Interface
	0x09, 0x04, 0x00, 0x00, 0x01, 0x09, 0x00, 0x00,
	0x00,
	// Endpoint (interrupt in)
	0x07, 0x05, 0x81, 0x03, 0x01, 0x00, 0x0c,
};

const uint8_t PROGMEM HUB_Hub_Descriptor[] = {
	0x09, 0x29, 0x06, 0xa9, 0x00, 0x32, 0x64, 0x00,
	0xff,
};

const uint8_t PROGMEM port1_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
	0x00, PORT1_NUM_CONFIGS,
};

const uint8_t PROGMEM port1_short_config_descriptor[] = {
	0x09, 0x02, PORT1_DESC_LEN_LO, PORT1_DESC_LEN_HI, 0x01, 0x00, 0x00, 0x80,
};

const uint8_t PROGMEM port1_config_descriptor[] = {
	0x09, 0x02, 0x12, 0x00, 0x01, 0x00, 0x00, 0x80, 0xfa,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	MAGIC_NUMBER,
#ifndef USE_JIG
	SHELLCODE_PTR,
	SHELLCODE_ADDRESS,
	RTOC_TABLE,
#endif
	default_payload_macro,
};

const uint8_t PROGMEM port2_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01,
};

const uint8_t PROGMEM port2_config_descriptor[] = {
	// config
	0x09, 0x02, 0x16, 0x00, 0x01, 0x01, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x02,
	0x00,
	// extra
	0x04, 0x21, 0xb4, 0x2f,
};

const uint8_t PROGMEM port3_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x02,
};

/*const uint8_t PROGMEM port3_config_descriptor[] = {
	0x09, 0x02, PORT3_DESC_LEN_LO, PORT3_DESC_LEN_HI, 0x01, 0x01, 0x00, 0x80, 0x01,
};*/

const uint8_t PROGMEM port3_config_descriptor[] = {
	0x09, 0x02, PORT3_DESC_LEN_LO, PORT3_DESC_LEN_HI,
	                        0x01, 0x01, 0x00, 0x80, 0x01, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
};

const uint8_t PROGMEM port4_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x03,
};

const uint8_t PROGMEM port4_config_descriptor_1[] = {
	// config
	0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00,
};

const uint8_t PROGMEM port4_short_config_descriptor_2[] = {
	// config
	0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80,
};

const uint8_t PROGMEM port4_config_descriptor_2[] = {
	// config
	0x09, 0x02, 0x00, 0x00, 0x01, 0x01, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00,
};

const uint8_t PROGMEM port4_config_descriptor_3[] = {
	0x09, 0x02, 0x30, 0x00, 0x01, 0x01, 0x00, 0x80, 0x01, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x3e, 0x21, 0x00, 0x00, 0x00, 0x00,
	MAGIC_NUMBER, /* magic number to look for in the start of the page */
	SHELLCODE_PAGE, /* Initial data search ptr */
	SHELLCODE_DESTINATION, /* destination ptr for heap structure (jig response) */
};

const uint8_t PROGMEM port5_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0x4c, 0x05, 0xeb, 0x02, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01,
};

const uint8_t PROGMEM port5_config_descriptor[] = {
	// config
	0x09, 0x02, 0x20, 0x00, 0x01, 0x00, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x02, 0xff, 0x00, 0x00,
	0x00,
	// endpoint
	0x07, 0x05, 0x02, 0x02, 0x08, 0x00, 0x00,
	// endpoint
	0x07, 0x05, 0x81, 0x02, 0x08, 0x00, 0x00,
};

