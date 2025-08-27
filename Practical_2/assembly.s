/*
 * assembly.s
 *
 */
 
 @ DO NOT EDIT
	.syntax unified
    .text
    .global ASM_Main
    .thumb_func

@ DO NOT EDIT
vectors:
	.word 0x20002000
	.word ASM_Main + 1

@ DO NOT EDIT label ASM_Main
ASM_Main:

	@ Some code is given below for you to start with
	LDR R0, RCC_BASE  		@ Enable clock for GPIOA and B by setting bit 17 and 18 in RCC_AHBENR
	LDR R1, [R0, #0x14]
	LDR R2, AHBENR_GPIOAB	@ AHBENR_GPIOAB is defined under LITERALS at the end of the code
	ORRS R1, R1, R2
	STR R1, [R0, #0x14]

	LDR R0, GPIOA_BASE		@ Enable pull-up resistors for pushbuttons
	MOVS R1, #0b01010101
	STR R1, [R0, #0x0C]
	LDR R1, GPIOB_BASE  	@ Set pins connected to LEDs to outputs
	LDR R2, MODER_OUTPUT
	STR R2, [R1, #0]
	MOVS R2, #0         	@ NOTE: R2 will be dedicated to holding the value on the LEDs

@ TODO: Add code, labels and logic for button checks and LED patterns

main_loop:
    @ ---- Read buttons ----
    LDR R0, GPIOA_BASE
    LDR R3, [R0, #0x10]       @ Read input data register (buttons)

    @ ---- Default settings ----
    MOVS R4, #1                @ Increment = 1
    LDR R5, LONG_DELAY_CNT     @ Delay = 0.7s by default

    @ ---- SW0 check: increment by 2 ----
    MOVS R6, #1         @ mask for SW0
    TST R3, R6
    BEQ sw0_pressed
    B skip_sw0
sw0_pressed:
    MOVS R4, #2
skip_sw0:

    @ ---- SW1 check: short delay ----
    MOVS R6, #2         @ mask for SW1
    TST R3, R6
    BEQ sw1_pressed
    B skip_sw1
sw1_pressed:
    LDR R5, SHORT_DELAY_CNT
skip_sw1:

    @ ---- SW2 check: force 0xAA ----
    MOVS R6, #4         @ mask for SW2
    TST R3, R6
    BNE check_sw3
    MOVS R2, #0xAA
    B write_leds

    @ ---- SW3 check: freeze pattern ----
check_sw3:
    MOVS R6, #8         @ mask for SW3
    TST R3, R6
    BNE normal_update
freeze_loop:
    LDR R3, [R0, #0x10]
    TST R3, R6
    BEQ freeze_loop

    @ ---- Normal counting ----
normal_update:
    ADDS R2, R2, R4


write_leds:
    STR R2, [R1, #0x14]       @ Write LED value to GPIOB ODR

    @ ---- Delay loop ----
    MOV R6, R5
delay_loop:
    SUBS R6, R6, #1
    BNE delay_loop

    B main_loop

@ LITERALS; DO NOT EDIT
	.align
RCC_BASE: 			.word 0x40021000
AHBENR_GPIOAB: 		.word 0b1100000000000000000
GPIOA_BASE:  		.word 0x48000000
GPIOB_BASE:  		.word 0x48000400
MODER_OUTPUT: 		.word 0x5555

@ TODO: Add your own values for these delays
LONG_DELAY_CNT:     .word 700000     @ ~0.7 seconds
SHORT_DELAY_CNT:    .word 300000     @ ~0.3 seconds
