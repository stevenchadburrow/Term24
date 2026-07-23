
; Assembly code for Term24

.equ term_pixels, 0x2A00
.equ term_buffer, 0x2A50
.equ term_scanline, 0x2AA0
.equ term_location, 0x2AA2
.equ term_counter, 0x2AA4
.equ term_stretch, 0x2AA6
.equ term_scroll, 0x2AA8
.equ term_orientation, 0x2AAA
.equ term_cursor, 0x2AAC
.equ term_mode, 0x2AAE
.equ term_bottom_text, 0x2AB0
.equ term_bottom_color, 0x2AB2

.equ term_map, 0x3000
.equ term_memory, 0x3800 ; overlaps term_color
.equ term_color, 0x5000
.equ term_color_eds, 0x8000


.section usercode code
.global _TermScanline
_TermScanline:
	push w0
	push w1
	push w2
	push w3
	push w4
	push w5
	
	; clear timer1 interrupt flag
	mov.w 0x0800, w0
	mov.w #0xFFF7, w1
	and.w w0, w1, w0
	mov.w w0, 0x0800

	; used below
	mov.w #0x0000, w3

	; increment line scanline
	; zero scanline and counter if necessary
	mov.w term_scanline, w0
	inc.w w0, w0
	mov.w #0x0326, w1 ; 806 total lines
	cpsne.w w0, w1 ; skip if not equal
	mov.w #0x0000, w0
	mov.w w0, term_scanline
	cpsne.w w0, w3
	mov.w w0, term_location
	cpsne.w w0, w3
	mov.w w0, term_counter

	; switch to color mode
	mov.w term_mode, w2
	cpseq.w w2, w3
	bra TermScanline_Color

	; calculate top row of pixels
	mov.w term_scanline, w0
	mov.w #0x0320, w1
	cpslt.w w0, w1
	bra TermScanline_Next

	; if not visible region, exit
	mov.w term_scanline, w0
	mov.w #0x0258, w1 ; 600 visible lines
	cpslt.w w0, w1 ; skip if less than
	bra TermScanline_Blank

TermScanline_Next:

	; last row (calculated) of each character is not visible, exit
	mov.w term_counter, w0
	cpsne.w w0, w3
	bra TermScanline_Continue

	; find location of pixels
	mov.w #term_pixels, w1
	
	; clear high byte of w0
	mov.w #0x0000, w0

	; repeat 80 times
	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13
	sl.b w0, w0
	mov.b WREG, 0x0E13

TermScanline_Continue:
	; only show black
	mov.w #0x0000, w0
	mov.b WREG, 0x0E13 

	; branch to copy
	mov.w term_stretch, w0
	mov.w #0x0002, w1
	cpsne.w w0, w1
	bra TermScanline_Copy

	; find location in characters
	mov.w #term_memory, w0
	mov.w term_location, w1
	add.w w0, w1, w1
	mov.w term_bottom_text, w3
	add.w w0, w3, w3

	; if on last line do not scroll (in default mode)
	cpslt.w w1, w3
	bra TermScanline_Half

	; else, use scrolling
	mov.w term_scroll, w0
	sl.w w0, #0x0002, w2
	add.w w0, w2, w0
	sl.w w0, #0x0004, w0
	add.w w0, w1, w1 
	
	; roll over when scrolling
	mov.w term_bottom_text, w2
	cpslt.w w1, w3
	sub.w w1, w2, w1

TermScanline_Half:
	; add to location
	mov.w term_stretch, w0
	mov.w #0x0000, w2
	cpseq.w w0, w2
	mov.w #0x0028, w2
	add.w w2, w1, w1

	; load text map for each character
	mov.w #term_map, w2

	; load buffer address
	mov.w #term_buffer, w3

	; add to location
	mov.w term_stretch, w0
	mov.w #0x0000, w4
	cpseq.w w0, w4
	mov.w #0x0028, w4
	add.w w4, w3, w3

	; load video counter
	mov.w term_counter, w4

	; load mask
	mov.w #0x00FF, w5	

	; repeat 40 times
	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	mov.b [w1++], w0
	and.w w0, w5, w0
	sl.w w0, #0x0003, w0
	add.w w0, w4, w0
	mov.b [w2+w0], [w3++]

	bra TermScanline_Calculate

TermScanline_Copy:
	; get locations ready
	mov.w #term_buffer, w0
	mov.w #term_pixels, w1

	; repeat 80 times
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]

	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	mov.w [w0++], [w1++]
	
TermScanline_Calculate:
	; vertical extension
	mov.w term_stretch, w0
	inc.w w0, w0
	mov.w w0, term_stretch
	mov.w #0x0003, w1
	cpseq.w w0, w1
	bra TermScanline_Pop
	mov.w #0x0000, w0
	mov.w w0, term_stretch

	; increment counter by 1
	; and location by 160 if need be
	mov.w term_counter, w0
	inc.w w0, w0
	mov.w #0x0008, w1
	cpsne.w w0, w1
	mov.w #0x0000, w0
	mov.w w0, term_counter
	mov.w #0x0000, w1
	mov.w #0x0050, w2
	mov.w term_location, w3
	cpsne.w w0, w1
	add.w w2, w3, w3
	mov.w w3, term_location

