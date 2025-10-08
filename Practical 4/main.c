/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32f4xx.h"
#include "lcd_stm32f4.h"
#include <stdint.h>



/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// TODO: Add values for below variables
#define NS     128 // Number of samples in LUT
#define TIM2CLK  16000000 // STM Clock frequency: Hint You might want to check the ioc file
#define F_SIGNAL  	440// Frequency of output analog signal

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch1;

/* USER CODE BEGIN PV */
// TODO: Add code for global variables, including LUTs
uint16_t Sin_LUT[NS] = {
  2048,2148,2248,2348,2447,2546,2644,2741,2837,2931,3024,3116,3205,3293,3379,3463,
  3544,3623,3699,3773,3843,3911,3976,4037,4095,4149,4199,4246,4289,4328,4363,4395,
  4422,4445,4465,4480,4491,4498,4502,4501,4496,4487,4474,4457,4436,4412,4383,4351,
  4315,4276,4233,4186,4136,4083,4026,3967,3904,3839,3770,3700,3626,3550,3472,3391,
  3309,3225,3139,3052,2963,2873,2782,2690,2597,2504,2410,2316,2221,2127,2032,1938,
  1844,1750,1657,1565,1474,1384,1296,1208,1122,1038,955,874,795,718,642,569,
  498,430,364,301,241,183,128,76,27
};

uint16_t Saw_LUT[NS] = {
  0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,
  512,544,576,608,640,672,704,736,768,800,832,864,896,928,960,992,
  1024,1056,1088,1120,1152,1184,1216,1248,1280,1312,1344,1376,1408,1440,1472,1504,
  1536,1568,1600,1632,1664,1696,1728,1760,1792,1824,1856,1888,1920,1952,1984,2016,
  2048,2080,2112,2144,2176,2208,2240,2272,2304,2336,2368,2400,2432,2464,2496,2528,
  2560,2592,2624,2656,2688,2720,2752,2784,2816,2848,2880,2912,2944,2976,3008,3040,
  3072,3104,3136,3168,3200,3232,3264,3296,3328,3360,3392,3424,3456,3488,3520,3552,
  3584,3616,3648,3680,3712,3744,3776,3808,3840,3872,3904,3936,3968,4000,4032,4064
};

uint16_t Triangle_LUT[NS] = {
  0,64,128,192,256,320,384,448,512,576,640,704,768,832,896,960,
  1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,
  2048,2112,2176,2240,2304,2368,2432,2496,2560,2624,2688,2752,2816,2880,2944,3008,
  3072,3136,3200,3264,3328,3392,3456,3520,3584,3648,3712,3776,3840,3904,3968,4032,
  4095,4032,3968,3904,3840,3776,3712,3648,3584,3520,3456,3392,3328,3264,3200,3136,
  3072,3008,2944,2880,2816,2752,2688,2624,2560,2496,2432,2368,2304,2240,2176,2112,
  2048,1984,1920,1856,1792,1728,1664,1600,1536,1472,1408,1344,1280,1216,1152,1088,
  1024,960,896,832,768,704,640,576,512,448,384,320,256,192,128,64
};

