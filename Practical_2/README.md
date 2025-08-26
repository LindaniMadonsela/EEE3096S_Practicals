# EEE3096S 2025 – Practical 2  

## 1. Aim
The aim of this practical was to gain experience programming in ARM Cortex-M0 assembly by controlling GPIO hardware directly. The task was to read inputs from pushbuttons and update LED patterns based on those inputs, while implementing timing and sequencing in pure assembly.

---

## 2. Objectives
The assembly program had to:
1. Increment LEDs by **1 every ~0.7 seconds** (default).  
2. When **SW0** is pressed: increment LEDs by **2**.  
3. When **SW1** is pressed: shorten delay to **~0.3 seconds**.  
4. When **SW2** is pressed: force LEDs to display **0xAA** until release.  
5. When **SW3** is pressed: **freeze** the LED pattern until release.  

SW0 and SW1 can be used together; only one of SW2 or SW3 will be active at a time.

---

## 3. Implementation Summary
- **Clock and GPIO configuration:**  
  - Enabled GPIOA and GPIOB clocks using RCC.  
  - Configured GPIOA as inputs with pull-up resistors for buttons.  
  - Configured GPIOB as outputs for LEDs.  

- **Logic:**  
  - Read GPIOA IDR to detect button states.  
  - Adjusted increment value and delay length based on SW0 and SW1.  
  - Overrode LED pattern with `0xAA` if SW2 pressed.  
  - Paused updates while SW3 pressed.  

- **Timing:**  
  - Used two software delay counters: `LONG_DELAY_CNT` (~0.7 s) and `SHORT_DELAY_CNT` (~0.3 s).  

- **Registers:**  
  - `R2` held the LED pattern.  
  - `R4` held increment step.  
  - `R5`/`R6` used for delays and masks.

---

## 4. How to Build and Run
1. **Open** the project in STM32CubeIDE.  
2. **Build** the project (`Project → Build All`).  
3. **Flash** the code to the STM32F0 Discovery board.  
4. **Observe** LED patterns and press SW0–SW3 to verify behaviour.  

---

## 5. Key Code Snippet
Example of SW0 and SW1 checks (Thumb-1 compatible):
```asm
    MOVS R6, #1         @ SW0 mask
    TST R3, R6
    BEQ sw0_pressed
    B skip_sw0
sw0_pressed:
    MOVS R4, #2
skip_sw0:

    MOVS R6, #2         @ SW1 mask
    TST R3, R6
    BEQ sw1_pressed
    B skip_sw1
sw1_pressed:
    LDR R5, SHORT_DELAY_CNT
skip_sw1:
