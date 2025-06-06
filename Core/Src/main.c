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
#include "can.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "neo.h"
#include "string.h"
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
uint8_t pollPVT[8] = {0xB5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x08, 0x19};
uint8_t txData[100];
uint8_t rxData[100];
uint16_t len = 100;

uint8_t dummyTx[1000];
uint8_t dummyRx[1000];
uint16_t dummyLen = 1000;
float coords[2];
int status;

CAN_TxHeaderTypeDef NE_Vel_CTXHeader;
HAL_StatusTypeDef NE_Vel_Status;

CAN_TxHeaderTypeDef DHeading_CTXHeader;
HAL_StatusTypeDef DHeading_Status;

#define NE_Vel_ID 0x771;
#define DHeading_ID 0x781;

uint8_t NE_Vel_Tx[8];
uint8_t DHeading_Tx[8];

uint32_t mailbox;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void formNE_Vel_Frame();
void formDHeading_Frame();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void formNE_Vel_Frame() {
	// North Velocity
	NE_Vel_Tx[0] = rxData[54];
	NE_Vel_Tx[1] = rxData[55];
	NE_Vel_Tx[2] = rxData[56];
	NE_Vel_Tx[3] = rxData[57];

	// East Velocity
	NE_Vel_Tx[4] = rxData[58];
	NE_Vel_Tx[5] = rxData[59];
	NE_Vel_Tx[6] = rxData[60];
	NE_Vel_Tx[7] = rxData[61];
}

void formDHeading_Frame() {
	// Down Velocity
	DHeading_Tx[0] = rxData[62];
	DHeading_Tx[1] = rxData[63];
	DHeading_Tx[2] = rxData[64];
	DHeading_Tx[3] = rxData[65];

	// Heading Direction (need to scale by 1e-5)
	DHeading_Tx[4] = rxData[70];
	DHeading_Tx[5] = rxData[71];
	DHeading_Tx[6] = rxData[72];
	DHeading_Tx[7] = rxData[73];
}

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
  MX_SPI3_Init();
  MX_CAN1_Init();

  HAL_CAN_Start(&hcan1);
  /* USER CODE BEGIN 2 */

  memset(txData, 0xFF, len);
  memset(dummyTx, 0xFF, dummyLen);

  NE_Vel_CTXHeader.IDE = CAN_ID_STD;
  NE_Vel_CTXHeader.StdId = NE_Vel_ID;
  NE_Vel_CTXHeader.RTR = CAN_RTR_DATA;
  NE_Vel_CTXHeader.DLC = 8;

  DHeading_CTXHeader.IDE = CAN_ID_STD;
  DHeading_CTXHeader.StdId = DHeading_ID;
  DHeading_CTXHeader.RTR = CAN_RTR_DATA;
  DHeading_CTXHeader.DLC = 8;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi3, pollPVT, 8, HAL_MAX_DELAY);
	  HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_SET);

	  HAL_Delay(100);

	  HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive(&hspi3, txData, rxData, len, HAL_MAX_DELAY);
	  HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_SET);

	  status = PVT_PARSE(rxData, coords);

	  if (status == 1) {
		  formNE_Vel_Frame();
		  formDHeading_Frame();

		  NE_Vel_Status = HAL_CAN_AddTxMessage(&hcan1, &NE_Vel_CTXHeader, NE_Vel_Tx, &mailbox);
		  DHeading_Status = HAL_CAN_AddTxMessage(&hcan1, &DHeading_CTXHeader, DHeading_Tx, &mailbox);
	  }

	  HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive(&hspi3, dummyTx, dummyRx, 1000, HAL_MAX_DELAY);
	  HAL_GPIO_WritePin(NEO_CS_PORT, NEO_CS_PIN, GPIO_PIN_SET);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_7;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
