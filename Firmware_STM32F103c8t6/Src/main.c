/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_customhid.h"
#include "delay.h"

/* USER CODE END Includes */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
/* change usbd_customhid.h as:
	#define  CUSTOM_HID_EPIN_SIZE  	0x40U	//0x02U
	#define CUSTOM_HID_EPOUT_SIZE  	0x40U	//0x02U
	#define USB_CUSTOM_HID_DESC_SIZ   33U		//9U
 	 also, the description hid service might need modification which
	is located in file usbd_custom_hid_if.c named CUSTOM_HID_ReportDesc_FS
*/
extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t USB_RX_Buffer[64];
uint8_t USB_TX_Buffer[64]; //to send USB data to PC
volatile uint8_t Flag_Rcvd_Data_HID = 0;
void	DDS_Set(uint16_t value);
void 	VGA_Set(uint16_t Gain);
void DDS_Freq_Set(uint32_t Freq);
uint32_t	DDS_Freqs[100];
uint32_t	Freq_Times[100];
uint32_t	Freq_time;
uint16_t 	Gains[100];
uint16_t	Freqs_numbers;
uint16_t 	Flag_end_Transaction;
uint8_t 	host_preamble[10] = 		{'S', 'i', 'G', 'n', 'V', '1', 'H', 'o', 's', 't'};
uint8_t	 	device_preamble_OK[10] = 	{'S', 'i', 'G', 'n', 'V', '1', 'D', 'e', 'O', 'K'};
uint8_t 	device_preamble_Error[10] = {'S', 'i', 'G', 'n', 'V', '1', 'D', 'e', 'E', 'r'};
uint16_t 	HostMsgVerified;
uint16_t 	Index = 0;
uint16_t 	k = 0;
uint16_t 	verifier = 0;
uint16_t 	index_checker[100];
uint16_t 	index_verifier = 0;

