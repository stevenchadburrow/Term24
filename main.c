
// Term24 - VGA Terminal Module using the PIC24.
// Supports: UART, SPI, I2C, and PS/2 Keyboard.
// Includes color mode for 128x128 resolution at 128-colors, with scrolling or layers.

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

// PIC24EP512GP202 Configuration Bit Settings

// 'C' source line config statements

// FICD
#pragma config ICS = PGD3               // ICD Communication Channel Select bits (Communicate on PGEC3 and PGED3)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = ON             // Alternate I2C2 pins (I2C2 mapped to ASDA2/ASCL2 pins)
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
volatile unsigned char __attribute__((address(0x3800))) term_game_field[512]; // only uses 200 bytes plus borders
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
volatile unsigned int __attribute__((address(0x3AB0))) term_bottom; // only used in color mode, 0x02FF by default
volatile unsigned int __attribute__((address(0x3AB2))) term_parallel;
volatile unsigned int __attribute__((address(0x3AB4))) term_last_channel;
volatile unsigned int __attribute__((address(0x3AB6))) term_last_address;
volatile unsigned int __attribute__((address(0x3AB8))) term_ps2_release;
volatile unsigned int __attribute__((address(0x3ABA))) term_ps2_extended;
volatile unsigned int __attribute__((address(0x3ABC))) term_ps2_shift;
volatile unsigned int __attribute__((address(0x3ABE))) term_ps2_capslock;
volatile unsigned int __attribute__((address(0x3AC0))) term_ps2_synced;
volatile unsigned int __attribute__((address(0x3AC2))) term_sequence;
volatile unsigned int __attribute__((address(0x3AC4))) term_command;
volatile unsigned int __attribute__((address(0x3AC6))) term_print;
volatile unsigned int __attribute__((address(0x3AC8))) term_dma_address;
volatile unsigned int __attribute__((address(0x3ACA))) term_dma_data; 
volatile unsigned int __attribute__((address(0x3ACC))) term_position;
volatile unsigned int __attribute__((address(0x3ACE))) term_setting_cursor;
volatile unsigned int __attribute__((address(0x3AD0))) term_setting_echo;
volatile unsigned char __attribute__((address(0x3AD2))) term_keycode[8];
volatile unsigned int __attribute__((address(0x3ADA))) term_game_piece_current;
volatile unsigned int __attribute__((address(0x3ADC))) term_game_piece_next;
volatile signed int __attribute__((address(0x3ADE))) term_game_piece_x;
volatile signed int __attribute__((address(0x3AE0))) term_game_piece_y;
volatile signed int __attribute__((address(0x3AE2))) term_game_piece_rot;
volatile unsigned int __attribute__((address(0x3AE4))) term_game_button_current;
volatile unsigned int __attribute__((address(0x3AE6))) term_game_button_previous;
volatile unsigned int __attribute__((address(0x3AE8))) term_game_button_held;
volatile unsigned int __attribute__((address(0x3AEA))) term_game_delay_low;
volatile unsigned int __attribute__((address(0x3AEC))) term_game_delay_high;
volatile unsigned int __attribute__((address(0x3AEE))) term_game_counter_low;
volatile unsigned int __attribute__((address(0x3AF0))) term_game_counter_high;
volatile unsigned int __attribute__((address(0x3AF2))) term_game_points;
volatile unsigned int __attribute__((address(0x3AF4))) term_game_tally;
volatile unsigned int __attribute__((address(0x3AF6))) term_game_seed;
// unused memory here
volatile unsigned char __attribute__((address(0x3B00))) term_ps2_conversion[256];
volatile unsigned int __attribute__((address(0x3C00))) term_array[256];
// unused memory here (generally used by heap, 512 bytes)
volatile unsigned char __attribute__((address(0x4000))) term_memory[2048];
volatile unsigned char __attribute__((address(0x4800))) term_map[2048];
volatile unsigned char __attribute__((address(0x5000))) term_color[12288];
volatile __eds__ unsigned char __attribute__((address(0x8000), eds)) term_color_eds[20480];

unsigned char __attribute__((section("usercode"))) term_read(unsigned int a)
{
	if (a >= 0x4000 && a < 0x4800)
	{
		return term_memory[a-0x4000];
	}
	else if (a >= 0x4800 && a < 0x5000)
	{
		return term_map[a-0x2800];
	}
	else if (a >= 0x5000 && a < 0x8000)
	{
		return term_color[a-0x5000];
	}
	else if (a >= 0x8000 && a < 0xD000)
	{
		return term_color_eds[a-0x8000];
	}

	return 0x00;
};

void __attribute__((section("usercode"))) term_write(unsigned int a, unsigned char d)
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
void __attribute__((interrupt, auto_psv, address(0x010600))) _T1Interrupt(void)
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

const __prog__ char __attribute__((space(prog), section("usercode"))) text_menu_0[32] = {
	"Term24    ESC;H for Help      \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_menu_1[32] = {
	"UART 9600-8-N-1               \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_menu_2[48] = {
	"PS/2 Press = or send 0x55 to sync ?\??\?-8-O-1  \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_menu_3[32] = {
	"SPI CLK/MOSI                  \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_menu_4[32] = {
	"PARALLEL CLK/PORT             \\" };