uint16_t Piano_LUT[NS] = {
  2048, 2260, 1719, 1472, 1944, 2392, 2032, 2065, 2105, 2038, 2035, 2047, 2060, 1995, 1920, 2077,
  2120, 2044, 2051, 2030, 2189, 2220, 2136, 2236, 2130, 2193, 2119, 1914, 2155, 2006, 2006, 2102,
  1886, 1920, 1776, 2069, 1801, 1999, 2403, 1804, 1697, 1969, 1968, 1978, 1961, 2727, 1814, 2227,
  1953, 2030, 2068, 1941, 2127, 1988, 2008, 2058, 2045, 2239, 1896, 2071, 2062, 2053, 2055, 2045,
  2031, 2062, 2056, 2027, 2066, 2498, 1794, 2099, 2093, 2047, 2058, 2030, 2723, 1837, 1891, 2105,
  2027, 2138, 2034, 1928, 2607, 1835, 2038, 2007, 2321, 2381, 2009, 1938, 2113, 1949, 1904, 1835,
  2212, 1832, 2040, 2054, 1987, 2327, 1810, 2013, 2181, 1884, 1931, 2055, 2100, 2014, 2029, 2046,
  2066, 2134, 2012, 1942, 2012, 2073, 2060, 1324, 1589, 1901, 2310, 2232, 2036, 1498, 1519, 2446,
  2248, 2044, 1956, 1994, 2609, 2426, 2163, 3146, 2177, 2223, 1824, 2258, 2168, 1999, 1961, 2014,
  1830, 2197, 1770, 2125, 2172, 1849, 2036, 2022, 2059, 2022, 2062, 2034, 2020, 2230, 1978, 2053,
  2025, 2035, 2045, 2050, 2065, 2046, 2064, 2045, 2048, 3610, 1013, 2209, 2020, 2082, 2098, 1806,
  2139, 2138, 1800, 2187, 2069, 2390, 2056, 2158, 2049, 2105, 2002, 2020, 2132, 2136, 2616, 2022,
  1914, 2231, 1947, 2014, 2137, 2065, 1988, 2040, 2714, 2500, 1795, 1994, 2036, 2031, 2036, 1873,
  2039, 2047, 2049, 2066, 2026, 2131, 1970, 2047, 2049, 2014, 2032, 1531, 2449, 1706, 1718, 2036,
  2001, 2071, 1905, 1648, 1967, 2345, 2049, 1674, 2028, 1891, 1980, 1847, 2710, 2562, 2186, 1802,
  2258, 2121, 2076, 2039, 2794, 1978, 1810, 2252, 2002, 2219, 1911, 2110, 2107, 1999, 2033, 2050,
  2082, 2085, 2009, 2037, 2061, 2027, 2043, 2051, 2033, 2046, 2032, 2032, 2050, 2135, 1919, 2140,
  2066, 2033, 2056, 2001, 2171, 2065, 1977, 2074, 2003, 2713, 1081, 2065, 2031, 2115, 2068, 2844,
  1862, 2040, 2055, 2209, 1919, 1909, 1965, 2474, 2072, 2160, 2131, 2077, 3149, 2401, 1819, 2118,
  1918, 2037, 2107, 2079, 2112, 2014, 2068, 2024, 2121, 2117, 1976, 2051, 2075, 2058, 2062, 1981,
  2108, 1793, 2007, 1901, 1940, 2183, 1853, 1942, 2114, 1811, 1920, 2198, 2277, 2145, 1951, 2462,
  1868, 2359, 2331, 2108, 1938, 2177, 2091, 1147, 2571, 2283, 1745, 2111, 2036, 2185, 2016, 2079,
  2084, 2073, 2066, 2049, 2201, 2000, 2030, 2056, 2078, 2047, 2030, 2067, 2057, 2059, 2039, 2041,
  1957, 2075, 1762, 2196, 2217, 1887, 2244, 1740, 2211, 1731, 1989, 2130, 2022, 2183, 2046, 2077,
  2015, 2009, 1995, 1205, 2228, 2388, 1765, 1988, 1995, 2499, 1734, 2257, 2204, 2301, 1990, 2048
};

