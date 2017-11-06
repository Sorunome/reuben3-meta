#include "text.h"

struct Strings_LUT {
	uint16_t offset;
	uint8_t chunk;
};

#include "data/strings.h"

extern uint8_t* decompression_buffer;