const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_0[32] = {
	"ANSI/Special Commands:        \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_1[32] = {
	" ESC[xA     = Cursor Up       \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_2[32] = {
	" ESC[xB     = Cursor Down     \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_3[32] = {
	" ESC[xC     = Cursor Forward  \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_4[32] = {
	" ESC[xD     = Cursor Back     \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_5[32] = {
	" ESC[xE     = Cursor Next Line\\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_6[32] = {
	" ESC[xF     = Cursor Prev Line\\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_7[32] = {
	" ESC[xG     = Cursor Horz Abs \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_8[32] = {
	" ESC[y;xH   = Cursor Position \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_9[32] = {
	" ESC[xJ     = Erase in Display\\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_10[32] = {
	" ESC[xK     = Erase in Line   \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_11[32] = {
	" ESC[xS     = Scroll Up       \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_12[32] = {
	" ESC[xT     = Scroll Down     \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_13[32] = {
	" ESC;T      = Terminal Mode   \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_14[32] = {
	" ESC;xC     = Color Mode      \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_15[32] = {
	" ESC;xE     = Echo On/Off     \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_16[32] = {
	" ESC;Ahh    = Memory Address  \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_17[32] = {
	" ESC;R      = Read then Inc   \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_18[48] = {
	" ESC;Whh... = Write Length followed by Values \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_19[32] = {
	"Memory Addresses:             \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_20[32] = {
	" $4000-47FF = Terminal Data   \\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_21[32] = {
	" $4800-4FFF = Terminal Mapping\\" };
const __prog__ char __attribute__((space(prog), section("usercode"))) text_help_22[32] = {
	" $5000-DFFF = Color Data      \\" };


void __attribute__((section("usercode"))) color_character(int x, int y, unsigned char c)
{
	if (c < 0x20 || c >= 0x80) return;

	unsigned char v;

	for (int i=0; i<8; i++)
	{
		v = text_map[(c - 0x20) * 8 + i];

		for (int j=0; j<6; j++)
		{
			if ((v & 0x80) == 0x80) term_color[(y+i)*128+(x+j)] = 0x7F;
			else term_color[(y+i)*128+(x+j)] = 0x00;

			v <<= 1;
		}
	}
};

void __attribute__((section("usercode"))) text_character(int x, int y, unsigned char c)
{
	term_memory[y*80+x] = (unsigned int)c;
};