uint16_t Guitar_LUT[NS] = {
  2048, 2048, 2553, 2149, 2045, 1970, 2251, 2192, 2118, 1843, 1699, 1774, 2005, 2167, 2161, 2103,
  2051, 2090, 2211, 2331, 1858, 905, 696, 919, 1009, 1435, 1903, 2065, 2081, 2054, 2055, 2010,
  2033, 2011, 2028, 2078, 2060, 2032, 2024, 2018, 2022, 2173, 2193, 1974, 1994, 1985, 2045, 2065,
  2056, 2036, 2049, 2187, 1822, 2481, 1639, 2308, 1626, 2240, 1778, 2186, 1790, 1990, 1816, 2029,
  1948, 1905, 1922, 1938, 2097, 2005, 2590, 1128, 1948, 1653, 2358, 1776, 2053, 2009, 2020, 2018,
  2041, 2039, 2020, 2019, 2036, 2021, 2026, 2029, 2028, 2183, 2003, 1859, 1992, 2031, 1993, 2027,
  2018, 2045, 2048, 2042, 2042, 2056, 2196, 2145, 2101, 1978, 1856, 2079, 2201, 2158, 2015, 1922,
  2035, 2174, 2139, 1917, 2093, 1884, 2117, 2062, 1956, 1604, 2186, 2190, 2087, 2135, 2086, 1922,
  2073, 2058, 2071, 2064, 2045, 2018, 2036, 2050, 2068, 2057, 2021, 2042, 2038, 1944, 2080, 1963,
  2070, 1934, 2107, 2005, 2050, 2051, 2035, 1830, 2049, 1863, 2107, 2223, 2125, 1988, 1872, 2035,
  2172, 2128, 1991, 1877, 1981, 2046, 1972, 2239, 2058, 2011, 2589, 2362, 1745, 2163, 2104, 2266,
  2343, 1788, 2179, 1777, 2526, 2026, 2593, 1840, 2450, 1800, 2496, 1898, 2440, 1897, 2445, 1937,
  2310, 1781, 1939, 2157, 1917, 2087, 2018, 2424, 2182, 1419, 1605, 2130, 2417, 2371, 2106, 1996,
  2020, 2226, 2275, 2217, 1944, 1744, 1856, 1863, 2263, 2150, 2086, 1707, 2092, 2851, 2717, 2586,
  2048, 2012, 2037, 2054, 2075, 2016, 2017, 2024, 2039, 2071, 2086, 2049, 2017, 2026, 2125, 2257,
  1926, 1888, 2091, 2225, 2069, 2043, 2032, 2036, 2056, 2056, 1831, 2713, 1767, 2416, 1790, 2456,
  1847, 2409, 1760, 2282, 1811, 2231, 1839, 1933, 1922, 2041, 1926, 2084, 2179, 1357, 2275, 2515,
  2187, 1686, 2466, 1865, 2055, 1991, 2014, 1979, 2069, 2025, 2020, 2028, 2054, 2047, 2050, 2037,
  1735, 2180, 2072, 1941, 1956, 2243, 2303, 2230, 2061, 2051, 2060, 2062, 1717, 2181, 2061, 1799,
  2137, 2211, 2124, 2005, 1852, 2063, 2215, 2110, 1974, 1933, 1874, 2231, 2256, 2109, 1937, 2597,
  1473, 1848, 1875, 2125, 1754, 2008, 2046, 2119, 2036, 2036, 2027, 2061, 2066, 2048, 2052, 2021,
  2050, 2067, 2090, 2035, 2099, 1888, 2135, 1944, 2106, 1898, 2085, 2033, 2058, 2048, 2051, 2361,
  2068, 2038, 1992, 1940, 2160, 2197, 2093, 2003, 1892, 2086, 2458, 1955, 2033, 2138, 1976, 2039,
  1909, 2574, 1974, 1783, 2023, 2415, 1695, 1721, 1676, 1935, 1936, 2002, 1906, 1681, 2070, 1845,
  2295, 1921, 2297, 1815, 2284, 1694, 2386, 1981, 2572, 2092, 2369, 2104, 2733, 1920, 2248, 2048
};

uint16_t Drum_LUT[NS] = {
  2047, 1396, 1706, 2438, 2174, 2561, 1949, 1513, 2246, 1840, 2152, 2056, 2123, 2033, 2103, 2295,
  1733, 1832, 2644, 2107, 2239, 2299, 1921, 2108, 1985, 2070, 2596, 2451, 1641, 1958, 2239, 1982,
  2043, 2018, 2064, 2035, 2047, 2045, 2034, 1744, 2494, 2417, 2380, 1812, 1547, 2147, 2103, 2103,
  2090, 2081, 2058, 1627, 1844, 1879, 2047, 2070, 2031, 2003, 2026, 2044, 2033, 2045, 2047, 2040,
  2285, 2286, 2070, 2070, 2030, 2049, 2005, 2056, 2058, 2050, 2048, 2046, 3043, 1513, 2408, 1950,
  2007, 2085, 2045, 2071, 2016, 2058, 2032, 2050, 1781, 2084, 2087, 1987, 2045, 2057, 2110, 2187,
  3786, 2701, 2446, 2141, 1844, 4093, 1114, 2130, 1987, 2682, 2266, 1951, 2276, 1868, 2085, 1995,
  2083, 2430, 1981, 2003, 2124, 2062, 2025, 2044, 2063, 2008, 2098, 2053, 2046, 2044, 1939, 2835,
  1502, 2110, 2083, 2053, 2069, 2049, 2058, 2041, 2040, 2045, 4089, 1238, 1862, 2128, 2915, 2382,
  1933, 2250, 2059, 2039, 2051, 2104, 1932, 1602, 1920, 2118, 2016, 2079, 2060, 2073, 2031, 2041,
  2056, 2051, 2051, 0, 2175, 1230, 1396, 2323, 2027, 2327, 2225, 1990, 2134, 1997, 2048, 9,
  2516, 1887, 2310, 1994, 2036, 2033, 2028, 2044, 2048, 2067, 2032, 2027, 2407, 1889, 2113, 2038,
  2049, 2042, 129, 0, 1353, 1766, 2100, 2291, 3331, 3423, 3541, 2229, 1308, 1862, 2123, 1941,
  2263, 1951, 2074, 1983, 2038, 2894, 1162, 1945, 2594, 1914, 2340, 1747, 1869, 2079, 1959, 2064,
  2051, 4087, 1896, 2248, 1687, 2130, 2030, 2080, 2004, 2062, 2047, 2047, 2050, 4016, 3195, 3487,
  2057, 1191, 1546, 2525, 1780, 2244, 2001, 2000, 2001, 2098, 2236, 2083, 2165, 2014, 2039, 2041,
  2060, 2025, 2043, 2051, 2045, 2042, 2640, 1612, 2049, 2073, 2042, 2050, 2022, 2043, 2048, 2040,
  2051, 2048, 2012, 680, 2117, 1633, 2292, 1942, 2111, 2058, 2046, 2062, 2049, 2057, 2066, 2643,
  1911, 2198, 2004, 2058, 2046, 3433, 4093, 1987, 2111, 1268, 1791, 1950, 920, 2375, 2218, 2367,
  1831, 2136, 1936, 1827, 2140, 1966, 2089, 2044, 2320, 2127, 2069, 2088, 2044, 2054, 1993, 2069,
  2139, 2043, 2048, 2045, 1205, 632, 2285, 2056, 1853, 2091, 1978, 2022, 2000, 2051, 2048, 2043,
  2056, 1749, 2524, 2150, 2314, 1851, 1747, 2143, 1715, 2049, 2034, 2113, 2000, 2414, 1953, 2183,
  2079, 2041, 2042, 2045, 2041, 2034, 2040, 2047, 2049, 2043, 955, 0, 1827, 2802, 2076, 2435,
  1689, 1987, 1990, 1964, 2043, 2068, 888, 2314, 1651, 2222, 2059, 2022, 2056, 2012, 2043, 2035,
  2050, 2038, 1959, 2358, 2011, 2029, 2047, 2046, 1999, 578, 2163, 2140, 3309, 2070, 1687, 2047
};