/* USER CODE END PFP */

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  if(DWT_Delay_Init())
   {
   Error_Handler(); /* Call Error Handler */
   }
  /* USER CODE END 2 */

	HAL_Delay(50);
	DDS_Set(0x2338); //initialize DDS
	HAL_Delay(10);
	Flag_end_Transaction = 0;
	Flag_Rcvd_Data_HID = 0;
	Index = 0;
	k = 0;
	verifier = 0;
	HostMsgVerified = 0;
	DDS_FSEL_GPIO_Port -> BRR = DDS_FSEL_Pin;
  while (1)
  {
	  /*
	   * the incoming message is received through fucntion CUSTOM_HID_OutEvent_FS
	   * in file usbd_custom_hid_if.c. once the packaged is received the flag Flag_Rcvd_Data_HID
	   * is activated.
	   */
		if (Flag_Rcvd_Data_HID == 1)
		{
			Flag_Rcvd_Data_HID = 0;
			k = 0;
			// find the zero index of received packet:
			if (USB_RX_Buffer[0] != host_preamble[0])
			{
				for (int i = 0; i < 63; i++)
				{
					USB_RX_Buffer[i] = USB_RX_Buffer[i+1];
				}
			}

			// verify the incoming message from host:
			verifier = 0;
			HostMsgVerified = 0;
			for (int i = 0; i < 10; i++)
			{
				if (host_preamble[i] == USB_RX_Buffer[k++])
				{verifier++;}
			}
			//
			Index = USB_RX_Buffer[k++];			// index of current received package which conveys the data of a single freq
			Freqs_numbers = USB_RX_Buffer[k++]; // Total number of freqs which are supposed to be received
			//
			if ((verifier == 10) && (Index < 100) && (Freqs_numbers < 101))
			{	// if everything is ok, parse the received package:
				HostMsgVerified = 1;
				index_checker[Index] = Index;
				DDS_Freqs[Index] = USB_RX_Buffer[k++];
				DDS_Freqs[Index] += (USB_RX_Buffer[k++] << 8);
				DDS_Freqs[Index] += (USB_RX_Buffer[k++] << 16);
				DDS_Freqs[Index] += (USB_RX_Buffer[k++] << 24);
				Gains[Index] = USB_RX_Buffer[k++];
				Freq_Times[Index] = USB_RX_Buffer[k++];
				Freq_Times[Index] += (USB_RX_Buffer[k++] << 8);
				Freq_Times[Index] += (USB_RX_Buffer[k++] << 16);
				Freq_Times[Index] += (USB_RX_Buffer[k++] << 24);
			}
			// preparing the sending package to the host:
			if (HostMsgVerified == 1)
			{
				for (uint8_t i = 0; i < 10; i++)
				{
					USB_TX_Buffer[i] = device_preamble_OK[i];
				}
			}
			else
			{
				for (uint8_t i = 0; i < 10; i++)
				{
					USB_TX_Buffer[i] = device_preamble_Error[i];
				}
			}
			for (uint8_t i = 10; i < 64; i++)
			{
				// echo back the the received package for further verification on the host side:
				USB_TX_Buffer[i] = USB_RX_Buffer[i];
			}

			//
			LED1_GPIO_Port -> BSRR = LED1_Pin;
			// calling the HID sending function:
			USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,USB_TX_Buffer,64);
			LED1_GPIO_Port -> BRR = LED1_Pin;
			for (uint8_t i = 0; i < 64; i++)
			{
				USB_TX_Buffer[i] = 0;
			}
			if ((Index >= (Freqs_numbers - 1)) && (HostMsgVerified == 1))
			{
				index_verifier = 0;
				for(uint16_t i = 0; i < Freqs_numbers; i++)
				{
					if (index_checker[i] == i)
					{
						index_verifier++;
					}
				}
				if(index_verifier == Freqs_numbers)
				{
					for(uint16_t i = 0; i < Freqs_numbers; i++)
					{
						index_checker[i] = 0;
					}
					Flag_end_Transaction = 1;
				}
				else
				{Flag_end_Transaction = 0;}

			}
			else
			{	Flag_end_Transaction = 0;}
	}
	//
	if (Flag_end_Transaction == 1)
	{
		for (uint16_t idx = 0; idx < Freqs_numbers; idx++)
		{
			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
			//LED1_GPIO_Port -> BRR = LED1_Pin;

			VGA_PWUP_GPIO_Port -> BRR = VGA_PWUP_Pin; //reset
			DDS_Freq_Set(DDS_Freqs[idx]);
			VGA_Set(Gains[idx]);
			VGA_PWUP_GPIO_Port -> BSRR = VGA_PWUP_Pin; //set
			Freq_time = Freq_Times[idx];

			while(Freq_time > 50000)
			{
				Freq_time -= 50000;
				DWT_Delay_us(50000);
				if(Flag_Rcvd_Data_HID == 1)
					break;
			}
			if ((Freq_time > 0) && (Flag_Rcvd_Data_HID == 0)){
				DWT_Delay_us(Freq_time);
			}

			VGA_PWUP_GPIO_Port -> BRR = VGA_PWUP_Pin; //reset
		}

	}

  }
}
//-----------------------------------------------------
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
//-----------------------------
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA2 PA4
                           PA5 PA6 PA8 PA9
                           PA10 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4
                       |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9
					   |GPIO_PIN_10|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB3 PB4 PB5
                          PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                       |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Din_Pin */
  GPIO_InitStruct.Pin = Din_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Din_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SCL1_Pin */
  GPIO_InitStruct.Pin = SCL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SCL1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SDA1_Pin */
  GPIO_InitStruct.Pin = SDA1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SDA1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DDS_SCLK_Pin */
  GPIO_InitStruct.Pin = DDS_SCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DDS_SCLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DDS_DATA_Pin */
  GPIO_InitStruct.Pin = DDS_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DDS_DATA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DDS_RST_Pin */
  GPIO_InitStruct.Pin = DDS_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DDS_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DDS_FSYNC_Pin */
  GPIO_InitStruct.Pin = DDS_FSYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DDS_FSYNC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DDS_FSEL_Pin */
  GPIO_InitStruct.Pin = DDS_FSEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DDS_FSEL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VGA_PWUP_Pin */
  GPIO_InitStruct.Pin = VGA_PWUP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VGA_PWUP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VGA_DAT_Pin */
  GPIO_InitStruct.Pin = VGA_DAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VGA_DAT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VGA_CLK_Pin */
  GPIO_InitStruct.Pin = VGA_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VGA_CLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VGA_LATCH_Pin */
  GPIO_InitStruct.Pin = VGA_LATCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VGA_LATCH_GPIO_Port, &GPIO_InitStruct);

}
///////////////////////////////////////
void DDS_Freq_Set(uint32_t Freq)
	{
		uint16_t DDS_Value;
		uint32_t DDS_freq_Reg;
		DDS_freq_Reg = (uint32_t)(Freq * 5.36871);
		DDS_Value = (uint16_t) DDS_freq_Reg;
		DDS_Value &= 0x3FFF;
		DDS_Value	|= 0x4000;
		DDS_Set(DDS_Value);
		DDS_Value = (uint16_t) (DDS_freq_Reg >> 14);
		DDS_Value &= 0x3FFF;
		DDS_Value	|= 0x4000;
		DDS_Set(DDS_Value);
	}
