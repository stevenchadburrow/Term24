
// Term24 - VGA Terminal Module using the PIC24.
// Supports: UART, SPI, I2C, and PS/2 Keyboard.
// Includes color mode for 128x128 resultion at 128-colors, with scrolling or layers.

/*
XGA Signal 1024 x 768 @ 60 Hz timing

General timing
Screen refresh rate	60 Hz
Vertical refresh	48.363095238095 kHz
Pixel freq.	65.0 MHz
Horizontal timing (line)
Polarity of horizontal sync pulse is negative.

Scanline part	Pixels	Time [µs]
Visible area	1024	15.753846153846
Front porch	24	0.36923076923077
Sync pulse	136	2.0923076923077
Back porch	160	2.4615384615385
Whole line	1344	20.676923076923
Vertical timing (frame)
Polarity of vertical sync pulse is negative.

Frame part	Lines	Time [ms]
Visible area	768	15.879876923077
Front porch	3	0.062030769230769
Sync pulse	6	0.12406153846154
Back porch	29	0.59963076923077
Whole frame	806	16.6656
*/

// PIC24EP512GP204 Configuration Bit Settings

// 'C' source line config statements

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = OFF             // PLL Lock Enable bit (Clock switch will not wait for the PLL lock signal.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Source Selection (Fast RC Oscillator with divide-by-N with PLL module (FRCPLL))
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <p24Exxxx.h>

// unused memory here
volatile unsigned char __attribute__((address(0x3A00))) term_pixels[80];
volatile unsigned char __attribute__((address(0x3A50))) term_buffer[80];
volatile unsigned int __attribute__((address(0x3AA0))) term_scanline;
volatile unsigned int __attribute__((address(0x3AA2))) term_location;
volatile unsigned int __attribute__((address(0x3AA4))) term_counter;
volatile unsigned int __attribute__((address(0x3AA6))) term_stretch;
volatile unsigned int __attribute__((address(0x3AA8))) term_scroll;
volatile unsigned int __attribute__((address(0x3AAA))) term_orientation;
volatile unsigned int __attribute__((address(0x3AAC))) term_cursor; 
volatile unsigned int __attribute__((address(0x3AAE))) term_mode; // 0 = terminal, 1 = color
volatile unsigned int __attribute__((address(0x3AB0))) term_parallel;
volatile unsigned int __attribute__((address(0x3AB2))) term_last_channel;
volatile unsigned int __attribute__((address(0x3AB4))) term_last_address;
volatile unsigned int __attribute__((address(0x3AB6))) term_ps2_release;
volatile unsigned int __attribute__((address(0x3AB8))) term_ps2_extended;
volatile unsigned int __attribute__((address(0x3ABA))) term_ps2_shift;
volatile unsigned int __attribute__((address(0x3ABC))) term_ps2_capslock;
volatile unsigned int __attribute__((address(0x3ABE))) term_ps2_synced;
volatile unsigned int __attribute__((address(0x3AC0))) term_sequence;
volatile unsigned int __attribute__((address(0x3AC2))) term_command;
volatile unsigned int __attribute__((address(0x3AC4))) term_print;
volatile unsigned int __attribute__((address(0x3AC6))) term_dma_address;
volatile unsigned int __attribute__((address(0x3AC8))) term_dma_data; 
volatile unsigned int __attribute__((address(0x3ACA))) term_position;
volatile unsigned int __attribute__((address(0x3ACC))) term_setting_cursor;
volatile unsigned int __attribute__((address(0x3ACE))) term_setting_echo;
volatile unsigned char __attribute__((address(0x3AD0))) term_keycode[8];
// unused memory here
volatile unsigned char __attribute__((address(0x3B00))) term_ps2_conversion[256];
volatile unsigned int __attribute__((address(0x3C00))) term_array[256];
// unused memory here (generally used by heap, 512 bytes)
volatile unsigned char __attribute__((address(0x4000))) term_memory[2048];
volatile unsigned char __attribute__((address(0x4800))) term_map[2048];
volatile unsigned char __attribute__((address(0x5000))) term_color[12288];
volatile __eds__ unsigned char __attribute__((address(0x8000), eds)) term_color_eds[20480];

void __attribute__((section("usercode"))) term_data(unsigned int a, unsigned char d)
{
	if (a >= 0x4000 && a < 0x4800)
	{
		term_memory[a-0x4000] = d;
	}
	else if (a >= 0x4800 && a < 0x5000)
	{
		term_map[a-0x2800] = d;
	}
	else if (a >= 0x5000 && a < 0x8000)
	{
		term_color[a-0x5000] = d;
	}
	else if (a >= 0x8000 && a < 0xD000)
	{
		term_color_eds[a-0x8000] = d;
	}
};

extern void __attribute__((section("usercode"))) TermScanline(void);

// interrupt latency is 10 cycles
// for some reason, having auto_psv instead of no_auto_psv makes this work?!
void __attribute__((interrupt, auto_psv, section("usercode"))) _T1Interrupt(void)
{
	// C always includes:
	// lnk #0
  
	TermScanline();
	
	// C always includes:
	// ulnk
	// retfie
};

// text character mapping, 8-bytes per character
const __prog__ unsigned char __attribute__((space(prog), section("usercode"))) text_map[768] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00, 
	0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x50, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x50, 0x00, 
	0x70, 0xA8, 0xA0, 0x70, 0x28, 0xA8, 0x70, 0x00, 
	0x88, 0x08, 0x10, 0x20, 0x40, 0x80, 0x88, 0x00, 
	0x60, 0x90, 0x90, 0x60, 0xA8, 0x90, 0x68, 0x00, 
	0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 
	0x40, 0x20, 0x20, 0x20, 0x20, 0x20, 0x40, 0x00, 
	0x88, 0x50, 0x20, 0xF8, 0x20, 0x50, 0x88, 0x00, 
	0x00, 0x00, 0x20, 0x70, 0x20, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x40, 0x00, 
	0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 
	0x08, 0x08, 0x10, 0x20, 0x40, 0x80, 0x80, 0x00, 
	0x70, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00, 
	0x20, 0x60, 0xA0, 0x20, 0x20, 0x20, 0xF8, 0x00, 
	0x70, 0x88, 0x08, 0x70, 0x80, 0x80, 0xF8, 0x00, 
	0x70, 0x88, 0x08, 0x70, 0x08, 0x88, 0x70, 0x00, 
	0x18, 0x28, 0x48, 0x88, 0xF8, 0x08, 0x08, 0x00, 
	0xF8, 0x80, 0x80, 0xF0, 0x08, 0x88, 0x70, 0x00, 
	0x70, 0x88, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00, 
	0xF8, 0x08, 0x10, 0x20, 0x20, 0x20, 0x20, 0x00, 
	0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00, 
	0x70, 0x88, 0x88, 0x78, 0x08, 0x88, 0x70, 0x00, 
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 
	0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x40, 0x00, 
	0x00, 0x00, 0x20, 0x40, 0x20, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x70, 0x00, 0x70, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x20, 0x10, 0x20, 0x00, 0x00, 0x00, 
	0x70, 0x88, 0x08, 0x10, 0x20, 0x00, 0x20, 0x00, 
	0x70, 0x88, 0xA8, 0xA8, 0xB8, 0x80, 0xF8, 0x00, 
	0x20, 0x50, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00, 
	0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00, 
	0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00, 
	0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00, 
	0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x00, 
	0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x00, 
	0x70, 0x88, 0x80, 0xB8, 0x88, 0x88, 0x78, 0x00, 
	0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00, 
	0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0xF8, 0x00, 
	0x38, 0x10, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00, 
	0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00, 
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00, 
	0x88, 0xD8, 0xA8, 0x88, 0x88, 0x88, 0x88, 0x00, 
	0x88, 0x88, 0xC8, 0xA8, 0x98, 0x88, 0x88, 0x00, 
	0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 
	0xF0, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x80, 0x00, 
	0x70, 0x88, 0x88, 0x88, 0xA8, 0x90, 0x68, 0x00, 
	0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0x88, 0x00, 
	0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00, 
	0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 
	0x88, 0x88, 0x88, 0x88, 0xA8, 0xD8, 0x88, 0x00, 
	0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00, 
	0x88, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x00, 
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00, 
	0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x00, 
	0x80, 0x80, 0x40, 0x20, 0x10, 0x08, 0x08, 0x00, 
	0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x60, 0x00, 
	0x20, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 
	0x40, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00, 
	0x80, 0x80, 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x00, 
	0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00, 
	0x08, 0x08, 0x78, 0x88, 0x88, 0x88, 0x78, 0x00, 
	0x00, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78, 0x00, 
	0x18, 0x20, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x00, 
	0x00, 0x00, 0x78, 0x88, 0x78, 0x08, 0x70, 0x00, 
	0x80, 0x80, 0xF0, 0x88, 0x88, 0x88, 0x88, 0x00, 
	0x20, 0x00, 0xE0, 0x20, 0x20, 0x20, 0xF8, 0x00, 
	0x08, 0x00, 0x38, 0x08, 0x08, 0x88, 0x70, 0x00, 
	0x80, 0x80, 0x88, 0x90, 0xE0, 0x90, 0x88, 0x00, 
	0xE0, 0x20, 0x20, 0x20, 0x20, 0x20, 0xF8, 0x00, 
	0x00, 0x00, 0xD0, 0xA8, 0xA8, 0xA8, 0xA8, 0x00, 
	0x00, 0x00, 0xF0, 0x88, 0x88, 0x88, 0x88, 0x00, 
	0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 
	0x00, 0x00, 0xF0, 0x88, 0xF0, 0x80, 0x80, 0x00, 
	0x00, 0x00, 0x78, 0x88, 0x78, 0x08, 0x08, 0x00, 
	0x00, 0x00, 0xB0, 0xC8, 0x80, 0x80, 0x80, 0x00, 
	0x00, 0x00, 0x78, 0x80, 0x70, 0x08, 0xF0, 0x00, 
	0x40, 0x40, 0xF0, 0x40, 0x40, 0x48, 0x30, 0x00, 
	0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 
	0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 
	0x00, 0x00, 0xA8, 0xA8, 0xA8, 0xA8, 0x50, 0x00, 
	0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, 
	0x00, 0x00, 0x88, 0x88, 0x78, 0x08, 0x70, 0x00, 
	0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00, 
	0x10, 0x20, 0x20, 0x60, 0x20, 0x20, 0x10, 0x00, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 
	0x40, 0x20, 0x20, 0x30, 0x20, 0x20, 0x40, 0x00, 
	0x00, 0x00, 0x00, 0x68, 0x90, 0x00, 0x00, 0x00, 
	0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x00, // 0x7F, DEL, used for cursor?
};

