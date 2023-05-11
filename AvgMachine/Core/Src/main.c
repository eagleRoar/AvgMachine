/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "fatfs.h"
#include "stdio.h"
#include <string.h>

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
RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd;

UART_HandleTypeDef huart6;

RTC_DateTypeDef GetData;  //获取日期结构体
RTC_TimeTypeDef GetTime;   //获取时间结构体
uint8_t ch;
uint8_t ch_r;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//重写这个函数,重定向printf函数到串口
int fputc(int c, FILE * f)
{
   ch = c;//存储一个数据
   HAL_UART_Transmit(&huart6,&ch,1,1000);//通过串口发送这一个数据(超时时间1000毫秒)
   return c;//发送完成后返回这个数据
 }
 
//重定向scanf函数到串口 意思就是说接受串口发过来的数据
int fgetc(FILE * F)
{
   HAL_UART_Receive(&huart6,&ch_r,1,0xffff);//通过串口接收数据(超时时间65535毫秒)
   return ch_r;//返回接收到的数据信息
}

int GetFileLength(char* name)
{
    int ret;
    FILINFO buf;

    ret = f_stat(name, &buf);

    if (ret == 0) {
        //LOG_D("%s file size = %d", name, buf.st_size);
        return buf.fsize;
    } else {
        //LOG_E("%s file not found");
        return 0;
    }
}


/* USER CODE END 0 */
	uint32_t wtext[1024 * 5]; /* File write buffer */
static void ReadWriteTest(void)
{
		uint8_t res = 0;
	uint32_t byteswritten;                /* File write counts */
  uint32_t bytesread;                   /* File read counts */

  char filename[] = "test.bin";//"STM32cube.txt";
  char SensorBuff[100];
	static uint32_t timeNow = 0;
	static uint32_t timeNow1 = 0;
	int size = 0;
	
	//printf("sd path = %s\r\n",SDPath);
	memset((uint8_t *)wtext,0,sizeof(wtext));
	
  printf("********* STM32CubeMX FatFs Example *********\r\n\r\n");
	//1.文件挂载	
	res = f_mount(&SDFatFS,SDPath,1); 	
	
	//2.文件写入
//  if(res == FR_OK)
//	{
//      printf("f_mount sucess!!! \r\n");
//			if(f_open(&SDFile,filename,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
//			{
//					printf("f_open file sucess!!! \r\n");
//					HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
//					timeNow =  GetTime.Hours * 3600 + GetTime.Minutes * 60 + GetTime.Seconds;
//					printf("start writing--------------------------------\r\n");
//					size = 1024 * 40 * 1024 / sizeof(wtext);
//					printf("size========================%d\r\n",size);
//					for(int i = 0; i < size; i++)
//					{
//						wtext[1023] = i;
//						if(f_write(&SDFile,wtext,sizeof(wtext),&byteswritten) == FR_OK)
//						{
//								byteswritten = sizeof(wtext) * i;
//						}
//					}
//					HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
//					printf("end writing,time goes %d--------------------------------\r\n",GetTime.Hours * 3600 + GetTime.Minutes * 60 + GetTime.Seconds - timeNow);
//					
//					//关闭文件
//					if(f_close(&SDFile) == FR_OK)
//					{
//						printf("f_close sucess!!! \r\n");
//					}
//					else
//					{
//						printf("f_close error : %d\r\n",retSD);
//					}
//			}
//			else
//			{
//				printf("f_open file error\r\n");  
//			}
//	}
//	else
//	{
//		printf("f_mount error : %d \r\n",res);
//	}

		//3.文件读取 速度> 8m/s
		retSD = f_open(&SDFile, filename, FA_READ | FA_OPEN_EXISTING);
    if(retSD)
		{
        printf("f_open file error : %d\r\n",retSD);
		}
    else
		{
        printf("f_open file sucess!!! \r\n");
		}

		HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
		timeNow =  GetTime.Hours * 3600 + GetTime.Minutes * 60 + GetTime.Seconds;
		printf("%2d:%2d:%2d\r\n",GetTime.Hours,GetTime.Minutes,GetTime.Seconds);
		//读取当前文件的长度
		int fileSize = GetFileLength(filename);
		//printf("file length = %d",fileSize);
		size = fileSize / sizeof(wtext);
		printf("start reading--------------------------------, timenow = %d, size = %d\r\n", timeNow, size);
		for(int i = 0; i < size; i++)
		{
			retSD = f_read(&SDFile, wtext, sizeof(wtext), (UINT*)&bytesread);
			bytesread = sizeof(wtext) * i;
			//printf("%d\t",wtext[1023]);
		}
		HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
		timeNow1 =  GetTime.Hours * 3600 + GetTime.Minutes * 60 + GetTime.Seconds;
		printf("end reading,time goes %d, timenow = %d--------------------------------\r\n",timeNow1 - timeNow, timeNow1);
		printf("%2d:%2d:%2d\r\n",GetTime.Hours,GetTime.Minutes,GetTime.Seconds);
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	FATFS fs;                       /* FatFs 文件系统对象 */
	FIL file;                       /* 文件对象 */
	FRESULT f_res;                  /* 文件操作结果 */
	UINT fnum;                      /* 文件成功读写数量 */
	BYTE ReadBuffer[1024] = {0};    /* 读缓冲区 */
	BYTE WriteBuffer[] =   "This is STM32 working with FatFs\r\n";/* 写缓冲区 */
	BYTE TestBuf[1024];

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
  MX_RTC_Init();
  MX_SDIO_SD_Init();
  MX_USART6_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);

	HAL_Delay(3000);

	ReadWriteTest();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */
		//HAL_UART_Transmit(&huart6, data, 13, 10);
		
//    HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
//		
//		printf("%2d:%2d:%2d\r\n",GetTime.Hours,GetTime.Minutes,GetTime.Seconds);
		
		
		HAL_Delay(1000);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  //SD 必须拉低才会供电
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
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