void __attribute__((section("usercode"))) text_string(int x, int y, const __prog__ char *s)
{
	for (int i=0; i<80; i++)
	{
		if (s[i] == '\\') return;

		text_character(x+i, y, s[i]);
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

void __attribute__((interrupt, auto_psv, address(0x010100))) _U1RXInterrupt(void)
{
	IFS0bits.U1RXIF = 0; // Clear RX Interrupt flag
};

void __attribute__((interrupt, auto_psv, address(0x010200))) _U1TXInterrupt(void)
{
	IFS0bits.U1TXIF = 0; // Clear TX Interrupt flag
};

void __attribute__((interrupt, auto_psv, address(0x010300))) _SPI2Interrupt(void)
{
	IFS2bits.SPI2IF = 0; // clear flag
}

void __attribute__((interrupt, auto_psv, address(0x010400))) _INT1Interrupt(void)
{
	IFS1bits.INT1IF = 0; // clear flag

	unsigned int pa = (PORTA & 0x000F); // get port values
	unsigned int pb = ((PORTB & 0x003C) << 2);
	
	term_last_channel = 0x0000; // indicate channel use
	term_last_address += 2; // increment channel address
	
	term_array[term_last_address] = (pb | pa); // store in array
}

void __attribute__((interrupt, auto_psv, address(0x010500))) _DMA0Interrupt(void)
{	
	IFS0bits.DMA0IF = 0; // clear flag
};

const __prog__ char __attribute__((space(prog), section("usercode"))) game_pieces[4*4*4*7] = {
	// I
	' ','0',' ',' ',
	' ','0',' ',' ',
	' ','0',' ',' ',
	' ','0',' ',' ',

	' ',' ',' ',' ',
	'0','0','0','0',
	' ',' ',' ',' ',
	' ',' ',' ',' ',

	' ','0',' ',' ',
	' ','0',' ',' ',
	' ','0',' ',' ',
	' ','0',' ',' ',

	' ',' ',' ',' ',
	'0','0','0','0',
	' ',' ',' ',' ',
	' ',' ',' ',' ',

	// J
	' ',' ','0',' ',
	' ',' ','0',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',

	' ',' ',' ',' ',
	'0','0','0',' ',
	' ',' ','0',' ',
	' ',' ',' ',' ',

	' ',' ',' ',' ',
	' ','0','0',' ',
	' ','0',' ',' ',
	' ','0',' ',' ',

	' ',' ',' ',' ',
	' ','0',' ',' ',
	' ','0','0','0',
	' ',' ',' ',' ',

	// L
	' ','0',' ',' ',
	' ','0',' ',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',

	' ',' ',' ',' ',
	' ',' ','0',' ',
	'0','0','0',' ',
	' ',' ',' ',' ',

	' ',' ',' ',' ',
	' ','0','0',' ',
	' ',' ','0',' ',
	' ',' ','0',' ',

	' ',' ',' ',' ',
	' ','0','0','0',
	' ','0',' ',' ',
	' ',' ',' ',' ',

	// O
	' ','0','0',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',
	' ',' ',' ',' ',

	' ','0','0',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',
	' ',' ',' ',' ',

	' ','0','0',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',
	' ',' ',' ',' ',

	' ','0','0',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',
	' ',' ',' ',' ',

	// S
	' ',' ',' ',' ',
	' ','0','0',' ',
	'0','0',' ',' ',
	' ',' ',' ',' ',

	' ','0',' ',' ',
	' ','0','0',' ',
	' ',' ','0',' ',
	' ',' ',' ',' ',

	' ',' ',' ',' ',
	' ','0','0',' ',
	'0','0',' ',' ',
	' ',' ',' ',' ',

	' ','0',' ',' ',
	' ','0','0',' ',
	' ',' ','0',' ',
	' ',' ',' ',' ',

	// Z
	' ',' ',' ',' ',
	'0','0',' ',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',

	' ',' ','0',' ',
	' ','0','0',' ',
	' ','0',' ',' ',
	' ',' ',' ',' ',

	' ',' ',' ',' ',
	'0','0',' ',' ',
	' ','0','0',' ',
	' ',' ',' ',' ',

	' ',' ','0',' ',
	' ','0','0',' ',
	' ','0',' ',' ',
	' ',' ',' ',' ',

	// T
	' ','0',' ',' ',
	'0','0','0',' ',
	' ',' ',' ',' ',
	' ',' ',' ',' ',

	' ','0',' ',' ',
	'0','0',' ',' ',
	' ','0',' ',' ',
	' ',' ',' ',' ',

	' ',' ',' ',' ',
	'0','0','0',' ',
	' ','0',' ',' ',
	' ',' ',' ',' ',

	' ','0',' ',' ',
	' ','0','0',' ',
	' ','0',' ',' ',
	' ',' ',' ',' ',
};

const __prog__ char __attribute__((space(prog), section("usercode"))) game_bags[7*16] = {
	0, 1, 2, 3, 4, 5, 6,
	6, 4, 3, 5, 0, 1, 2,
	3, 4, 1, 0, 6, 5, 2,
	1, 0, 6, 3, 4, 2, 5,

	5, 2, 1, 3, 4, 0, 6,
	0, 2, 1, 6, 3, 5, 4,
	5, 4, 6, 0, 3, 1, 2,
	4, 5, 2, 6, 1, 0, 3,

	4, 2, 0, 1, 6, 3, 5,
	3, 0, 6, 2, 1, 4, 5,
	1, 0, 3, 4, 5, 6, 2,
	2, 0, 4, 5, 1, 6, 3,

	6, 4, 0, 5, 2, 1, 3,
	1, 2, 4, 5, 6, 3, 0,
	6, 2, 1, 3, 0, 5, 4,
	4, 1, 2, 3, 0, 6, 5,
};

void __attribute__((section("usercode"))) play()
{
	term_game_seed = 0;

	CNPUA = 0x000F; // pull-up on RA3 to RA0
	CNPUB = (CNPUB | 0x000C); // pull-ups on RB3 and RB2

	if (term_mode == 0)
	{
		// turn off colors
		TRISB = (TRISB & 0x00FF) | 0x7F00;
		PORTB = 0x0000;

		term_bottom = 600-1; // usually 768-1 by default

		for (int i=0; i<2048; i++) term_memory[i] = ' ';
	}
	else if (term_mode == 1)
	{
		// turn on colors
		TRISB = (TRISB & 0x00FF) | 0x8000;
		PORTB = 0x0000;

		term_orientation = 2; // layered
		term_scroll = 0; // no scrolling
		term_bottom = 576-1; // usually 768-1 by default

		for (int i=0; i<12288; i++)
		{
			term_color[i] = 0x00;
		}

		for (int i=0; i<20480; i++)
		{
			term_color_eds[i] = 0x00;
		}
	}

	for (int y=0; y<24; y++)
	{
		for (int x=0; x<16; x++)
		{
			if (y == 0 || y > 20) term_game_field[y*16+x] = '*';
			else
			{
				if (x < 3 || x > 12) term_game_field[y*16+x] = '*';
				else term_game_field[y*16+x] = ' ';
			}
		}
	}

	term_game_piece_current = 0; // randomize
	term_game_piece_next = 1; // randomize
	term_game_piece_x = 6; // starts at 6
	term_game_piece_y = 1; // starts at 1
	term_game_piece_rot = 0;
	term_game_button_current = 0x003F;
	term_game_button_previous = 0x003F;
	term_game_button_held = 60;
	term_game_delay_low = 60;
	term_game_delay_high = 0;
	term_game_counter_low = term_game_delay_low;
	term_game_counter_high = term_game_delay_high;
	term_game_points = 0;
	term_game_tally = 0;

	unsigned char bag[7];
	unsigned char pos = 2; // must start with 2
	unsigned int loc = 0;
	unsigned int redraw = 1; // must start with 1
	unsigned int descend = 0;
	unsigned int check = 0;
	unsigned int first = 0;
	unsigned int lines = 0;
	signed int prev_x = 6;
	signed int prev_y = 1;
	signed int prev_rot = 0;
	unsigned char color_value = 0x7F;
	unsigned int hold_time = 6; // adjust accordingly
	unsigned int text_shift = 25; // adjust accordingly
	unsigned int color_shift = 15; // adjust accordingly
	unsigned int color_scale = 3; // adjust accordingly

	// for PS/2 keyboard
	unsigned int keyboard_state = 0x003F;

	term_game_button_held = hold_time;

	for (int i=0; i<7; i++) bag[i] = (unsigned char)(i);

	if (term_mode == 0)
	{
		text_character(18+text_shift, 1, 'P');
		text_character(19+text_shift, 1, 'T');
		text_character(20+text_shift, 1, 'S');
		text_character(21+text_shift, 1, ':');
		text_character(22+text_shift, 1, ' ');
		text_character(23+text_shift, 1, '0');
		text_character(24+text_shift, 1, '0');
		text_character(25+text_shift, 1, '0');
		text_character(26+text_shift, 1, '0');

		text_character(18+text_shift, 3, 'N');
		text_character(19+text_shift, 3, 'E');
		text_character(20+text_shift, 3, 'X');
		text_character(21+text_shift, 3, 'T');
		text_character(22+text_shift, 3, ':');
		text_character(23+text_shift, 3, ' ');

		switch (term_game_piece_next)
		{
			case 0: { text_character(24+text_shift, 3, 'I'); break; }
			case 1: { text_character(24+text_shift, 3, 'J'); break; }
			case 2: { text_character(24+text_shift, 3, 'L'); break; }
			case 3: { text_character(24+text_shift, 3, 'O'); break; }
			case 4: { text_character(24+text_shift, 3, 'S'); break; }
			case 5: { text_character(24+text_shift, 3, 'Z'); break; }
			case 6: { text_character(24+text_shift, 3, 'T'); break; }
			case 7: { text_character(24+text_shift, 3, '?'); break; }
			default: { text_character(24+text_shift, 3, '?'); break; }
		}
	}
	else if (term_mode == 1)
	{
		color_character(56+color_shift, 1, 'P');
		color_character(62+color_shift, 1, 'T');
		color_character(68+color_shift, 1, 'S');
		color_character(74+color_shift, 1, ':');
		color_character(80+color_shift, 1, ' ');
		color_character(86+color_shift, 1, '0');
		color_character(92+color_shift, 1, '0');
		color_character(98+color_shift, 1, '0');
		color_character(104+color_shift, 1, '0');

		color_character(56+color_shift, 9, 'N');
		color_character(62+color_shift, 9, 'E');
		color_character(68+color_shift, 9, 'X');
		color_character(74+color_shift, 9, 'T');
		color_character(80+color_shift, 9, ':');
		color_character(86+color_shift, 9, ' ');

		switch (term_game_piece_next)
		{
			case 0: { color_character(92+color_shift, 9, 'I'); break; }
			case 1: { color_character(92+color_shift, 9, 'J'); break; }
			case 2: { color_character(92+color_shift, 9, 'L'); break; }
			case 3: { color_character(92+color_shift, 9, 'O'); break; }
			case 4: { color_character(92+color_shift, 9, 'S'); break; }
			case 5: { color_character(92+color_shift, 9, 'Z'); break; }
			case 6: { color_character(92+color_shift, 9, 'T'); break; }
			case 7: { color_character(92+color_shift, 9, '?'); break; }
			default: { color_character(92+color_shift, 9, '?'); break; }
		}
	}

	while (1)
	{
		term_game_seed++; // psuedo-random

		prev_x = term_game_piece_x;
		prev_y = term_game_piece_y;
		prev_rot = term_game_piece_rot;

		if (term_game_counter_low > 0)
		{
			term_game_counter_low--;
		}
		else
		{
			if (term_game_counter_high > 0)
			{
				term_game_counter_low = 65535;
				term_game_counter_high--;
			}
			else
			{
				term_game_counter_low = term_game_delay_low;
				term_game_counter_high = term_game_delay_high;

				if (term_game_piece_y < 21)
				{
					term_game_piece_y++;
				}

				descend = 1;
				check = 1;
				redraw = 1;
			}
		}	

		if (descend == 0)
		{
			// read PS/2 keyboard
			if (term_ps2_synced > 0)	
			{
				term_last_channel = (DMALCA & 0x000F);
				term_last_address = ((DSADRL + 2) & 0x01FF);

				if (term_last_channel == 0x0000)
				{
					if ((((term_position) & 0x00FF) << 1) != term_last_address)
					{
						if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0xE0) // term_ps2_extended
						{
							term_ps2_extended = 1;
						}
						else if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0xF0) // term_ps2_release
						{
							term_ps2_release = 1;
						}
						else
						{
							if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x1D) // W = up
							{
								if (term_ps2_release > 0) keyboard_state = (keyboard_state | 0x0008);
								else keyboard_state = (keyboard_state & 0x0037);
							}

							if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x1B) // S = down
							{
								if (term_ps2_release > 0) keyboard_state = (keyboard_state | 0x0004);
								else keyboard_state = (keyboard_state & 0x003B);
							}

							if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x1C) // A = left
							{
								if (term_ps2_release > 0) keyboard_state = (keyboard_state | 0x0002);
								else keyboard_state = (keyboard_state & 0x003D);
							}

							if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x23) // D = right
							{
								if (term_ps2_release > 0) keyboard_state = (keyboard_state | 0x0001);
								else keyboard_state = (keyboard_state & 0x003E);
							}

							if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x3B) // J = 'B'
							{
								if (term_ps2_release > 0) keyboard_state = (keyboard_state | 0x0010);
								else keyboard_state = (keyboard_state & 0x002F);
							}

							if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x42) // K = 'A'
							{
								if (term_ps2_release > 0) keyboard_state = (keyboard_state | 0x0020);
								else keyboard_state = (keyboard_state & 0x001F);
							}

							term_ps2_extended = 0;
							term_ps2_release = 0;
						}

						term_position++;
						if (term_position >= 256) term_position -= 256;
					}
				}
			}
			else
			{
				keyboard_state = 0x003F;
			}

			// buttons AND keyboard combined together
			term_game_button_previous = term_game_button_current;
			term_game_button_current = ((((PORTB & 0x000C) << 2) | (PORTA & 0x000F)) & keyboard_state);

			if (term_game_button_current == 0x003F)
			{
				term_game_button_held = hold_time;
			}

			if (term_game_button_held > 0) term_game_button_held--;

			if (term_game_button_current != term_game_button_previous || term_game_button_held == 0)
			{
				term_game_seed++; // psuedo-random

				if ((term_game_button_current & 0x0004) == 0x0000 &&
					((term_game_button_previous & 0x0004) == 0x0004 || term_game_button_held == 0)) // down
				{
					if (term_game_piece_y < 21)
					{
						term_game_piece_y++;
					}	

					term_game_counter_low = term_game_delay_low;
					term_game_counter_high = term_game_delay_high;		

					descend = 1;
					check = 1;
				}

				if (descend == 0)
				{
					if ((term_game_button_current & 0x0001) == 0x0000 &&
						((term_game_button_previous & 0x0001) == 0x0001 || term_game_button_held == 0)) // right
					{
						if (term_game_piece_x < 14)
						{
							term_game_piece_x++;
						}

						check = 1;
					}

					if ((term_game_button_current & 0x0002) == 0x0000 &&
						((term_game_button_previous & 0x0002) == 0x0002 || term_game_button_held == 0)) // left
					{
						if (term_game_piece_x > 0)
						{
							term_game_piece_x--;
						}

						check = 1;
					}

					if ((term_game_button_current & 0x0008) == 0x0000 &&
						((term_game_button_previous & 0x0008) == 0x0008 || term_game_button_held == 0)) // up
					{
						// do nothing

						check = 1;
					}

					if ((term_game_button_current & 0x0010) == 0x0000 &&
						((term_game_button_previous & 0x0010) == 0x0010)) // B
					{
						if (term_game_piece_rot == 3)
						{
							term_game_piece_rot = 0;
						}
						else
						{
							term_game_piece_rot++;
						}				

						check = 1;
					}

					if ((term_game_button_current & 0x0020) == 0x0000 &&
						((term_game_button_previous & 0x0020) == 0x0020)) // A
					{
						if (term_game_piece_rot == 0)
						{
							term_game_piece_rot = 3;
						}
						else
						{
							term_game_piece_rot--;
						}				

						check = 1;
					}
				}

				redraw = 1;
			}

			if (term_game_button_held == 0) term_game_button_held = hold_time;
		}

		if (check > 0) // check for collision
		{
			check = 0;

			for (int y=0; y<4; y++)
			{
				for (int x=0; x<4; x++)
				{
					loc = term_game_piece_current*64+term_game_piece_rot*16+y*4+x;

					if (game_pieces[loc] != ' ')
					{
						if (term_game_field[(term_game_piece_y+y)*16+(term_game_piece_x+x)] != ' ')
						{
							check = 1;
						}
					}
				}
			}

			if (check == 1) // collision detected
			{
				term_game_piece_x = prev_x;
				term_game_piece_y = prev_y;
				term_game_piece_rot = prev_rot;

				if (first > 0) // game over
				{
					if (term_mode == 0)
					{
						text_character(18+text_shift, 5, 'G');
						text_character(19+text_shift, 5, 'A');
						text_character(20+text_shift, 5, 'M');
						text_character(21+text_shift, 5, 'E');
						text_character(22+text_shift, 5, ' ');
						text_character(23+text_shift, 5, 'O');
						text_character(24+text_shift, 5, 'V');
						text_character(25+text_shift, 5, 'E');
						text_character(26+text_shift, 5, 'R');
					}
					else if (term_mode == 1)
					{
						color_character(56+color_shift, 17, 'G');
						color_character(62+color_shift, 17, 'A');
						color_character(68+color_shift, 17, 'M');
						color_character(74+color_shift, 17, 'E');
						color_character(80+color_shift, 17, ' ');
						color_character(86+color_shift, 17, 'O');
						color_character(92+color_shift, 17, 'V');
						color_character(98+color_shift, 17, 'E');
						color_character(104+color_shift, 17, 'R');
					}
		
					while (1) { }
				}
			}

			first = 0;
			
			if (descend > 0) // now add to field
			{
				if (check == 1)
				{
					for (int y=0; y<4; y++)
					{
						for (int x=0; x<4; x++)
						{
							loc = term_game_piece_current*64+term_game_piece_rot*16+y*4+x;

							if (game_pieces[loc] != ' ')
							{
								term_game_field[(term_game_piece_y+y)*16+(term_game_piece_x+x)] = '#';
							}
						}
					}

					lines = 0;

					// check for lines here
					for (int y=20; y>=2; y--)
					{
						check = 1;

						for (int x=0; x<19; x++)
						{
							if (term_game_field[y*16+x] == ' ') check = 0;
						}

						if (check == 1) // remove line
						{
							term_game_seed++; // psuedo-random

							for (int i=y; i>=2; i--)
							{
								for (int x=0; x<19; x++)
								{
									term_game_field[i*16+x] = term_game_field[(i-1)*16+x];
								}
							}

							y++; // check line again

							lines++;
						}
					}

					if (lines == 0) term_game_points += 1;
					else if (lines == 1) term_game_points += 10;
					else if (lines == 2) term_game_points += 20;
					else if (lines == 3) term_game_points += 50;
					else if (lines == 4) term_game_points += 100;

					term_game_tally += 1;

					// new speed setting
					if (term_game_tally < 20)
					{
						term_game_delay_low = 60;
						term_game_delay_high = 0;
					}
					else if (term_game_tally < 40)
					{
						term_game_delay_low = 50;
						term_game_delay_high = 0;
					}
					else if (term_game_tally < 60)
					{
						term_game_delay_low = 40;
						term_game_delay_high = 0;
					}
					else if (term_game_tally < 80)
					{
						term_game_delay_low = 30;
						term_game_delay_high = 0;
					}
					else if (term_game_tally < 100)
					{
						term_game_delay_low = 20;
						term_game_delay_high = 0;
					}
					else
					{
						term_game_delay_low = 10;
						term_game_delay_high = 0;
					}			

					// new piece here
					term_game_piece_x = 6;
					term_game_piece_y = 1;
					term_game_piece_rot = 0;
					term_game_piece_current = term_game_piece_next;

					// random bag
					term_game_piece_next = bag[pos];
					pos++;
					if (pos >= 7)
					{
						pos = 0;

						for (int i=0; i<7; i++) bag[i] = game_bags[(term_game_seed & 0x0F)*7+i];
					}

					term_game_piece_next++;
					if (term_game_piece_next >= 7) term_game_piece_next = 0;

					first = 1;

					if (term_mode == 0)
					{
						text_character(18+text_shift, 1, 'P');
						text_character(19+text_shift, 1, 'T');
						text_character(20+text_shift, 1, 'S');
						text_character(21+text_shift, 1, ':');
						text_character(22+text_shift, 1, ' ');

						text_character(23+text_shift, 1, (unsigned int)((term_game_points % 10000) / 1000) + '0');
						text_character(24+text_shift, 1, (unsigned int)((term_game_points % 1000) / 100) + '0');
						text_character(25+text_shift, 1, (unsigned int)((term_game_points % 100) / 10) + '0');
						text_character(26+text_shift, 1, (unsigned int)((term_game_points % 10) / 1) + '0');

						text_character(18+text_shift, 3, 'N');
						text_character(19+text_shift, 3, 'E');
						text_character(20+text_shift, 3, 'X');
						text_character(21+text_shift, 3, 'T');
						text_character(22+text_shift, 3, ':');
						text_character(23+text_shift, 3, ' ');

						switch (term_game_piece_next)
						{
							case 0: { text_character(24+text_shift, 3, 'I'); break; }
							case 1: { text_character(24+text_shift, 3, 'J'); break; }
							case 2: { text_character(24+text_shift, 3, 'L'); break; }
							case 3: { text_character(24+text_shift, 3, 'O'); break; }
							case 4: { text_character(24+text_shift, 3, 'S'); break; }
							case 5: { text_character(24+text_shift, 3, 'Z'); break; }
							case 6: { text_character(24+text_shift, 3, 'T'); break; }
							case 7: { text_character(24+text_shift, 3, '?'); break; }
							default: { text_character(24+text_shift, 3, '?'); break; }
						}
					}
					else if (term_mode == 1)
					{
						color_character(56+color_shift, 1, 'P');
						color_character(62+color_shift, 1, 'T');
						color_character(68+color_shift, 1, 'S');
						color_character(74+color_shift, 1, ':');
						color_character(80+color_shift, 1, ' ');

						color_character(86+color_shift, 1, (unsigned int)((term_game_points % 10000) / 1000) + '0');
						color_character(92+color_shift, 1, (unsigned int)((term_game_points % 1000) / 100) + '0');
						color_character(98+color_shift, 1, (unsigned int)((term_game_points % 100) / 10) + '0');
						color_character(104+color_shift, 1, (unsigned int)((term_game_points % 10) / 1) + '0');

						color_character(56+color_shift, 9, 'N');
						color_character(62+color_shift, 9, 'E');
						color_character(68+color_shift, 9, 'X');
						color_character(74+color_shift, 9, 'T');
						color_character(80+color_shift, 9, ':');
						color_character(86+color_shift, 9, ' ');

						switch (term_game_piece_next)
						{
							case 0: { color_character(92+color_shift, 9, 'I'); break; }
							case 1: { color_character(92+color_shift, 9, 'J'); break; }
							case 2: { color_character(92+color_shift, 9, 'L'); break; }
							case 3: { color_character(92+color_shift, 9, 'O'); break; }
							case 4: { color_character(92+color_shift, 9, 'S'); break; }
							case 5: { color_character(92+color_shift, 9, 'Z'); break; }
							case 6: { color_character(92+color_shift, 9, 'T'); break; }
							case 7: { color_character(92+color_shift, 9, '?'); break; }
							default: { color_character(92+color_shift, 9, '?'); break; }
						}
					}
				}

				descend = 0;
			}

			check = 0;
		}

		if (redraw > 0) // redraw screen
		{
			if (term_mode == 0)
			{
				for (int y=0; y<24; y++)
				{
					for (int x=0; x<16; x++)
					{
						term_memory[y*80+x+text_shift] = term_game_field[y*16+x];
					}
				}

				for (int y=0; y<4; y++)
				{
					for (int x=0; x<4; x++)
					{
						loc = term_game_piece_current*64+term_game_piece_rot*16+y*4+x;

						if (game_pieces[loc] != ' ') term_memory[(y+term_game_piece_y)*80+(x+term_game_piece_x)+text_shift] = game_pieces[loc];
					}
				}
			}
			else if (term_mode == 1)
			{
				for (int y=0; y<24; y++)
				{
					for (int x=0; x<16; x++)
					{
						if (term_game_field[y*16+x] == ' ')
						{
							for (int i=0; i<color_scale; i++)
							{
								for (int j=0; j<color_scale; j++)
								{
									term_color[(y*color_scale+i)*128+(x*color_scale+j)+color_shift] = 0x00;
								}
							}
						}
						else if (term_game_field[y*16+x] == '#')
						{
							for (int i=0; i<color_scale; i++)
							{
								for (int j=0; j<color_scale; j++)
								{
									if (i == 0 || j == 0)
									{
										term_color[(y*color_scale+i)*128+(x*color_scale+j)+color_shift] = 0x15;
									}
									else
									{
										term_color[(y*color_scale+i)*128+(x*color_scale+j)+color_shift] = 0x2A;
									}
								}
							}
						}
						else
						{
							for (int i=0; i<color_scale; i++)
							{
								for (int j=0; j<color_scale; j++)
								{
									term_color[(y*color_scale+i)*128+(x*color_scale+j)+color_shift] = 0x7F;
								}
							}
						}
					}
				}

				switch (term_game_piece_current)
				{
					case 0: { color_value = 0x4F; break; } // I = cyan
					case 1: { color_value = 0x43; break; } // J = blue
					case 2: { color_value = 0x6A; break; } // L = light grey
					case 3: { color_value = 0x7C; break; } // O = yellow
					case 4: { color_value = 0x4C; break; } // S = green
					case 5: { color_value = 0x70; break; } // Z = red
					case 6: { color_value = 0x73; break; } // T = magenta
					case 7: { color_value = 0x7F; break; } 
					default: { color_value = 0x7F; break; }
				}

				for (int y=0; y<24*color_scale; y++)
				{
					for (int x=0; x<16*color_scale; x++)
					{
						term_color_eds[y*128+x+color_shift+4096] = 0x00;
					}
				}

				for (int y=0; y<4; y++)
				{
					for (int x=0; x<4; x++)
					{
						loc = term_game_piece_current*64+term_game_piece_rot*16+y*4+x;

						if (game_pieces[loc] != ' ')
						{
							for (int i=0; i<color_scale; i++)
							{
								for (int j=0; j<color_scale; j++)
								{
									if (i == 0 || j == 0)
									{
										term_color_eds[((y+term_game_piece_y)*color_scale+i)*128+((x+term_game_piece_x)*color_scale+j)+color_shift+4096] = (color_value & 0x3F);
									}
									else
									{
										term_color_eds[((y+term_game_piece_y)*color_scale+i)*128+((x+term_game_piece_x)*color_scale+j)+color_shift+4096] = color_value;
									}
								}
							}
						}
					}
				}
			}

			redraw = 0;
		}

		while (term_scanline > 0) { }

		while (term_scanline <= term_bottom) { }
	}
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
	term_bottom = 768-1; // default

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
	int option = 0;

	text_string(0, 24, text_menu_0);

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

	CNPUB = 0x000F; // pull-up on RB3 to RB0

	for (unsigned int i=0; i<32768; i++) { for (unsigned int j=0; j<64; j++) { } } // delay

	// read external switches for different input types
	//option = (((PORTB & 0x0003) ^ 0x0003) & 0x0003);
	option = (PORTB & 0x0003);

	CNPUB = 0x000C; // turn off pull-ups on RB1 and RB0

	if (option == 0) // UART
	{
		TRISA = 0x000F;
		LATA = 0x0000;
		TRISB = 0x7F3F;
		LATB = 0x0000;

		CNPUB = (CNPUB | 0x0060); // pull-up on RB6 and RB5

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

		text_string(32, 24, text_menu_1);
	}
	else if (option == 1) // PS/2
	{
		TRISA = 0x000F;
		LATA = 0x0000;
		TRISB = 0x7F7F;
		LATB = 0x0000;

		CNPUB = (CNPUB | 0x0060); // pull-up on RB6 and RB5

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

		term_ps2_synced = 0;

		text_string(32, 24, text_menu_2);
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

		text_string(32, 24, text_menu_3);
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

		text_string(32, 24, text_menu_4);
	}

	unsigned int buttons;

	while (1)
	{
		// play game if buttons pressed
		buttons = (PORTB & 0x000C);
		if ((buttons & 0x0004) == 0x0000 || (buttons & 0x0008) == 0x0000)
		{
			// for safety
			term_bottom = 576-1; // usually 768-1 by default

			if ((buttons & 0x0004) == 0x0000) term_mode = 0; // text mode
			if ((buttons & 0x0008) == 0x0000) term_mode = 1; // color mode

			play(); // play game
	
			while (1) { } // infinite loop
		}

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
						term_write(term_dma_address, (term_array[(term_position & 0x00FF)] & 0x00FF));

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
						else if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x05 ||
							(term_array[(term_position & 0x00FF)] & 0x00FF) == 0x06) // F1 or F2 to play game
						{
							// for safety
							term_bottom = 576-1; // usually 768-1 by default

							if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x05) term_mode = 0; // text mode
							else if ((term_array[(term_position & 0x00FF)] & 0x00FF) == 0x06) term_mode = 1; // color mode

							play(); // play game

							while (1) { } // infinite loop
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
					text_string(0, 0, text_help_0);
					text_string(0, 1, text_help_1);
					text_string(0, 2, text_help_2);
					text_string(0, 3, text_help_3);
					text_string(0, 4, text_help_4);
					text_string(0, 5, text_help_5);
					text_string(0, 6, text_help_6);
					text_string(0, 7, text_help_7);
					text_string(0, 8, text_help_8);
					text_string(0, 9, text_help_9);
					text_string(0, 10, text_help_10);
					text_string(0, 11, text_help_11);
					text_string(0, 12, text_help_12);
					text_string(0, 13, text_help_13);
					text_string(0, 14, text_help_14);
					text_string(0, 15, text_help_15);
					text_string(0, 16, text_help_16);
					text_string(0, 17, text_help_17);
					text_string(0, 18, text_help_18);
					text_string(0, 19, text_help_19);
					text_string(0, 20, text_help_20);
					text_string(0, 21, text_help_21);
					text_string(0, 22, text_help_22);

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

					term_orientation = 0;

					// turn on colors
					TRISB = (TRISB & 0x00FF) | 0x8000;
					PORTB = 0x0000;

					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 3 && term_keycode[3] == 'C')
				{
					term_mode = 1;

					term_orientation = (int)(term_keycode[term_sequence-1]-'0');

					// turn on colors
					TRISB = (TRISB & 0x00FF) | 0x8000;
					PORTB = 0x0000;

					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 2 && term_keycode[2] == 'E')
				{
					term_setting_echo = 1 - term_setting_echo;

					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 3 && term_keycode[3] == 'E')
				{
					term_setting_echo = (int)(term_keycode[term_sequence-1]-'0');
					
					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 4 && term_keycode[2] == 'A')
				{
					term_dma_address = (unsigned int)(term_keycode[term_sequence-1]) * 256 + (unsigned int)(term_keycode[term_sequence]);
					
					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 2 && term_keycode[2] == 'R')
				{
					//if (term_setting_echo > 0)
					//{
						if (option == 0)
						{
							U1TXREG = term_read(term_dma_address);
						}
					//}

					term_dma_address++;

					term_command = 0;
					term_sequence = 0;
				}
				else if (term_sequence == 4 && term_keycode[2] == 'W')
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

	// firmware update uses UART, will rewrite all but main() essentially
	// [file.hex is the Intel HEX file from the dist/default/production folder]
	// Commands: 
	// chmod a+rw /dev/ttyUSB0
	// stty -F /dev/ttyUSB0 9600
	// #echo 'U' > /dev/ttyUSB0
	// Wait, then:
	// cat file.hex > /dev/ttyUSB0

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

		// delay and then detect baud rate
		//for (unsigned int i=0; i<32768; i++) { for (unsigned int j=0; j<64; j++) { } } // delay
		//U1MODEbits.ABAUD = 1; // detect baud rate of UART1 (by pressing = sign on the keyboard)
		//while (U1MODEbits.ABAUD == 1) { } // wait for sync using 'U' character or 0x55 values

		U1TXREG = 'E'; // dummy transfer

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

		U1TXREG = 'R'; // dummy transfer

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

		U1TXREG = 'X'; // dummy transfer

		for (unsigned int i=0; i<32768; i++) { for (unsigned int j=0; j<64; j++) { } } // delay

		// reset system
		asm("RESET");
	}

	entry();	

	return 1;
}


/*
	// below used for debugging
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
	// test transfer
	U1TXREG = '*'; // dummy transfer
	// delay
	for (unsigned int i=0; i<32768; i++) { for (unsigned int j=0; j<64; j++) { } } // delay
	// infinite loop
	while (1) { }
*/