uint16_t nothin_LUT[NS] ={0};

volatile uint16_t *CurrentLUT = Sin_LUT;  // start with sine
volatile uint32_t CurrentLen  = NS;


// TODO: Equation to calculate TIM2_Ticks
uint32_t TIM2_Ticks = 284; // How often to write new LUT value
uint32_t DestAddress = (uint32_t) &(TIM3->CCR3); // Write LUT TO TIM3->CCR3 to modify PWM duty cycle
// Low-level hooks you likely have:
//extern void lcd_write_cmd(uint8_t cmd);
//extern void lcd_putstring(const char *s);

// Set cursor using HD44780 addresses (row 0: 0x00, row 1: 0x40)
static void lcd_goto(uint8_t row, uint8_t col) {
    const uint8_t base[] = {0x00, 0x40};
    lcd_command(0x80 | (base[row] + col)); // 0x80 = Set DDRAM addr
}

static void lcd_write_line(uint8_t row, const char *text) {
    char buf[17];
    uint8_t i = 0;
    while (text[i] && i < 16) { buf[i] = text[i]; i++; }
    while (i < 16) { buf[i++] = ' '; }
    buf[16] = '\0';
    lcd_goto(row, 0);
    lcd_putstring(buf);
}



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void EXTI0_IRQHandler(void);
/* USER CODE BEGIN 0 */
#include <string.h>

/* Waveform selector */
typedef enum {
  WF_SINE = 0,
  WF_SAW,
  WF_TRI,
  WF_PIANO,
  WF_GUITAR,
  WF_DRUM,
  WF_COUNT
} Waveform_t;

volatile Waveform_t g_wave = WF_SINE;

/* Active source for DMA (points at one of the LUTs) */
/*extern uint16_t Sin_LUT[NS];
extern uint16_t Saw_LUT[NS];
extern uint16_t Triangle_LUT[NS];
extern uint16_t Piano_LUT[NS];
extern uint16_t Guitar_LUT[NS];
extern uint16_t Drum_LUT[NS];

volatile uint16_t *CurrentLUT = (volatile uint16_t*)Sin_LUT;
volatile uint32_t  CurrentLen = NS;*/

/* LCD call aliases (adjust to your driver names) */
    /* or lcd_puts(s) */

/* Debounce: returns 1 if accepted */
static int debounce_ok(uint32_t ms)
{
  static uint32_t last = 0;
  uint32_t now = HAL_GetTick();
  if (now - last < ms) return 0;
  last = now;
  return 1;
}