// modified by-hand some
const __prog__ unsigned char __attribute__((space(prog), section("usercode"))) text_conversion[256] = {
	// regular
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x60, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0x31, 0x00, // 1x
	0x00, 0x00, 0x7A, 0x73, 0x61, 0x77, 0x32, 0x00,
	0x00, 0x63, 0x78, 0x64, 0x65, 0x34, 0x33, 0x00, // 2x
	0x00, 0x20, 0x76, 0x66, 0x74, 0x72, 0x35, 0x00,
	0x00, 0x6E, 0x62, 0x68, 0x67, 0x79, 0x36, 0x00, // 3x
	0x00, 0x00, 0x6D, 0x6A, 0x75, 0x37, 0x38, 0x00,
	0x00, 0x2C, 0x6B, 0x69, 0x6F, 0x30, 0x39, 0x00, // 4x 
	0x00, 0x2E, 0x2F, 0x6C, 0x3B, 0x70, 0x2D, 0x00, 
	0x00, 0x00, 0x27, 0x00, 0x5B, 0x3D, 0x00, 0x00, // 5x
	0x00, 0x00, 0x0D, 0x5D, 0x00, 0x5C, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, // 6x
	0x00, 0x31, 0x00, 0x34, 0x37, 0x00, 0x00, 0x00, 
	0x30, 0x2E, 0x32, 0x35, 0x36, 0x38, 0x1B, 0x00, // 7x
	0x00, 0x2B, 0x33, 0x2D, 0x2A, 0x39, 0x00, 0x00, 
	// term_ps2_shifted
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 8x
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x7E, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x21, 0x00, // 9x
	0x00, 0x00, 0x5A, 0x53, 0x41, 0x57, 0x40, 0x00, 
	0x00, 0x43, 0x58, 0x44, 0x45, 0x24, 0x23, 0x00, // Ax
	0x00, 0x00, 0x56, 0x46, 0x54, 0x52, 0x25, 0x00, 
	0x00, 0x4E, 0x42, 0x48, 0x47, 0x2B, 0x5E, 0x00, // Bx
	0x00, 0x00, 0x4D, 0x4A, 0x55, 0x26, 0x2A, 0x00, 
	0x00, 0x3C, 0x4B, 0x49, 0x4F, 0x29, 0x28, 0x00, // Cx
	0x00, 0x3E, 0x3F, 0x4C, 0x3A, 0x50, 0x5F, 0x00, 
	0x00, 0x00, 0x22, 0x00, 0x7B, 0x2B, 0x00, 0x00, // Dx
	0x00, 0x00, 0x0D, 0x7D, 0x00, 0x7C, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, // Ex
	0x00, 0x17, 0x00, 0x14, 0x19, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x12, 0x00, 0x13, 0x11, 0x1B, 0x00, // Fx
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};


void __attribute__((section("usercode"))) text_draw(int x, int y, unsigned char c)
{
	term_memory[y*80+x] = (unsigned int)c;
};

void __attribute__((section("usercode"))) text_string(int x, int y, const __prog__ char *s)
{
	for (int i=0; i<80; i++)
	{
		if (s[i] == '\\') return;

		text_draw(x+i, y, s[i]);
	}
};

char __attribute__((section("usercode"))) text_high_nibble(unsigned char c)
{
	if ((c & 0xF0) >= 0xA0)
	{
		return ((c & 0xF0) >> 4) - 10 + 'A';
	}
	else
	{
		return ((c & 0xF0) >> 4) + '0';
	}
};

char __attribute__((section("usercode"))) text_low_nibble(unsigned char c)
{
	if ((c & 0x0F) >= 0x0A)
	{
		return (c & 0x0F) - 10 + 'A';
	}
	else
	{
		return (c & 0x0F) + '0';
	}
};

void __attribute__((interrupt, auto_psv, section("usercode"))) _U1RXInterrupt(void)
{
	IFS0bits.U1RXIF = 0; // Clear RX Interrupt flag
};

void __attribute__((interrupt, auto_psv, section("usercode"))) _U1TXInterrupt(void)
{
	IFS0bits.U1TXIF = 0; // Clear TX Interrupt flag
};

void __attribute__((interrupt, auto_psv, section("usercode"))) _SPI2Interrupt(void)
{
	IFS2bits.SPI2IF = 0; // clear flag
}

void __attribute__((interrupt, auto_psv, section("usercode"))) _INT1Interrupt(void)
{
	IFS1bits.INT1IF = 0; // clear flag

	unsigned int pa = (PORTA & 0x000F); // get port values
	unsigned int pb = ((PORTB & 0x003C) << 2);
	
	term_last_channel = 0x0000; // indicate channel use
	term_last_address += 2; // increment channel address
	
	term_array[term_last_address] = (pb | pa); // store in array
}

void __attribute__((interrupt, auto_psv, section("usercode"))) _DMA0Interrupt(void)
{	
	IFS0bits.DMA0IF = 0; // clear flag
};