//////////////////////////////////////
void	DDS_Set(uint16_t value)
{
	DDS_FSYNC_GPIO_Port -> BSRR = DDS_FSYNC_Pin; //Set
	DDS_SCLK_GPIO_Port -> BSRR = DDS_SCLK_Pin; // Set
	DDS_FSYNC_GPIO_Port -> BRR = DDS_FSYNC_Pin; //Reset

	for(uint16_t i = 0; i < 16; i++)
	{
		if (value & (0x8000 >> i))
		{
			DDS_DATA_GPIO_Port -> BSRR = DDS_DATA_Pin; //Set
		}
		else
		{
		  DDS_DATA_GPIO_Port -> BRR = DDS_DATA_Pin; //Reset
		}
		 __asm(" NOP");
		DDS_SCLK_GPIO_Port -> BRR = DDS_SCLK_Pin; // Reset
  	DDS_SCLK_GPIO_Port -> BSRR = DDS_SCLK_Pin; // Set
	}
   __asm(" NOP");
	DDS_FSYNC_GPIO_Port -> BSRR = DDS_FSYNC_Pin; //Set

}
////////////////////////////////////////////
void 	VGA_Set(uint16_t Gain)
{
  VGA_CLK_GPIO_Port -> BRR = VGA_CLK_Pin; //Reset
	__ASM("NOP");
	VGA_LATCH_GPIO_Port -> BRR = VGA_LATCH_Pin; // Reset
	__ASM("NOP");
    //
	for (uint16_t i = 0; i < 8; i++)
	{
			if (Gain & (0x0080 >> i))
			{
				VGA_DAT_GPIO_Port -> BSRR = VGA_DAT_Pin; //Set
			}
			else
			{
				VGA_DAT_GPIO_Port -> BRR = VGA_DAT_Pin; //Reset
			}
		__ASM("NOP");
		VGA_CLK_GPIO_Port -> BSRR = VGA_CLK_Pin; //Set
	__ASM("NOP");
		VGA_CLK_GPIO_Port -> BRR = VGA_CLK_Pin; //Reset

  }
		__ASM("NOP");
		__ASM("NOP");
		VGA_LATCH_GPIO_Port -> BSRR = VGA_LATCH_Pin; // set
		VGA_DAT_GPIO_Port -> BRR = VGA_DAT_Pin; //Reset
}
/////////////////////////////////////////

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