/* Switch LUT + LCD label based on g_wave */
static void select_waveform_and_label(void)
{
  switch (g_wave) {
    case WF_SINE:   CurrentLUT = (uint16_t*)Sin_LUT; lcd_write_line(0, "Sine");    break;
    case WF_SAW:    CurrentLUT = (uint16_t*)Saw_LUT;     lcd_write_line(0, "Sawtooth"); break;
    case WF_TRI:    CurrentLUT = (uint16_t*)Triangle_LUT; lcd_write_line(0, "Triangle");break;
    case WF_PIANO:  CurrentLUT = (uint16_t*)Piano_LUT;    lcd_write_line(0, "Piano");    break;
    case WF_GUITAR: CurrentLUT = (uint16_t*)Guitar_LUT;   lcd_write_line(0, "Guitar");   break;
    case WF_DRUM:   CurrentLUT = (uint16_t*)Drum_LUT;     lcd_write_line(0, "Drum");   break;
    default:        g_wave =WF_SINE;                     CurrentLUT = (uint16_t*)Sin_LUT;  lcd_clear_line();lcd_putstring("nothing");  break;
  }
}
/* USER CODE END 0 */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	//lcd_putstring("Sine");

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE BEGIN 2 */
  // 1) Start TIM3 PWM on CH3
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

  // 2) Start TIM2 in Output Compare (OC) mode on CH1
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);

  // 3) Prepare and start DMA in IT mode: source = LUT, dest = TIM3->CCR3
  //    We transfer CurrentLen 32-bit words because CCR is 32-bit; LUT values are widened.
  HAL_StatusTypeDef st = HAL_DMA_Start_IT(
      &hdma_tim2_ch1,
      (uint32_t)CurrentLUT,   // source: LUT base
      DestAddress,            // dest: TIM3->CCR3
      CurrentLen              // number of transfers
  );
  if (st != HAL_OK) { Error_Handler(); }

  // 4) Write the current waveform type to the LCD
  init_LCD();
  //lcd_clear_line();

  lcd_putstring("Sine");


  // 5) Enable TIM2 CC1 DMA request (so each OC event clocks the next sample)
  __HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);
  /* USER CODE END 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIM2_Ticks - 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* TIM2_CH1 DMA Init */
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_tim2_ch1.Instance = DMA1_Stream5;
  hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;         // TIM2_CH1 is on channel 3
  hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH; // Memory -> TIM3->CCR3
  hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;    // Peripheral address fixed
  hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;        // Memory address increments
  hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;            // Repeat LUT automatically
  hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
  {
      Error_Handler();
  }

  /* Link DMA handle to TIM2 handle */
  __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 4095;   // 12-bit PWM range

  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // -------------------------------
  // LCD pins configuration
  // -------------------------------
  // Configure PC14 (RS) and PC15 (E) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure PB8 (D4) and PB9 (D5) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure PA12 (D6) and PA15 (D7) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Set all LCD pins LOW initially
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);


  // -------------------------------
  // Button0 configuration (PA0)
  // -------------------------------
  GPIO_InitStruct.Pin = Button0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Interrupt on rising edge
  GPIO_InitStruct.Pull = GPIO_PULLUP;         // Use pull-up resistor
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Enable and set EXTI line 0 interrupt priority
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void EXTI0_IRQHandler(void)
{
  /* Debounce ~80 ms to ignore bounces and rapid repeats */
  if (!debounce_ok(80)) {
    HAL_GPIO_EXTI_IRQHandler(Button0_Pin);  // still clear EXTI flag
    return;
  }

  /* 1) Stop generating new DMA requests from TIM2 CC1 */
  __HAL_TIM_DISABLE_DMA(&htim2, TIM_DMA_CC1);

  /* 2) Abort the current DMA transfer (IT) */
  HAL_DMA_Abort_IT(&hdma_tim2_ch1);

  /* 3) Advance waveform selection (wrap around) */
  g_wave = (Waveform_t)((g_wave + 1) % WF_COUNT);

  /* 4) Point to new LUT and update LCD */
  select_waveform_and_label();

  /* 5) Restart DMA: source = CurrentLUT, dest = TIM3->CCR3, length = NS
        (DMA is configured CIRCULAR, so it will loop the LUT) */
  if (HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)CurrentLUT, (uint32_t)&(TIM3->CCR3), CurrentLen) != HAL_OK) {
    Error_Handler();
  }

  /* 6) Re-enable TIM2 CC1 DMA requests so compare events feed CCR3 */
  __HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);

  /* 7) Clear EXTI line flag */
  HAL_GPIO_EXTI_IRQHandler(Button0_Pin);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
