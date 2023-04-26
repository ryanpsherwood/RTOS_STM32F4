/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CE_A2_PIC_Pin GPIO_PIN_2
#define CE_A2_PIC_GPIO_Port GPIOE
#define CE_A3_PIC_Pin GPIO_PIN_3
#define CE_A3_PIC_GPIO_Port GPIOE
#define IN6_SPC_I_Pin GPIO_PIN_4
#define IN6_SPC_I_GPIO_Port GPIOE
#define IN7_SPC_I_Pin GPIO_PIN_5
#define IN7_SPC_I_GPIO_Port GPIOE
#define IN8_SPC_I_Pin GPIO_PIN_6
#define IN8_SPC_I_GPIO_Port GPIOE
#define SDC_DETECT_Pin GPIO_PIN_13
#define SDC_DETECT_GPIO_Port GPIOC
#define BOOT0_SPA_SPB_Pin GPIO_PIN_14
#define BOOT0_SPA_SPB_GPIO_Port GPIOC
#define RST_SPA_SPB_CE_Pin GPIO_PIN_15
#define RST_SPA_SPB_CE_GPIO_Port GPIOC
#define DOF_SPC_Pin GPIO_PIN_0
#define DOF_SPC_GPIO_Port GPIOC
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define DCF_SPC_Pin GPIO_PIN_2
#define DCF_SPC_GPIO_Port GPIOC
#define NDF_SPC_Pin GPIO_PIN_3
#define NDF_SPC_GPIO_Port GPIOC
#define PA0_24V_CE_Pin GPIO_PIN_0
#define PA0_24V_CE_GPIO_Port GPIOA
#define ETH_CLK_Pin GPIO_PIN_1
#define ETH_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define MBRE_PLD_I_Pin GPIO_PIN_3
#define MBRE_PLD_I_GPIO_Port GPIOA
#define MEB2_PLD_I_Pin GPIO_PIN_4
#define MEB2_PLD_I_GPIO_Port GPIOA
#define MEB1_PLD_I_Pin GPIO_PIN_5
#define MEB1_PLD_I_GPIO_Port GPIOA
#define MPME_PLD_I_Pin GPIO_PIN_6
#define MPME_PLD_I_GPIO_Port GPIOA
#define RMII_CRS_Pin GPIO_PIN_7
#define RMII_CRS_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define PLD_PRG_Pin GPIO_PIN_0
#define PLD_PRG_GPIO_Port GPIOB
#define SNN087_Pin GPIO_PIN_1
#define SNN087_GPIO_Port GPIOB
#define SPC_ON_DS6_Pin GPIO_PIN_2
#define SPC_ON_DS6_GPIO_Port GPIOB
#define SNN095_Pin GPIO_PIN_7
#define SNN095_GPIO_Port GPIOE
#define IO1_SPC_I_Pin GPIO_PIN_8
#define IO1_SPC_I_GPIO_Port GPIOE
#define IO2_SPC_I_Pin GPIO_PIN_9
#define IO2_SPC_I_GPIO_Port GPIOE
#define IO3_SPC_I_Pin GPIO_PIN_10
#define IO3_SPC_I_GPIO_Port GPIOE
#define IO4_SPC_I_Pin GPIO_PIN_11
#define IO4_SPC_I_GPIO_Port GPIOE
#define IO5_SPC_I_Pin GPIO_PIN_12
#define IO5_SPC_I_GPIO_Port GPIOE
#define IO6_SPC_I_Pin GPIO_PIN_13
#define IO6_SPC_I_GPIO_Port GPIOE
#define IO7_SPC_I_Pin GPIO_PIN_14
#define IO7_SPC_I_GPIO_Port GPIOE
#define IO8_SPC_I_Pin GPIO_PIN_15
#define IO8_SPC_I_GPIO_Port GPIOE
#define CSC_REV2_Pin GPIO_PIN_10
#define CSC_REV2_GPIO_Port GPIOB
#define RMII_TXEN_Pin GPIO_PIN_11
#define RMII_TXEN_GPIO_Port GPIOB
#define RMII_TXD0_Pin GPIO_PIN_12
#define RMII_TXD0_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define CSC_REV1_Pin GPIO_PIN_14
#define CSC_REV1_GPIO_Port GPIOB
#define CSC_REV0_Pin GPIO_PIN_15
#define CSC_REV0_GPIO_Port GPIOB
#define USART3_SPA_MON_TX_Pin GPIO_PIN_8
#define USART3_SPA_MON_TX_GPIO_Port GPIOD
#define USART3_SPA_MON_RX_Pin GPIO_PIN_9
#define USART3_SPA_MON_RX_GPIO_Port GPIOD
#define CE_BUS_MODE_IO_1_Pin GPIO_PIN_10
#define CE_BUS_MODE_IO_1_GPIO_Port GPIOD
#define CE_WATCH_DOG_Pin GPIO_PIN_11
#define CE_WATCH_DOG_GPIO_Port GPIOD
#define CE_SCK_Pin GPIO_PIN_12
#define CE_SCK_GPIO_Port GPIOD
#define CE_D0_Pin GPIO_PIN_13
#define CE_D0_GPIO_Port GPIOD
#define CE_WR_SPC_PLD_Pin GPIO_PIN_14
#define CE_WR_SPC_PLD_GPIO_Port GPIOD
#define CE_SDI_Pin GPIO_PIN_15
#define CE_SDI_GPIO_Port GPIOD
#define USART6_MON_SPB_TX_Pin GPIO_PIN_6
#define USART6_MON_SPB_TX_GPIO_Port GPIOC
#define USART6_MON_SPB_RX_Pin GPIO_PIN_7
#define USART6_MON_SPB_RX_GPIO_Port GPIOC
#define SDC_D0_Pin GPIO_PIN_8
#define SDC_D0_GPIO_Port GPIOC
#define SDC_D1_Pin GPIO_PIN_9
#define SDC_D1_GPIO_Port GPIOC
#define USB_PWR_EN_Pin GPIO_PIN_8
#define USB_PWR_EN_GPIO_Port GPIOA
#define USB_FLT_Pin GPIO_PIN_9
#define USB_FLT_GPIO_Port GPIOA
#define SNN003_Pin GPIO_PIN_10
#define SNN003_GPIO_Port GPIOA
#define USB_FS_DM_Pin GPIO_PIN_11
#define USB_FS_DM_GPIO_Port GPIOA
#define USB_FS_DP_Pin GPIO_PIN_12
#define USB_FS_DP_GPIO_Port GPIOA
#define JTMS_SPC_Pin GPIO_PIN_13
#define JTMS_SPC_GPIO_Port GPIOA
#define JTCK_SPC_Pin GPIO_PIN_14
#define JTCK_SPC_GPIO_Port GPIOA
#define JRD1_SPC_Pin GPIO_PIN_15
#define JRD1_SPC_GPIO_Port GPIOA
#define SDC_D2_Pin GPIO_PIN_10
#define SDC_D2_GPIO_Port GPIOC
#define SDC_D3_Pin GPIO_PIN_11
#define SDC_D3_GPIO_Port GPIOC
#define SDC_CLK_Pin GPIO_PIN_12
#define SDC_CLK_GPIO_Port GPIOC
#define PD0_BOOT_MODE_Pin GPIO_PIN_0
#define PD0_BOOT_MODE_GPIO_Port GPIOD
#define PD1_BOOT_MODE_Pin GPIO_PIN_1
#define PD1_BOOT_MODE_GPIO_Port GPIOD
#define SDC_CMD_Pin GPIO_PIN_2
#define SDC_CMD_GPIO_Port GPIOD
#define TDO_PLD_Pin GPIO_PIN_3
#define TDO_PLD_GPIO_Port GPIOD
#define TCK_MUXB1_Pin GPIO_PIN_4
#define TCK_MUXB1_GPIO_Port GPIOD
#define TMS_MUXB2_Pin GPIO_PIN_5
#define TMS_MUXB2_GPIO_Port GPIOD
#define TDI_MUXB3_Pin GPIO_PIN_6
#define TDI_MUXB3_GPIO_Port GPIOD
#define CE_SD0_Pin GPIO_PIN_7
#define CE_SD0_GPIO_Port GPIOD
#define JRD0_SPC_Pin GPIO_PIN_3
#define JRD0_SPC_GPIO_Port GPIOB
#define JRST_SPC_Pin GPIO_PIN_4
#define JRST_SPC_GPIO_Port GPIOB
#define CAN2_RX_MON_Pin GPIO_PIN_5
#define CAN2_RX_MON_GPIO_Port GPIOB
#define CAN2_TX_MON_Pin GPIO_PIN_6
#define CAN2_TX_MON_GPIO_Port GPIOB
#define SNN049_Pin GPIO_PIN_7
#define SNN049_GPIO_Port GPIOB
#define CAN1_RX_MON_Pin GPIO_PIN_8
#define CAN1_RX_MON_GPIO_Port GPIOB
#define CAN1_TX_MON_Pin GPIO_PIN_9
#define CAN1_TX_MON_GPIO_Port GPIOB
#define CE_A0_PIC_Pin GPIO_PIN_0
#define CE_A0_PIC_GPIO_Port GPIOE
#define CE_A1_PIC_Pin GPIO_PIN_1
#define CE_A1_PIC_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
