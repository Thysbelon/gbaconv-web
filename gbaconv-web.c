#include <stdint.h>
#include <stdio.h>
#ifndef __CELLOS_LV2__
#include <getopt.h>
#endif
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

enum save_type { EEPROM_512B,
	EEPROM_8K,
	FLASH_64K,
	FLASH_128K,
	SRAM,
	SAVE_UNKNOWN };

static const char* save_type_to_string(enum save_type type)
{
	switch (type) {
	case EEPROM_512B:
		return "EEPROM 4kbit";
	case EEPROM_8K:
		return "EEPROM 64kbit";
	case FLASH_64K:
		return "FLASH 512kbit";
	case FLASH_128K:
		return "FLASH 1MBit";
	case SRAM:
		return "SRAM";

	default:
		return "Unknown type";
	}
}

static bool scan_section(const uint8_t* data, unsigned size)
{
	for (unsigned i = 0; i < size; i++) {
		if (data[i] != 0xff)
			return true;
	}

	return false;
}

static enum save_type detect_save_type(const uint8_t* data, unsigned size)
{
	if (size == 512)
		return EEPROM_512B;
	if (size == 0x2000)
		return EEPROM_8K;
	if (size == 0x10000)
		return FLASH_64K;
	if (size == 0x20000)
		return FLASH_128K;
	if (size == 0x8000)
		return SRAM;

	if (size == (0x20000 + 0x2000)) {
		if (scan_section(data, 0x8000) && !scan_section(data + 0x8000, 0x1A000))
			return SRAM;
		if (scan_section(data, 0x10000) && !scan_section(data + 0x10000, 0x10000))
			return FLASH_64K;
		if (scan_section(data, 0x20000))
			return FLASH_128K;

		if (scan_section(data + 0x20000, 512) && !scan_section(data + 0x20000 + 512, 0x20000 - 512))
			return EEPROM_512B;
		if (scan_section(data + 0x20000, 0x2000))
			return EEPROM_8K;
	}

	return SAVE_UNKNOWN;
}

static void dump_srm(FILE* file, const uint8_t* data, enum save_type type)
{
	void* buf = malloc(0x20000 + 0x2000);
	memset(buf, 0xff, 0x20000 + 0x2000);

	switch (type) {
	case EEPROM_512B:
		fwrite(buf, 1, 0x20000, file);
		fwrite(data, 1, 512, file);
		fwrite(buf, 1, 0x2000 - 512, file);
		break;

	case EEPROM_8K:
		fwrite(buf, 1, 0x20000, file);
		fwrite(data, 1, 0x2000, file);
		break;

	case FLASH_64K:
		fwrite(data, 1, 0x10000, file);
		fwrite(buf, 1, 0x20000 + 0x2000 - 0x10000, file);
		break;

	case FLASH_128K:
		fwrite(data, 1, 0x20000, file);
		fwrite(buf, 1, 0x2000, file);
		break;
	
	case SRAM:
	default:
		// SRAM
		// write the save data (32 kb) to the top of the file, fill the remaining space with 0xFF to reach a size of 136 kb.
		fwrite(data, 1, 0x8000 /* 32 KB */, file);
		fwrite(buf, 1, 0x1A000, file);
		break;
	}

	free(buf);
}

static void dump_sav(FILE* file, const uint8_t* data, enum save_type type)
{
	switch (type) {
	case EEPROM_512B:
		fwrite(data + 0x20000, 1, 512, file);
		break;

	case EEPROM_8K:
		fwrite(data + 0x20000, 1, 0x2000, file);
		break;

	case FLASH_64K:
		fwrite(data, 1, 0x10000, file);
		break;

	case FLASH_128K:
		fwrite(data, 1, 0x20000, file);
		break;

	case SRAM:
	default:
		// SRAM
		// write only the first 32 kb
		fwrite(data, 1, 0x8000 /* 32 KB */, file);
		break;
	}
}

static long removeRTCdata(const uint8_t* data, long len) {
	switch (len) {
		case 512:
		case 0x2000:
		case 0x10000:
		case 0x20000:
		case 0x8000:
			EM_ASM({console.log('No RTC data detected.');});
			return len;
			break;
		default:
			EM_ASM({console.log('RTC data detected.');});
			return len - 16;
			break;
	}
}

// One shot cowboy code :)

EMSCRIPTEN_KEEPALIVE
int GBAConv(char* filename)
{
	//printf("file \"%s\"\n", filename);
	EM_ASM({console.log('filename: ' + Module.UTF8ToString($0));}, filename);
	//if (argc != 2) {
	//	fprintf(stderr, "Usage: %s <file>\n", argv[0]);
	//	return 1;
	//}

	FILE* file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Failed to open file \"%s\"\n", filename);
		EM_ASM({console.log('Failed to open file ' + Module.UTF8ToString($0));}, filename);
		goto error;
	}

	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	rewind(file);

	uint8_t* buffer = malloc(len);
	if (!buffer) {
		fprintf(stderr, "Failed to allocate memory!\n");
		EM_ASM({console.log('Failed to allocate memory');});
		goto error;
	}
	fread(buffer, 1, len, file);
	fclose(file);
	file = NULL;

	char* out_path = strdup(filename);
	char* split = strrchr(out_path, '.');
	const char* ext = NULL;

	if (split) {
		*split = '\0';
		ext = split + 1;

		if (strcasecmp(ext, "srm") == 0)
			strcat(out_path, ".sav");
		else if (strlen(ext) >= 3)
			strcat(out_path, ".srm");
		else
			ext = NULL;
	}

	if (!ext) {
		fprintf(stderr, "Cannot detect extension!\n");
		EM_ASM({console.log('Cannot detect extension!');});
		goto error;
	}
	
	//printf("len: %ld\n", len);
	EM_ASM({console.log('len: ' + $0);}, len);
	// remove RTC data. function that checks the size of the buffer, then cuts off the last 16 bytes of the buffer if the size does not match any save type. only runs for .sav files
	//EM_ASM({console.log('ext: ' + Module.UTF8ToString($0));}, ext);
	if (strcasecmp(ext, "srm") == 0) { // ext changes when the out_path changes (because strings are pointers), so if ext == "srm", that means the original file ext was "sav"
		len = removeRTCdata(buffer, len);
	}
	enum save_type type = detect_save_type(buffer, len);
	//printf("Detected save type: %s\n", save_type_to_string(type));
	EM_ASM({console.log('Detected save type: ' + Module.UTF8ToString($0));}, save_type_to_string(type));

	if (type == SAVE_UNKNOWN) {
		//fprintf(stderr, "Cannot infer save type ...\n");
		//printf("Assuming a save type of SRAM.\n");
		EM_ASM({console.log('Cannot infer save type ...');});
		EM_ASM({console.log('Assuming a save type of SRAM.');});
		//goto error;
	}

	file = fopen(out_path, "wb");
	if (!file)
		goto error;

	if (len == (0x20000 + 0x2000))
		dump_sav(file, buffer, type);
	else
		dump_srm(file, buffer, type);
	fclose(file);

	return 0;

error:
	if (file)
		fclose(file);
	return 1;
}