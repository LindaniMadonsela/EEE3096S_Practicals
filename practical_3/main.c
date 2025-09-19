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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//TODO: Define variables you think you might need
// - Performance timing variables (e.g execution time, throughput, pixels per second, clock cycles)
uint64_t checksum = 0;
uint32_t start_time = 0;
uint32_t end_time = 0;
uint32_t execution_time;
uint32_t size = 1920;  // width and height of the image
uint32_t max_iter[5] = {100, 250, 500, 750, 1000}; // different iteration sizes
uint32_t cycles = 0;
float throughput = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
//TODO: Define any function prototypes you might need such as the calculate Mandelbrot function among others
uint64_t calculate_mandelbrot_fixed_point_arithmetic(int width, int height, int max_iterations);
uint64_t calculate_mandelbrot_double(int width, int height, int max_iterations);
static inline void DWT_Init(void);
uint64_t mandelbrot_tile_fixed(int W, int H, int x0, int x1, int y0, int y1, int max_iterations);
uint64_t mandelbrot_tile_double(int W, int H, int x0, int x1, int y0, int y1, int max_iterations);

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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // --- MANUAL SETTINGS ---
      int W = 128;   // change this manually (128, 160, 192, 224, 256, etc.)
      int H = 128;   // change this manually
      const int MAX_ITER = 100;

      // Choose which Mandelbrot implementation to test:
      // 1 = double, 2 = fixed-point
      int mode = 2;

      // --- BENCHMARK START ---
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // LED0 = start

      start_time = HAL_GetTick();
      DWT_Init(); // works on F4, ignored on F0
      uint32_t start_cycles = DWT->CYCCNT;

      uint64_t total_checksum = 0;

      if (mode == 1) {
          // Double-precision version
          if (W <= 256 && H <= 256) {
              total_checksum = calculate_mandelbrot_double(W, H, MAX_ITER);
          } else {
              total_checksum = mandelbrot_tile_double(W, H, 0, W, 0, H, MAX_ITER);
          }
      }
      else if (mode == 2) {
          // Fixed-point version
          if (W <= 256 && H <= 256) {
              total_checksum = calculate_mandelbrot_fixed_point_arithmetic(W, H, MAX_ITER);
          } else {
              total_checksum = mandelbrot_tile_fixed(W, H, 0, W, 0, H, MAX_ITER);
          }
      }

      end_time = HAL_GetTick();
      uint32_t end_cycles = DWT->CYCCNT;

      execution_time = end_time - start_time;
      cycles = (end_cycles > start_cycles) ? (end_cycles - start_cycles) : 0;
      throughput = (float)(W * H) / (execution_time / 1000.0f);
      checksum = total_checksum;
      size = W;

      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // LED1 = done
      HAL_Delay(1000);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

      // BREAKPOINT: record results -> (W, H, mode, execution_time, cycles, throughput, checksum)

      break; // stop after one run
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//TODO: Function signatures you defined previously , implement them here
//TODO: Mandelbroat using variable type integers and fixed point arithmetic
uint64_t calculate_mandelbrot_fixed_point_arithmetic(int width, int height, int max_iterations){
    const int64_t SCALE = 1000000LL; // scale factor for fixed point
    const int64_t SCALE_2P0 = 2000000LL;  // 2.0 * SCALE
    const int64_t SCALE_3P5 = 3500000LL;  // 3.5 * SCALE
    const int64_t SCALE_2P5 = 2500000LL;  // 2.5 * SCALE
    const int64_t SCALE_4P0 = 4000000LL;  // 4.0 * SCALE
    uint64_t mandelbrot_sum = 0;
    //TODO: Complete the function implementation
    for (int y = 0; y < height; y++) {
            int64_t y0 = ((int64_t)y * SCALE_2P0) / height - SCALE; // (y/height)*2.0 - 1.0
            for (int x = 0; x < width; x++) {
                int64_t x0 = ((int64_t)x * SCALE_3P5) / width - SCALE_2P5;

                int64_t xi = 0;
                int64_t yi = 0;
                int iteration = 0;

                while (iteration < max_iterations) {
                    int64_t xi_sq = (xi * xi) / SCALE;
                    int64_t yi_sq = (yi * yi) / SCALE;

                    if ((xi_sq + yi_sq) > SCALE_4P0) break;

                    int64_t xtemp = xi_sq - yi_sq;
                    yi = ((2 * xi * yi) / SCALE) + y0;
                    xi = xtemp + x0;

                    iteration++;
                }

                mandelbrot_sum += iteration;
            }
        }
    return mandelbrot_sum;

}

//TODO: Mandelbroat using variable type double
uint64_t calculate_mandelbrot_double(int width, int height, int max_iterations){
    uint64_t mandelbrot_sum = 0;
    //TODO: Complete the function implementation
    for (int y = 0; y < height; y++) {
            double y0 = ((double)y * 2.0 / (double)height) - 1.0;
            for (int x = 0; x < width; x++) {
                double x0 = ((double)x * 3.5 / (double)width) - 2.5;

                double xi = 0.0;
                double yi = 0.0;
                int iteration = 0;

                while (iteration < max_iterations && (xi * xi + yi * yi <= 4.0)) {
                    double xtemp = xi * xi - yi * yi;
                    yi = 2.0 * xi * yi + y0;
                    xi = xtemp + x0;
                    iteration++;
                }

                mandelbrot_sum += iteration;
            }
        }
    return mandelbrot_sum;
}

// Mandelbrot for a sub-tile (fixed-point arithmetic)
uint64_t mandelbrot_tile_fixed(int W, int H, int x0, int x1, int y0, int y1, int max_iterations) {
    const int64_t SCALE = 1000000LL;
    const int64_t SCALE_4P0 = 4000000LL;  // 4.0 * SCALE
    uint64_t sum = 0;

    for (int py = y0; py < y1; py++) {
        int64_t y_coord = ((int64_t)py * 2000000LL) / H - 1000000LL; // (py/H)*2 - 1
        for (int px = x0; px < x1; px++) {
            int64_t x_coord = ((int64_t)px * 3500000LL) / W - 2500000LL; // (px/W)*3.5 - 2.5
            int64_t xi = 0, yi = 0;
            int iter = 0;

            while (iter < max_iterations) {
                int64_t xi_sq = (xi * xi) / SCALE;
                int64_t yi_sq = (yi * yi) / SCALE;

                if (xi_sq + yi_sq > SCALE_4P0) break;

                int64_t xt = xi_sq - yi_sq + x_coord;
                yi = ((2 * xi * yi) / SCALE) + y_coord;
                xi = xt;

                iter++;
            }
            sum += iter;
        }
    }
    return sum;
}

// Mandelbrot for a sub-tile (used in scalability when W,H are large)
uint64_t mandelbrot_tile_double(int W, int H, int x0, int x1, int y0, int y1, int max_iterations) {
    uint64_t sum = 0;
    for (int py = y0; py < y1; py++) {
        double y_coord = ((double)py * 2.0 / (double)H) - 1.0;
        for (int px = x0; px < x1; px++) {
            double x_coord = ((double)px * 3.5 / (double)W) - 2.5;
            double xi = 0.0, yi = 0.0;
            int iter = 0;
            while (iter < max_iterations && (xi*xi + yi*yi <= 4.0)) {
                double xt = xi*xi - yi*yi + x_coord;
                yi = 2.0*xi*yi + y_coord;
                xi = xt;
                iter++;
            }
            sum += iter;
        }
    }
    return sum;
}


// Function to init DWT cycle counter
static inline void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // enable DWT
    DWT->CYCCNT = 0;                                // reset counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // enable counter
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