TermScanline_Pop:

	pop w5
	pop w4
	pop w3
	pop w2
	pop w1
	pop w0

	return

TermScanline_Blank:

	; only show black
	mov.w #0x0000, w0
	mov.b WREG, 0x0E13 	

TermScanline_Exit:
	pop w5
	pop w4
	pop w3
	pop w2
	pop w1
	pop w0

	return


TermScanline_Color:

	; if not visible region, exit
	mov.w term_bottom_color, w1 ; default of 0x02FF, 768 visible lines
	cpslt.w w0, w1 ; skip if less than
	bra TermScanline_Blank

	; used below
	mov.w #0x0000, w2

	; increment location
	mov.w term_counter, w0
	mov.w term_location, w1
	inc.w w0, w0
	mov.w #0x0006, w3
	cpsne.w w0, w3
	mov.w w2, w0
	mov.w w0, term_counter
	cpsne.w w0, w2
	inc.w w1, w1
	mov.w w1, term_location

	mov.w term_orientation, w0
	cpsne.w w0, w2
	bra TermScanline_Vertical
	mov.w #0x0001, w2
	cpsne.w w0, w2
	bra TermScanline_Horizontal

TermScanline_Layers:

	; shift scanline distance
	sl.w w1, #0x0007, w1 ; multiply by 128
	mov.w #0x0001, w2 ; lsb only
	and.w w0, w2, w0 ; keep only bit 0
	sl.w w1, w0, w1 ; shift by either 0 or 1 bit
	mov.w #0x4000, w2 ; add 16K bytes
	btsc.w w0, #0x0000
	mov.w #0x0080, w2 ; or add 128 bytes
	add.w w1, w2, w2 ; add to location

	; find pixel address
	mov.w #term_color, w3 ; find pixel location
	add.w w1, w3, w1 ; add to address
	add.w w2, w3, w2 ; add to address

	; clear high byte
	mov.w #0x0000, w0

	; read only from EDS, not PSV
	push DSRPAG
	push DSWPAG
    
	; first page of EDS shown from 0x8000-0xFFFF
	movpag #0x0001, DSRPAG
	movpag #0x0001, DSWPAG 

	mov.w #0x0000, w4 ; for comparisons

	; repeat 128 times
	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	mov.b [w1++], w0
	mov.b [w2++], w3
	cpsne.w w0, w4
	mov.b w3, w0
	mov.b WREG, 0x0E13
	nop 

	; black pixels
	mov.w #0x0000, w0
	mov.b WREG, 0x0E13

	; return paging back to original state 
	pop DSWPAG
	pop DSRPAG 

	bra TermScanline_Exit

TermScanline_Vertical:
	; shift scanline distance
	sl.w w1, #0x0007, w1 ; multiply by 128

	; load values for vertical scrolling
	mov.w term_scroll, w2
	sl.w w2, #0x0008, w2
	add.w w1, w2, w1
	bclr.w w1, #0x000F

	; find pixel address
	mov.w #term_color, w2 ; find pixel location
	add.w w1, w2, w1 ; add to address

	; clear high byte
	mov.w #0x0000, w0

	; read only from EDS, not PSV
	push DSRPAG
	push DSWPAG
    
	; first page of EDS shown from 0x8000-0xFFFF
	movpag #0x0001, DSRPAG
	movpag #0x0001, DSWPAG 
	
	; repeat 128 times
	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	mov.b [w1++], w0
	mov.b WREG, 0x0E13
	nop
	nop
	nop
	nop

	; black pixels
	mov.w #0x0000, w0
	mov.b WREG, 0x0E13

	; return paging back to original state 
	pop DSWPAG
	pop DSRPAG 

	bra TermScanline_Exit

TermScanline_Horizontal:
	; find pixel address
	sl.w w1, #0x0008, w1 ; multiply by 256
	mov.w #term_color, w2 ; find pixel location
	add.w w1, w2, w1 ; add to address
	
	; load values for horizontal scrolling
	mov.w term_scroll, w2
	mov.w #0x00FF, w3
	and.w w2, w3, w2

	; clear high byte
	mov.w #0x0000, w0

	; read only from EDS, not PSV
	push DSRPAG
	push DSWPAG
    
	; first page of EDS shown from 0x8000-0xFFFF
	movpag #0x0001, DSRPAG
	movpag #0x0001, DSWPAG 
	
	; repeat 128 times
	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	mov.b [w1+w2], w0
	mov.b WREG, 0x0E13
	inc.w w2, w2
	and.w w2, w3, w2
	nop
	nop

	; black pixels
	mov.w #0x0000, w0
	mov.b WREG, 0x0E13

	; return paging back to original state 
	pop DSWPAG
	pop DSRPAG 

	bra TermScanline_Exit


