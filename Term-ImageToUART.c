// Converts a 128x128 BMP to UART for Term24
// Just testing it's graphical capabilities

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char bitmap[128*128];

int main(const int argc, const char **argv)
{
	if (argc < 4)
	{
		printf("Arguments: <input.bmp> <dark|light> <output.bin>\n");
		return 0;
	}

	FILE *input = NULL;

	input = fopen(argv[1], "rb");
	if (!input)
	{
		printf("Cannot open input file!\n");
		return 0;
	}

	FILE *output = NULL;

	output = fopen(argv[3], "wb");
	if (!output)
	{
		printf("Cannot open output file!\n");
		return 0;
	}

	unsigned char buffer;

	unsigned char red, green, blue;

	unsigned char intensity = 0x40;

	if (strcmp(argv[2], "dark") == 0) intensity = 0x00;
	else if (strcmp(argv[2], "bright") == 0) intensity = 0x40;

	for (int i=0; i<54; i++) fscanf(input, "%c", &buffer); // header

	fprintf(output, "%c;0E", 0x1B); // disable echo
	fprintf(output, "%c;0C", 0x1B); // color mode
	fprintf(output, "%c;%c%cA", 0x1B, 0x50, 0x00); // address command
	fprintf(output, "%c;%c%cW", 0x1B, 0x40, 0x00); // write command

	for (int y=127; y>=0; y--)
	{
		for (int x=0; x<128; x++)
		{
			fscanf(input, "%c%c%c", &blue, &green, &red);

			buffer = ((red & 0xC0) >> 2) | ((green & 0xC0) >> 4) | ((blue & 0xC0) >> 6);
			buffer |= intensity;

			bitmap[y*128+x] = buffer;
		}
	}

	for (int i=0; i<16384; i++) fprintf(output, "%c", bitmap[i]);	

	fclose(input);
	fclose(output);

	printf("Complete\n");
	printf("Now run: cat %s > /dev/ttyUSB0\n", argv[3]);
	printf("Make sure 'picocom' is up and running!\n");

	return 1;
}