void __attribute__((section("usercode"))) setup()
{
	// default values to allow h-sync and v-sync as output with mono output
	TRISA = 0xFFEF;
	LATA = 0x0000;
	TRISB = 0x7F7F;
	LATB = 0x0000;

	// internal settings that might be changed
	term_setting_cursor = 1;
	term_setting_echo = 1;

	// sets Output Compare to appropriate pins
	RPOR2 = (RPOR2  & 0x00FF) | 0x1000; // OC1 / V-SYNC on RP39
	RPOR0 = (RPOR0 & 0xFF00) | 0x0011; // OC2 / H-SYNC on RP20

	// Timer2 is used with Output Compare for V-SYNC signal
	OC1CON1 = 0; // clear output compare bits
	OC1CON2 = 0; // clear output compare bits
	OC1CON1bits.OCTSEL = 0x00; // uses Timer2
	OC1R = 0x0000; // v-sync rise
	OC1RS = 0x41D2; // v-sync fall
	T2CON = 0; // set timer
	T2CONbits.T32 = 0; // both timers are 16-bit
	TMR2 = 0x0000; // zero counter
	T2CONbits.TCKPS = 0x02; // prescale of 1:64 selected
	PR2 = 0x421D; // v-reset (minus one!)
	OC1CON2bits.SYNCSEL = 0x0C; // uses Timer2
	OC1CON1bits.OCM = 0x05; // double continuous pulse
	
	// Timer3 is used with Output Compare for H-SYNC signal
	OC2CON1 = 0; // clear output compare bits
	OC2CON2 = 0; // clear output compare bits
	OC2CON1bits.OCTSEL = 0x01; // uses Timer3
	OC2R = 0x0000; // h-sync rise
	OC2RS = 0x04B8; // h-sync fall
	T3CON = 0; // reset timer
	TMR3 = 0x0000; // zero counter
	PR3 = 0x053F; // h-reset (minus one!)
	OC2CON2bits.SYNCSEL = 0x0D; // uses Timer3
	OC2CON1bits.OCM = 0x05; // double continuous pulse

	// Timer1 used for interrupts to race-the-beam
	T1CONbits.TON = 0; // turn timer off
	T1CONbits.TCS = 0; // use internal instructions
	T1CONbits.TGATE = 0; // disable gated mode
	T1CONbits.TCKPS = 0x00; // no prescalar
	TMR1 = 0x053F; // timer counter, horizontal adjustment
	PR1 = 0x053F; // timer period on last line of v-blank (minus one!)
	IPC0bits.T1IP = 0x07; // highest interrupt priority
	IFS0bits.T1IF = 0; // interrupt flag
	IEC0bits.T1IE = 1; // enable interrupt

	// turn on interrupts globally here!
	SRbits.IPL = 0x00; // cpu interrupt priority
	CORCONbits.IPL3 = 0; // cpu interrupt 7 or less
	INTCON1 = 0; // clear registers
	INTCON2 = 0;
	INTCON3 = 0;
	INTCON4 = 0;
	INTTREG = 0x0000; // clear interrupt vectors
	INTCON1bits.NSTDIS = 0; // nested interrupts enabled
	INTCON2bits.GIE = 1; // global interrupts enabled

	// clear video memory
	for (int i=0; i<12288; i++)
	{
		term_color[i] = 0x00;
	}

	for (int i=0; i<20480; i++)
	{
		term_color_eds[i] = 0x00;
	}

	for (int i=0; i<2048; i++)
	{
		term_memory[i] = 0x0020; // spaces
	}

	for (int i=0; i<256; i++)
	{
		term_map[i] = 0x00; // 32x special characters
	}

	for (int i=0; i<768; i++)
	{
		term_map[i+256] = text_map[i]; // 96x regular characters
	}

	for (int i=0; i<256; i++)
	{
		term_map[i+1024] = 0x00; // 32x remappable characters
	}

	for (int i=0; i<768; i++)
	{
		term_map[i+1280] = (text_map[i] ^ 0xFF); // 96x inverted characters
	}

	for (int i=0; i<256; i++)
	{
		term_array[i] = 0x0000;
	}

	for (int i=0; i<256; i++)
	{
		term_ps2_conversion[i] = text_conversion[i];
	}

	for (int i=0; i<80; i++)
	{
		term_pixels[i] = 0x00;
		term_buffer[i] = 0x00;
	}

	// set video scanline to line after v-blank
	term_scanline = 777; // 777 default, vertical adjustment
	term_location = 0;
	term_counter = 0;
	term_stretch = 0;
	term_scroll = 0;
	term_orientation = 0;
	term_cursor = 0;
	term_position = 0;
	term_mode = 0;

	asm("mov.w #0x8000, w1"); // bit 15
	asm("mov.w 0x0110, w0");
	asm("ior.w w0, w1, w0");
	asm("mov.w w0, 0x0110"); // timer 2 on (v-sync)
	
	asm("mov.w #0x8000, w1"); // bit 15
	asm("mov.w 0x0112, w0");
	asm("ior.w w0, w1, w0");
	asm("mov.w w0, 0x0112"); // timer 3 on (h-sync)

	asm("mov.w #0x8000, w1"); // bit 15
	asm("mov.w 0x0104, w0");
	asm("ior.w w0, w1, w0");
	asm("mov.w w0, 0x0104"); // timer 1 on (interrupt)
};

void __attribute__((section("usercode"))) run()
{
	text_string(0, 24, "Term24\\");
	text_string(10, 24, "ESC;H for Help\\");

	term_parallel = 0;
	term_last_channel = 0x000F;
	term_last_address = 0x0000;
	term_ps2_release = 0;
	term_ps2_extended = 0;
	term_ps2_shift = 0;
	term_ps2_capslock = 0;
	term_ps2_synced = 0;
	term_sequence = 0;
	term_command = 0;
	term_print = 0;
	term_dma_address = 0;
	term_dma_data = 0;
	term_keycode[0] = 0;
	term_keycode[1] = 0;
	term_keycode[2] = 0;
	term_keycode[3] = 0;
	term_keycode[4] = 0;
	term_keycode[5] = 0;
	term_keycode[6] = 0;
	term_keycode[7] = 0;

	term_scroll = 0;
	term_cursor = 1840;
	if (term_setting_cursor > 0) term_memory[term_cursor] = 0xA0; // inverted space

	CNPUB = 0x000C; // pull-up on RB3 and RB2

	for (unsigned int i=0; i<32768; i++) { for (unsigned int j=0; j<64; j++) { } } // delay

	// read external switches for different input types
	int option = ((((PORTB & 0x000C) >> 2) ^ 0x0003) & 0x0003);

	CNPUB = 0x0000; // turn off pull-ups on RB3 and RB2

	if (option == 0) // UART
	{
		TRISA = 0x000F;
		LATA = 0x0000;
		TRISB = 0x7F3F;
		LATB = 0x0000;

		// sets UART1 to appropriate pins
		RPINR18 = 0x0025; // UART1-RX on RP37
		RPOR2 = (RPOR2 & 0xFF00) | 0x0001; // UART1-TX on RP38

		// set up UART1
		IEC0bits.U1TXIE = 0; // disable transmit interrupts
		IEC0bits.U1RXIE = 0; // disable receive interrupts
		IFS0bits.U1TXIF = 0; // clear flag
		IFS0bits.U1RXIF = 0; // clear flag
		IPC2bits.U1RXIP = 0x01; // lowest interrupt priority 
		IPC3bits.U1TXIP = 0x01; // lowest interrupt priority 
		U1MODE = 0x0000; // disable and clear everything, 8-bit, no parity, 1 stop bit, etc.
		U1STA = 0x0000; // clear all flags
		U1BRG = (65000000 / 9600) / 16 - 1; // baud rate divisor = (65000000 / 9600) / 16 - 1 = 422.177 (or 115200 baud = 34.2647)
		U1MODEbits.UARTEN = 1; // enable uart module (needs to be on before UTXEN is enabled)
		U1STAbits.UTXEN = 1; // enable transmit
		//IEC0bits.U1TXIE = 1; // enable transmit interrupts
		//IEC0bits.U1RXIE = 1; // enable receive interrupts

		// set up DMA0 with UART1
		DMA0CON = 0x0000; // continuous, no ping-pong, word-aligned
		DMA0CNT = 255; // 256 bytes in array
		DMA0REQ = 0x000B; // UART1-RX
		DMA0PAD = (volatile unsigned int)&U1RXREG;
		DMA0STAL = (volatile unsigned int)&term_array;
		DMA0STAH = 0x0000;
		IFS0bits.DMA0IF = 0;
		IEC0bits.DMA0IE = 1;
		DMA0CONbits.CHEN = 1;

		// send dummy characters
		U1TXREG = '\r'; // dummy transfers
		U1TXREG = '\n';

		text_string(30, 24, "UART: 9600-8-N-1\\");
	}
	else if (option == 1) // PS/2
	{
		TRISA = 0x000F;
		LATA = 0x0000;
		TRISB = 0x7F7F;
		LATB = 0x0000;

		// sets UART1 to appropriate pins
		RPINR18 = 0x0025; // UART1-RX on RP37
		//RPOR2 = (RPOR2 & 0xFF00) | 0x0001; // UART1-TX on RP38

		// set up UART1
		IEC0bits.U1TXIE = 0; // disable transmit interrupts
		IEC0bits.U1RXIE = 0; // disable receive interrupts
		IFS0bits.U1TXIF = 0; // clear flag
		IFS0bits.U1RXIF = 0; // clear flag 
		IPC2bits.U1RXIP = 0x01; // lowest interrupt priority 
		IPC3bits.U1TXIP = 0x01; // lowest interrupt priority 
		U1MODE = 0x0004; // disable and clear everything, 8-bit, odd parity, 1 stop bits, etc.
		U1STA = 0x0000; // clear all flags
		U1BRG = (65000000 / 16667) / 16 - 1; // baud rate divisor, will change later
		U1MODEbits.UARTEN = 1; // enable uart module (needs to be on before UTXEN is enabled)
		//U1STAbits.UTXEN = 1; // enable transmit
		//IEC0bits.U1TXIE = 1; // enable transmit interrupts
		//IEC0bits.U1RXIE = 1; // enable receive interrupts

		// set up DMA0 with UART1
		DMA0CON = 0x0000; // continuous, no ping-pong, word-aligned
		DMA0CNT = 255; // 256 bytes in array
		DMA0REQ = 0x000B; // UART1-RX
		DMA0PAD = (volatile unsigned int)&U1RXREG;
		DMA0STAL = (volatile unsigned int)&term_array;
		DMA0STAH = 0x0000;
		IFS0bits.DMA0IF = 0;
		IEC0bits.DMA0IE = 1;
		DMA0CONbits.CHEN = 1;

		// delay and then detect baud rate
		for (unsigned int i=0; i<32768; i++) { for (unsigned int j=0; j<64; j++) { } } // delay
		U1MODEbits.ABAUD = 1; // detect baud rate of UART1 (by pressing = sign on the keyboard)

		text_string(30, 24, "PS/2: Press = to sync\\");
	}
	else if (option == 2) // SPI
	{
		TRISA = 0x000F;
		LATA = 0x0000;
		TRISB = 0x7F6F;
		LATB = 0x0000;

		// sets SPI2 to appropriate pins
		RPINR22 = 0x2625; // SPI2-CLK on RP38 and SPI2-MOSI on RP37
		RPOR2 = (RPOR2 & 0xFF00) | 0x0009; // SPI2-CLK on RP38 (needed?)
		RPOR1 = (RPOR1 & 0xFF00) | 0x0008; // SPI2-MISO on RP36 (needed?)

		// set up SPI2
		SPI2BUF = 0; // clear buffer first
		IFS2bits.SPI2IF = 0; // clear flag
		IEC2bits.SPI2IE = 0; // disable interrupts
		IPC8bits.SPI2IP = 0x01; // lowest interrupt priority 
		SPI2STAT = 0x0001; // interrupt when data on receive buffer 
		SPI2CON1 = 0x0000; // 8-bit slave mode
		SPI2CON2 = 0x0001; // enhanced buffer enabled
		IFS2bits.SPI2IF = 0; // clear flag
		//IEC2bits.SPI2IE = 1; // enable interrupts
	
		// set up DMA0 with UART1
		DMA0CON = 0x0000; // continuous, no ping-pong, word-aligned
		DMA0CNT = 255; // 256 bytes in array
		DMA0REQ = 0x0021; // SPI2 Transfer Done
		DMA0PAD = (volatile unsigned int)&SPI2BUF;
		DMA0STAL = (volatile unsigned int)&term_array;
		DMA0STAH = 0x0000;
		IFS0bits.DMA0IF = 0;
		IEC0bits.DMA0IE = 1;
		DMA0CONbits.CHEN = 1;

		text_string(30, 24, "SPI\\");
	}
	else if (option == 3) // PARALLEL
	{
		TRISA = 0x000F;
		LATA = 0x0000;
		TRISB = 0x7F7F;
		LATB = 0x0000;

		// sets INT1 to appropriate pins
		RPINR0 = 0x2600; // INT1 on RP38
		
		// set up INT1
		IFS1bits.INT1IF = 0; // clear flag
		IEC1bits.INT1IE = 0; // disable interrupts
		INTCON2bits.INT1EP = 1; // interrupt on negative edge
		IPC5bits.INT1IP = 0x02; // near-lowest interrupt priority 
		IFS1bits.INT1IF = 0; // clear flag
		IEC1bits.INT1IE = 1; // enable interrupts

		term_parallel = 1;

		text_string(30, 24, "PARALLEL\\");
	}

	while (1)
	{
		term_last_channel = (DMALCA & 0x000F);
		term_last_address = ((DSADRL + 2) & 0x01FF);

		if (term_last_channel == 0x0000)
		{
			if ((((term_position) & 0x00FF) << 1) != term_last_address)
			{
				if (option == 0 || option == 2 || option == 3) // UART/SPI/PARALLEL
				{
					if (term_dma_data == 0)
					{
						term_keycode[term_sequence] = (term_array[(term_position & 0x00FF)] & 0x00FF);

						if (term_setting_echo > 0 && term_command != 2)
						{
							if (option == 0) U1TXREG = term_keycode[term_sequence];
						}
						term_print = 1;
					}
					else
					{
						term_data(term_dma_address, (term_array[(term_position & 0x00FF)] & 0x00FF));

						term_dma_address++;
						term_dma_data--;
					}

					term_position++;
					if (term_position >= 256) term_position -= 256;
				}
				else if (option == 1) // PS/2
				{
					if (U1MODEbits.ABAUD == 0 && term_ps2_synced == 0) term_ps2_synced = 1;

					if (term_ps2_synced > 0)
					{
						if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0xE0) // term_ps2_extended
						{
							term_ps2_extended = 1;
						}
						else if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0xF0) // term_ps2_release
						{
							term_ps2_release = 1;
						}
						else if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x58) // term_ps2_capslock
						{
							if (term_ps2_release == 0) term_ps2_capslock = 1 - term_ps2_capslock;
							else term_ps2_release = 0;
						}
						else if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x12 ||
							(term_array[(term_position & 0x00FF)] & 0x00FF) == 0x59) // term_ps2_shift
						{
							if (term_ps2_release == 0) term_ps2_shift = 1;
							else
							{
								term_ps2_shift = 0;
								term_ps2_release = 0;
							}
						}
						else
						{
							if (term_ps2_release == 0)
							{
								unsigned int conv = (((term_array[(term_position & 0x00FF)] & 0x00FF) + 0x80 * term_ps2_shift + 0x80 * term_ps2_capslock + 0x80 * term_ps2_extended) & 0x00FF);

								term_keycode[term_sequence] = term_ps2_conversion[conv];

								if (term_ps2_extended > 0 && (conv == 0x4A || conv == 0xCA)) term_keycode[term_sequence] = 0x2F; // numpad divide

								if (term_keycode[term_sequence] == 0x11) // DC1 = arrow up
								{
									term_keycode[0] = 0x1B;
									term_keycode[1] = 0x5B;
									term_keycode[2] = 0x41;

									term_command = 1;
									term_sequence = 2;

									if (term_setting_echo > 0 && term_command != 2)
									{
										if (option == 0)
										{
											U1TXREG = term_keycode[0];
											U1TXREG = term_keycode[1];
											U1TXREG = term_keycode[2];
										}
									}
								}
								else if (term_keycode[term_sequence] == 0x12) // DC2 = arrow down
								{
									term_keycode[0] = 0x1B;
									term_keycode[1] = 0x5B;
									term_keycode[2] = 0x42;

									term_command = 1;
									term_sequence = 2;

									if (term_setting_echo > 0 && term_command != 2)
									{
										if (option == 0)
										{
											U1TXREG = term_keycode[0];
											U1TXREG = term_keycode[1];
											U1TXREG = term_keycode[2];
										}
									}
								}
								else if (term_keycode[term_sequence] == 0x13) // DC3 = arrow right
								{
									term_keycode[0] = 0x1B;
									term_keycode[1] = 0x5B;
									term_keycode[2] = 0x43;

									term_command = 1;
									term_sequence = 2;

									if (term_setting_echo > 0 && term_command != 2)
									{
										if (option == 0)
										{
											U1TXREG = term_keycode[0];
											U1TXREG = term_keycode[1];
											U1TXREG = term_keycode[2];
										}
									}
								}
								else if (term_keycode[term_sequence] == 0x14) // DC3 = arrow left
								{
									term_keycode[0] = 0x1B;
									term_keycode[1] = 0x5B;
									term_keycode[2] = 0x44;

									term_command = 1;
									term_sequence = 2;

									if (term_setting_echo > 0 && term_command != 2)
									{
										if (option == 0)
										{
											U1TXREG = term_keycode[0];
											U1TXREG = term_keycode[1];
											U1TXREG = term_keycode[2];
										}
									}
								}
								else if (term_keycode[term_sequence] == 0x17) // ETB = end
								{
									term_keycode[0] = 0x1B;
									term_keycode[1] = 0x5B;
									term_keycode[2] = 0x46;

									term_command = 1;
									term_sequence = 2;

									if (term_setting_echo > 0 && term_command != 2)
									{
										if (option == 0)
										{
											U1TXREG = term_keycode[0];
											U1TXREG = term_keycode[1];
											U1TXREG = term_keycode[2];
										}
									}
								}
								else if (term_keycode[term_sequence] == 0x19) // EM = home
								{
									term_keycode[0] = 0x1B;
									term_keycode[1] = 0x5B;
									term_keycode[2] = 0x48;

									term_command = 1;
									term_sequence = 2;

									if (term_setting_echo > 0 && term_command != 2)
									{
										if (option == 0)
										{
											U1TXREG = term_keycode[0];
											U1TXREG = term_keycode[1];
											U1TXREG = term_keycode[2];
										}
									}
								}
								else
								{
									if (term_setting_echo > 0 && term_command != 2)
									{
										if (option == 0)
										{
											U1TXREG = term_keycode[term_sequence];
										}
									}
								}
								term_print = 1;
							}

							term_ps2_extended = 0;
							term_ps2_release = 0;
						}
					}

					term_position++;
					if (term_position >= 256) term_position -= 256;
				}
			}		
		}

		if (term_print > 0)
		{
			term_print = 0;

			if (term_setting_cursor > 0 && term_cursor < 1920) 
			{
				term_memory[(term_scroll*80+term_cursor)%1920] = ((term_memory[(term_scroll*80+term_cursor)%1920] + 0x80) & 0x00FF);
			}

			if (term_sequence == 0)
			{
				if (term_keycode[0] == 0x1B) // escape
				{
					term_sequence = 1;
				}
				else if (term_keycode[0] == 0x08) // backspace
				{
					if ((term_cursor % 80) > 1) term_cursor--;
				}
				else if (term_keycode[0] == 0x09) // tab
				{
					term_cursor += 8;
					if ((term_cursor % 80) < 8) term_cursor -= (term_cursor % 80) + 1;
					term_cursor -= (term_cursor % 8);
				}
				else if (term_keycode[0] == 0x0A) // line feed
				{
					term_cursor -= (term_cursor % 80);
					if (term_cursor < 1840) term_cursor += 80;
				}
				else if (term_keycode[0] == 0x0C) // form feed (same as line feed?)
				{
					term_cursor -= (term_cursor % 80);
					if (term_cursor < 1840) term_cursor += 80;
				}
				else if (term_keycode[0] == 0x0D) // return
				{
					term_cursor -= (term_cursor % 80);
				}
				else if (term_keycode[0] >= 0x20 && term_keycode[0] < 0x7F) // regular characters
				{
					if (term_cursor >= 1920)
					{
						term_cursor -= 80;
						for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
						term_scroll++;
						if (term_scroll >= 24) term_scroll -= 24;
					}
					term_memory[(term_scroll*80+term_cursor)%1920] = term_keycode[0];
					term_cursor++;
				}	
				else if (term_keycode[0] == 0x7F) // delete
				{
					// do nothing
				}
				else if (term_keycode[0] >= 0x80 && term_keycode[0] < 0xA0) // remappable characters
				{
					if (term_cursor >= 1920)
					{
						term_cursor -= 80;
						for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
						term_scroll++;
						if (term_scroll >= 24) term_scroll -= 24;
					}
					term_memory[(term_scroll*80+term_cursor)%1920] = term_keycode[0];
					term_cursor++;
				}
				else if (term_keycode[0] >= 0xA0 && term_keycode[0] < 0xFF) // inverted characters
				{
					if (term_cursor >= 1920)
					{
						term_cursor -= 80;
						for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
						term_scroll++;
						if (term_scroll >= 24) term_scroll -= 24;
					}
					term_memory[(term_scroll*80+term_cursor)%1920] = term_keycode[0];
					term_cursor++;
				}
				else if (term_keycode[0] == 0xFF) // unused
				{
					// do nothing
				}
			}
			else if (term_sequence == 1)
			{
				if (term_keycode[1] == 0x5B) // left bracket for control term_sequence introducer
				{
					term_command = 1; // ansi escape codes
					term_sequence = 2;
				}
				else if (term_keycode[1] == 0x3B) // semicolon for personal term_commands
				{
					term_command = 2; // personal escape codes
					term_sequence = 2;
				}
				else
				{
					term_command = 0;
					term_sequence = 0;
				}
			}
			else if (term_sequence >= 2 && term_command == 1) // ansi escape codes
			{
				if ((term_keycode[term_sequence] >= 0x41 && term_keycode[term_sequence] <= 0x5A) ||
					(term_keycode[term_sequence] >= 0x61 && term_keycode[term_sequence] <= 0x7A) ||
					term_sequence >= 7) // ends with letter or max term_sequence length
				{
					if (term_keycode[term_sequence] == 0x41) // A for up
					{
						switch (term_sequence)
						{
							case 2:
							{
								if (term_cursor >= 80) term_cursor -= 80;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if (term_cursor >= 80) term_cursor -= 80;
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if (term_cursor >= 80) term_cursor -= 80;
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x42) // B for down
					{
						switch (term_sequence)
						{
							case 2:
							{
								if (term_cursor < 1840) term_cursor += 80;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if (term_cursor < 1840) term_cursor += 80;
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if (term_cursor < 1840) term_cursor += 80;
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x43) // C for right
					{
						switch (term_sequence)
						{
							case 2:
							{
								if ((term_cursor % 80) < 79) term_cursor++;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if ((term_cursor % 80) < 79) term_cursor++;
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if ((term_cursor % 80) < 79) term_cursor++;
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x44) // D for left
					{
						switch (term_sequence)
						{
							case 2:
							{
								if ((term_cursor % 80) > 0) term_cursor--;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if ((term_cursor % 80) > 0) term_cursor--;
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if ((term_cursor % 80) > 0) term_cursor--;
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x45) // E for next line
					{
						switch (term_sequence)
						{
							case 2:
							{
								term_cursor -= (term_cursor % 80);
								if (term_cursor < 1840) term_cursor += 80;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									term_cursor -= (term_cursor % 80);
									if (term_cursor < 1840) term_cursor += 80;
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									term_cursor -= (term_cursor % 80);
									if (term_cursor < 1840) term_cursor += 80;
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x46) // F for previous line
					{
						switch (term_sequence)
						{
							case 2:
							{
								term_cursor -= (term_cursor % 80);
								if (term_cursor >= 80) term_cursor -= 80;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									term_cursor -= (term_cursor % 80);
									if (term_cursor >= 80) term_cursor -= 80;
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									term_cursor -= (term_cursor % 80);
									if (term_cursor >= 80) term_cursor -= 80;
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x47) // G for horizontal absolute
					{
						switch (term_sequence)
						{
							case 2:
							{
								term_cursor -= (term_cursor % 80);
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								term_cursor -= (term_cursor % 80);
								term_cursor += val;
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								term_cursor -= (term_cursor % 80);
								term_cursor += val;
								break;
							}	
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x48) // H for cursor position
					{
						switch (term_sequence)
						{
							case 2:
							{
								term_cursor = 0;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								if (val <= 0) val = 1;
								term_cursor = 80 * (val - 1);
								break;
							}
							case 4:
							{
								if (term_keycode[term_sequence-2] != ';' && term_keycode[term_sequence-1] != ';')
								{
									int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
									if (val <= 0) val = 1;
									term_cursor = 80 * (val - 1);
								}
								else if (term_keycode[term_sequence-2] == ';')
								{
									int val = (int)(term_keycode[term_sequence-1]-'0');
									if (val <= 0) val = 1;
									term_cursor = (val - 1);
								}
								else if (term_keycode[term_sequence-1] == ';')
								{
									int val = (int)(term_keycode[term_sequence-2]-'0');
									if (val <= 0) val = 1;
									term_cursor = 80 * (val - 1);
								}
								break;
							}
							case 5:
							{
								if (term_keycode[term_sequence-3] == ';')
								{
									int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
									if (val <= 0) val = 1;
									term_cursor = (val - 1);
								}
								else if (term_keycode[term_sequence-2] == ';')
								{
									int val1 = (int)(term_keycode[term_sequence-3]-'0');
									if (val1 <= 0) val1 = 1;
									int val2 = (int)(term_keycode[term_sequence-1]-'0');
									if (val2 <= 0) val2 = 1;
									term_cursor = 80 * (val1 - 1) + (val2 - 1);
								}
								else if (term_keycode[term_sequence-1] == ';')
								{
									int val = (int)(term_keycode[term_sequence-3]-'0') * 10 + (int)(term_keycode[term_sequence-2]-'0');
									if (val <= 0) val = 1;
									term_cursor = 80 * (val - 1);
								}
								break;
							}
							case 6:
							{
								if (term_keycode[term_sequence-3] == ';')
								{
									int val1 = (int)(term_keycode[term_sequence-4]-'0');
									if (val1 <= 0) val1 = 1;
									int val2 = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
									if (val2 <= 0) val2 = 1;
									term_cursor = 80 * (val1 - 1) + (val2 - 1);
								}
								else if (term_keycode[term_sequence-2] == ';')
								{
									int val1 = (int)(term_keycode[term_sequence-4]-'0') * 10 + (int)(term_keycode[term_sequence-3]-'0');
									if (val1 <= 0) val1 = 1;
									int val2 = (int)(term_keycode[term_sequence-1]-'0');
									if (val2 <= 0) val2 = 1;
									term_cursor = 80 * (val1 - 1) + (val2 - 1);
								}
								break;
							}
							case 7:
							{
								int val1 = (int)(term_keycode[term_sequence-5]-'0') * 10 + (int)(term_keycode[term_sequence-4]-'0');
								if (val1 <= 0) val1 = 1;
								int val2 = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								if (val2 <= 0) val2 = 1;
								term_cursor = 80 * (val1 - 1) + (val2 - 1);
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x4A) // J for erase screen
					{
						switch (term_sequence)
						{
							case 2:
							{
								for (int i=term_cursor; i<1920; i++)
								{
									term_memory[i] = 0x20; // space
								}
								term_memory[(term_scroll*80+term_cursor)%1920] = 0xA0; // inverted space
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								if (val == 0)
								{
									for (int i=term_cursor; i<1920; i++)
									{
										term_memory[i] = 0x20; // space
									}
								}
								else if (val == 1)
								{
									for (int i=0; i<=term_cursor; i++)
									{
										term_memory[i] = 0x20; // space
									}
								}
								else if (val == 2)
								{
									for (int i=0; i<1920; i++)
									{
										term_memory[i] = 0x20; // space
									}
								}
								else if (val == 3)
								{
									for (int i=0; i<3840; i++)
									{
										term_memory[i] = 0x20; // space
									}
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x4B) // K for erase line
					{
						switch (term_sequence)
						{
							case 2:
							{
								int val = term_cursor;
								while ((val % 80) != 0)
								{
									term_memory[val] = 0x20; // space
									val++;
								}
								break;
							}
							case 3:
							{
								int val1 = (int)(term_keycode[term_sequence-1]-'0');
								if (val1 == 0)
								{
									int val2 = term_cursor;
									while ((val2 % 80) != 0)
									{
										term_memory[val2] = 0x20; // space
										val2++;
									}
								}
								else if (val1 == 1)
								{
									int val2 = term_cursor - (term_cursor % 80);
									while (val2 != term_cursor)
									{
										term_memory[val2] = 0x20; // space
										val2++;
									}
									term_memory[(term_scroll*80+term_cursor)%1920] = 0x20;
								}
								else if (val1 == 2)
								{
									int val2 = term_cursor - (term_cursor % 80);
									while ((val2 % 80) != 0)
									{
										term_memory[val2] = 0x20; // space
										val2++;
									}
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x53) // S for scroll up
					{
						switch (term_sequence)
						{
							case 2:
							{
								for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
								term_scroll++;
								if (term_scroll >= 24) term_scroll -= 24;
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
									term_scroll++;
									if (term_scroll >= 24) term_scroll -= 24;
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
									term_scroll++;
									if (term_scroll >= 24) term_scroll -= 24;
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}
					else if (term_keycode[term_sequence] == 0x54) // T for scroll down
					{
						switch (term_sequence)
						{
							case 2:
							{
								if (term_scroll == 0) term_scroll = 23;
								else term_scroll--;
								for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
								break;
							}
							case 3:
							{
								int val = (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if (term_scroll == 0) term_scroll = 23;
									else term_scroll--;
									for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
								}
								break;
							}
							case 4:
							{
								int val = (int)(term_keycode[term_sequence-2]-'0') * 10 + (int)(term_keycode[term_sequence-1]-'0');
								for (int i=0; i<val; i++)
								{
									if (term_scroll == 0) term_scroll = 23;
									else term_scroll--;
									for (int i=0; i<80; i++) term_memory[term_scroll * 80 + i] = 0x20; // space
								}
								break;
							}
							default:
							{
								break;
							}
						}
					}

					term_command = 0;
					term_sequence = 0;
				}
				else
				{
					term_sequence++;
					if (term_sequence >= 8)
					{
						term_command = 0;
						term_sequence = 0;
					}
				}
			}
			else if (term_sequence >= 2 && term_command == 2) // personal term_commands
			{
				if (term_sequence == 2 && term_keycode[2] == 'H')
				{
					text_string(0, 0,  "ANSI Commands:\\");
					text_string(0, 1,  "  ESC[xA      = Cursor Up\\");
					text_string(0, 2,  "  ESC[xB      = Cursor Down\\");
					text_string(0, 3,  "  ESC[xC      = Cursor Forward\\");
					text_string(0, 4,  "  ESC[xD      = Cursor Back\\");
					text_string(0, 5,  "  ESC[xE      = Cursor Next Line\\");
					text_string(0, 6,  "  ESC[xF      = Cursor Previous Line\\");
					text_string(0, 7,  "  ESC[xG      = Cursor Horizontal Absolute\\");
					text_string(0, 8,  "  ESC[y;xH    = Cursor Position\\");
					text_string(0, 9,  "  ESC[xJ      = Erase in Display\\");
					text_string(0, 10, "  ESC[xK      = Erase in Line\\");
					text_string(0, 11, "  ESC[xS      = Scroll Up\\");
					text_string(0, 12, "  ESC[xT      = Scroll Down\\");
					text_string(0, 13, "Special Commands:\\");
					text_string(0, 14, "  ESC;H       = Help Menu\\");
					text_string(0, 15, "  ESC;T       = Terminal Mode\\");
					text_string(0, 16, "  ESC;C       = Color Mode\\");
					text_string(0, 17, "  ESC;Axx     = Set Memory Start Address\\");
					text_string(0, 18, "  ESC;Dxx...  = Set Memory Data Length followed by Data\\");
					text_string(0, 19, "Memory Addresses:\\");
					text_string(0, 20, "  $4000-$47FF = Terminal Data\\");
					text_string(0, 21, "  $4800-$4FFF = Terminal Mappings\\");
					text_string(0, 22, "  $5000-$DFFF = Color Data\\");

					term_cursor = 1840;

					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 2 && term_keycode[2] == 'T')
				{
					term_mode = 0;

					// turn off colors
					TRISB = (TRISB & 0x00FF) | 0x7F00;
					PORTB = 0x0000;

					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 2 && term_keycode[2] == 'C')
				{
					term_mode = 1;

					// turn on colors
					TRISB = (TRISB & 0x00FF) | 0x8000;
					PORTB = 0x0000;

					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 4 && term_keycode[2] == 'A')
				{
					term_dma_address = (unsigned int)(term_keycode[term_sequence-1]) * 256 + (unsigned int)(term_keycode[term_sequence]);
					
					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 4 && term_keycode[2] == 'D')
				{
					term_dma_data = (unsigned int)(term_keycode[term_sequence-1]) * 256 + (unsigned int)(term_keycode[term_sequence]);
					
					term_command = 0;
					term_sequence = 0;
				}
				else
				{
					term_sequence++;
					if (term_sequence >= 8)
					{
						term_command = 0;
						term_sequence = 0;
					}
				}
			}

			if (term_setting_cursor > 0 && term_cursor < 1920) 
			{
				term_memory[(term_scroll*80+term_cursor)%1920] = ((term_memory[(term_scroll*80+term_cursor)%1920] + 0x80) & 0x00FF);
			}
		}
	}
};

void __attribute__((address(0x010000))) entry()
{
	setup();
	run();
};

#define CONVERT(h,l) \
	((unsigned char)(h >= 'A' && h <= 'F' ? ((h - 'A' + 10) << 4) : \
	(h >= 'a' && h <= 'f' ? ((h - 'a' + 10) << 4) : \
	(h >= '0' && h <= '9' ? ((h - '0') << 4) : 0x00))) | \
	(unsigned char)(l >= 'A' && l <= 'F' ? ((l - 'A' + 10)) : \
	(l >= 'a' && l <= 'f' ? ((l - 'a' + 10)) : \
	(l >= '0' && l <= '9' ? ((l - '0')) : 0x00))))

/*	
static inline unsigned char CONVERT(char h, char l)
{
	unsigned char v = 0x00;

	if (h >= 'A' && h <= 'F') v += ((h - 'A' + 10) << 4);
	else if (h >= 'a' && h <= 'f') v += ((h - 'a' + 10) << 4);
	else if (h >= '0' && h <= '9') v += ((h - '0') << 4);

	if (l >= 'A' && l <= 'F') v += ((l - 'A' + 10));
	else if (l >= 'a' && l <= 'f') v += ((l - 'a' + 10));
	else if (l >= '0' && l <= '9') v += ((l - '0'));

	return v;
};
*/

int __attribute__((address(0x000200))) main()
{
	// set Port A to defaults
	ANSELA = 0x0000;
	LATA = 0x0000;
	TRISA = 0xFFFF;
	CNPUA = 0x0000;
	CNPDA = 0x0000;

	// set Port B to defaults
	ANSELB = 0x0000;
	LATB = 0x0000;
	TRISB = 0xFFFF;
	CNPUB = 0x0000;
	CNPDB = 0x0000;

	// disable watchdog timer
	PTGCONbits.PTGWDT = 0x0;

	// Configure OSC tuner, PLL prescaler, PLL postscaler, PLL divisor
	// This makes it run at 65 MIPS
	OSCTUN = 0x0021; // TUN = -31
	PLLFBD = 0x00B1; // PLLDIV = 177
	CLKDIVbits.PLLPRE = 0x3; // PLLPRE = 3
	CLKDIVbits.PLLPOST = 0x0; // PLLPOST = 0

	CNPUB = 0x0010; // pull-up on RB4

	for (unsigned int i=0; i<32768; i++) { for (unsigned int j=0; j<64; j++) { } } // delay

	// check if RB4 is grounded, and if so, run firmware update
	int update = (PORTB & 0x0010);

	CNPUB = 0x0000; // disable pull-up on RB4

	// firmware update uses UART at 9600 baud, will rewrite all but main() essentially
	if (update == 0x0000)
	{
		// output on RB6 (RP38)
		TRISB = 0xFFBF;
		LATB = 0x0000;

		// sets UART1 to appropriate pins
		RPINR18 = 0x0025; // UART1-RX on RP37
		RPOR2 = (RPOR2 & 0xFF00) | 0x0001; // UART1-TX on RP38

		// set up UART1
		IEC0bits.U1TXIE = 0; // disable transmit interrupts
		IEC0bits.U1RXIE = 0; // disable receive interrupts
		IFS0bits.U1TXIF = 0; // clear flag
		IFS0bits.U1RXIF = 0; // clear flag
		IPC2bits.U1RXIP = 0x01; // lowest interrupt priority 
		IPC3bits.U1TXIP = 0x01; // lowest interrupt priority 
		U1MODE = 0x0000; // disable and clear everything, 8-bit, no parity, 1 stop bit, etc.
		U1STA = 0x0000; // clear all flags
		U1BRG = (65000000 / 9600) / 16 - 1; // baud rate divisor = (65000000 / 9600) / 16 - 1 = 422.177
		U1MODEbits.UARTEN = 1; // enable uart module (needs to be on before UTXEN is enabled)
		U1STAbits.UTXEN = 1; // enable transmit
		//IEC0bits.U1TXIE = 1; // enable transmit interrupts
		//IEC0bits.U1RXIE = 1; // enable receive interrupts

		uint16_t save;

		for (int i=1; i<=4; i++) // 0x010000 through 0x04FFFF
		{
			for (unsigned int j=0; j<64; j++)
			{
				// erase page operation
				NVMCON = 0x4003; // Set ERASE, WREN and configure NVMOP for page erase
				NVMADR = (unsigned int)(j << 10); // lower address
				NVMADRU = (unsigned int)i; // upper address
				save = INTCON2; // Save interrupt status
				__builtin_disable_interrupts(); // Disable interrupts for NVM unlock
				__builtin_write_NVM(); // Start write cycle (sends 0x55 and 0xAA to NVMKEY and sets NVMCON.WR)
				while(NVMCONbits.WR == 1) { } // Wait for write cycle to finish
				INTCON2 = save; // Restore interrupt status
			}
		}

		U1TXREG = '?'; // dummy transfer

		unsigned char buffer;
		unsigned int word;
		unsigned char nibble[2];

		unsigned char length;
		unsigned int address;
		unsigned char type;

		unsigned int ext_address;
		unsigned int prog_address;
		unsigned char prog_data[16];
		unsigned char prog_pos = 0;

		unsigned char loop = 1;

		while (loop > 0)
		{
			while (U1STAbits.URXDA == 0) { }
			buffer = U1RXREG;

			// code
			if (buffer == ':')
			{
				// length
				while (U1STAbits.URXDA == 0) { }
				nibble[0] = U1RXREG;
				while (U1STAbits.URXDA == 0) { }
				nibble[1] = U1RXREG;
				length = CONVERT(nibble[0], nibble[1]);

				if (length > 0)
				{
					// address
					while (U1STAbits.URXDA == 0) { }
					nibble[0] = U1RXREG;
					while (U1STAbits.URXDA == 0) { }
					nibble[1] = U1RXREG;
					address = (unsigned int)(CONVERT(nibble[0], nibble[1]) << 8);
					while (U1STAbits.URXDA == 0) { }
					nibble[0] = U1RXREG;
					while (U1STAbits.URXDA == 0) { }
					nibble[1] = U1RXREG;
					address += (unsigned int)(CONVERT(nibble[0], nibble[1]));

					// type
					while (U1STAbits.URXDA == 0) { }
					nibble[0] = U1RXREG;
					while (U1STAbits.URXDA == 0) { }
					nibble[1] = U1RXREG;
					type = CONVERT(nibble[0], nibble[1]);

					if (type == 0x00) // data
					{
						if (prog_pos == 0) prog_address = address;

						for (int i=0; i<(length<<1); i++)
						{
							while (U1STAbits.URXDA == 0) { }
							prog_data[prog_pos] = U1RXREG;
							prog_pos++;
							if (prog_pos >= 16)
							{
								if ((ext_address >> 1) >= 0x0001 && (ext_address >> 1) <= 0x0004) // keeps from overwriting important code
								{
									// write page operation
									NVMCON = 0x4001; // Set WREN and word program mode
									TBLPAG = 0xFA; // Must be 0xFA for some reason!
									NVMADR = (prog_address >> 1); // lower address
									NVMADRU = (ext_address >> 1); // upper address
									word = ((unsigned int)CONVERT(prog_data[2], prog_data[3]) << 8);
									word += (unsigned int)(CONVERT(prog_data[0], prog_data[1]));
									__builtin_tblwtl(0x0, word);
									word = ((unsigned int)CONVERT(prog_data[6], prog_data[7]) << 8);
									word += (unsigned int)(CONVERT(prog_data[4], prog_data[5]));
									__builtin_tblwth(0x0, word);
									word = ((unsigned int)CONVERT(prog_data[10], prog_data[11]) << 8);
									word += (unsigned int)(CONVERT(prog_data[8], prog_data[9]));
									__builtin_tblwtl(0x2, word);
									word = ((unsigned int)CONVERT(prog_data[14], prog_data[15]) << 8);
									word += (unsigned int)(CONVERT(prog_data[12], prog_data[13]));
									__builtin_tblwth(0x2, word);
									save = INTCON2;
									__builtin_disable_interrupts(); // Disable interrupts for NVM unlock sequence
									__builtin_write_NVM(); // initiate write
									while(NVMCONbits.WR == 1);
									INTCON2 = save;
								}

								prog_pos = 0;
								prog_address = address + (i>>1) + 1;
							}
						}

						// checksum
						while (U1STAbits.URXDA == 0) { }
						nibble[0] = U1RXREG;
						while (U1STAbits.URXDA == 0) { }
						nibble[1] = U1RXREG;
					}
					else if (type == 0x04) // ext_address
					{
						if (prog_pos > 0)
						{
							for (int i=prog_pos; i<16; i++) prog_data[i] = 'F';

							if ((ext_address >> 1) >= 0x0001 && (ext_address >> 1) <= 0x0004) // keeps from overwriting important code
							{
								// write page operation
								NVMCON = 0x4001; // Set WREN and word program mode
								TBLPAG = 0xFA; // Must be 0xFA for some reason!
								NVMADR = (prog_address >> 1); // lower address
								NVMADRU = (ext_address >> 1); // upper address
								word = ((unsigned int)CONVERT(prog_data[2], prog_data[3]) << 8);
								word += (unsigned int)(CONVERT(prog_data[0], prog_data[1]));
								__builtin_tblwtl(0x0, word);
								word = ((unsigned int)CONVERT(prog_data[6], prog_data[7]) << 8);
								word += (unsigned int)(CONVERT(prog_data[4], prog_data[5]));
								__builtin_tblwth(0x0, word);
								word = ((unsigned int)CONVERT(prog_data[10], prog_data[11]) << 8);
								word += (unsigned int)(CONVERT(prog_data[8], prog_data[9]));
								__builtin_tblwtl(0x2, word);
								word = ((unsigned int)CONVERT(prog_data[14], prog_data[15]) << 8);
								word += (unsigned int)(CONVERT(prog_data[12], prog_data[13]));
								__builtin_tblwth(0x2, word);
								save = INTCON2;
								__builtin_disable_interrupts(); // Disable interrupts for NVM unlock sequence
								__builtin_write_NVM(); // initiate write
								while(NVMCONbits.WR == 1);
								INTCON2 = save;
							}

							prog_pos = 0;
						}
			
						// ext_address
						while (U1STAbits.URXDA == 0) { }
						nibble[0] = U1RXREG;
						while (U1STAbits.URXDA == 0) { }
						nibble[1] = U1RXREG;
						ext_address = (unsigned int)(CONVERT(nibble[0], nibble[1]) << 8);
						while (U1STAbits.URXDA == 0) { }
						nibble[0] = U1RXREG;
						while (U1STAbits.URXDA == 0) { }
						nibble[1] = U1RXREG;
						ext_address += (unsigned int)(CONVERT(nibble[0], nibble[1]));

						// checksum
						while (U1STAbits.URXDA == 0) { }
						nibble[0] = U1RXREG;
						while (U1STAbits.URXDA == 0) { }
						nibble[1] = U1RXREG;
					}
				}
				else
				{
					loop = 0;
			
					if (prog_pos > 0)
					{
						for (int i=prog_pos; i<16; i++) prog_data[i] = 'F';

						if ((ext_address >> 1) >= 0x0001 && (ext_address >> 1) <= 0x0004) // keeps from overwriting important code
						{
							// write page operation
							NVMCON = 0x4001; // Set WREN and word program mode
							TBLPAG = 0xFA; // Must be 0xFA for some reason!
							NVMADR = (prog_address >> 1); // lower address
							NVMADRU = (ext_address >> 1); // upper address
							word = ((unsigned int)CONVERT(prog_data[2], prog_data[3]) << 8);
							word += (unsigned int)(CONVERT(prog_data[0], prog_data[1]));
							__builtin_tblwtl(0x0, word);
							word = ((unsigned int)CONVERT(prog_data[6], prog_data[7]) << 8);
							word += (unsigned int)(CONVERT(prog_data[4], prog_data[5]));
							__builtin_tblwth(0x0, word);
							word = ((unsigned int)CONVERT(prog_data[10], prog_data[11]) << 8);
							word += (unsigned int)(CONVERT(prog_data[8], prog_data[9]));
							__builtin_tblwtl(0x2, word);
							word = ((unsigned int)CONVERT(prog_data[14], prog_data[15]) << 8);
							word += (unsigned int)(CONVERT(prog_data[12], prog_data[13]));
							__builtin_tblwth(0x2, word);
							save = INTCON2;
							__builtin_disable_interrupts(); // Disable interrupts for NVM unlock sequence
							__builtin_write_NVM(); // initiate write
							while(NVMCONbits.WR == 1);
							INTCON2 = save;
						}

						prog_pos = 0;
					}
				}
			}
		}

		// reset system
		asm("RESET");
	}

	entry();	

	return 1;
}
