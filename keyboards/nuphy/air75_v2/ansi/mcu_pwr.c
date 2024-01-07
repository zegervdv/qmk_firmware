// Copyright 2023 NuPhy & jincao1
// SPDX-License-Identifier: GPL-2.0-or-later
#include "user_kb.h"
#include "mcu_stm32f0xx.h"
#include "mcu_pwr.h"

//------------------------------------------------
// 外部变量
extern DEV_INFO_STRUCT dev_info;
extern bool            f_dev_sleep_enable;
extern uint16_t        no_act_time;

bool f_usb_deinit = 0;

/** ================================================================
 * @brief   UART_GPIO 翻转速率配置低速+上拉
 ================================================================*/
void m_uart_gpio_set_low_speed_and_pullup(void) {
    ((GPIO_TypeDef *)GPIOB)->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7);
    ((GPIO_TypeDef *)GPIOB)->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0);
}

/** ================================================================
 * @brief   关闭USB
 *
 ================================================================*/
void m_deinit_usb_072(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

#if (0)
    // 调用qmk库关闭USB
    void close_usb(void);
    close_usb();
#endif

    // 复位USB寄存器
    RCC_APB1PeriphResetCmd(RCC_APB1RSTR_USBRST, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1RSTR_USBRST, DISABLE);
    wait_ms(10);

    // 关闭USB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_USBEN, DISABLE);

    // GPIO恢复为悬浮状态
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init((GPIO_TypeDef *)GPIOA, &GPIO_InitStructure);
}

/** ================================================================
 * @brief   低功耗处理
 *
 ================================================================*/
#include "hal_usb.h"
#include "usb_main.h"
void idle_enter_sleep(void);
void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex) {
    uint32_t tmp = 0x00;

    tmp = ((uint32_t)0x0F) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03));
    SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] &= ~tmp;
    SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] |= (((uint32_t)EXTI_PortSourceGPIOx) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03)));
}

#include "hal_lld.h"
#define EXTI_PortSourceGPIOA ((uint8_t)0x00)
#define EXTI_PortSourceGPIOB ((uint8_t)0x01)
#define EXTI_PortSourceGPIOC ((uint8_t)0x02)
#define EXTI_PortSourceGPIOD ((uint8_t)0x03)

/**
 * @brief Wake up from deep sleep
 * @note This is triggered by an interrupt event.
 *       This is mostly Nuphy's unreleased logic with cleanup/refactoring by me.
 */
#include "usb_main.h"
/**
 * @brief  Enter deep sleep
 * @note This is Nuphy's un-released logic with some cleanup/refactoring
 *       The MCU is put on a low power mode.
 */
void enter_deep_sleep(void) {
    //------------------------ 设置RF休眠状态
    if (dev_info.rf_state == RF_CONNECT)
        uart_send_cmd(CMD_SET_CONFIG, 5, 5); // 连接状态设置深度休眠时间
    else
        uart_send_cmd(CMD_SLEEP, 5, 5); // 非连接状态直接进入深度休眠

    //------------------------ 非USB模式下关闭USB
    // TODO - do we really need to deinitialize USB?
    if (dev_info.link_mode != LINK_USB) {
        f_usb_deinit = 1;
        m_deinit_usb_072(); // 关闭USB
    }

    // 关闭定时器
    TIM_Cmd(TIM6, DISABLE);

    //------------------------ 配置按键唤醒
    setPinOutput(KCOL_0);
    writePinHigh(KCOL_0);
    setPinOutput(KCOL_1);
    writePinHigh(KCOL_1);
    setPinOutput(KCOL_2);
    writePinHigh(KCOL_2);
    setPinOutput(KCOL_3);
    writePinHigh(KCOL_3);
    setPinOutput(KCOL_4);
    writePinHigh(KCOL_4);
    setPinOutput(KCOL_5);
    writePinHigh(KCOL_5);
    setPinOutput(KCOL_6);
    writePinHigh(KCOL_6);
    setPinOutput(KCOL_7);
    writePinHigh(KCOL_7);
    setPinOutput(KCOL_8);
    writePinHigh(KCOL_8);
    setPinOutput(KCOL_9);
    writePinHigh(KCOL_9);
    setPinOutput(KCOL_10);
    writePinHigh(KCOL_10);
    setPinOutput(KCOL_11);
    writePinHigh(KCOL_11);
    setPinOutput(KCOL_12);
    writePinHigh(KCOL_12);
    setPinOutput(KCOL_13);
    writePinHigh(KCOL_13);
    setPinOutput(KCOL_14);
    writePinHigh(KCOL_14);
    setPinOutput(KCOL_15);
    writePinHigh(KCOL_15);
    setPinOutput(KCOL_16);
    writePinHigh(KCOL_16);

    setPinInputLow(KROW_0);
    setPinInputLow(KROW_1);
    setPinInputLow(KROW_2);
    setPinInputLow(KROW_3);
    setPinInputLow(KROW_4);
    setPinInputLow(KROW_5);

    SYSCFG_EXTILineConfig(EXTI_PORT_R0, EXTI_PIN_R0);
    SYSCFG_EXTILineConfig(EXTI_PORT_R1, EXTI_PIN_R1);
    SYSCFG_EXTILineConfig(EXTI_PORT_R2, EXTI_PIN_R2);
    SYSCFG_EXTILineConfig(EXTI_PORT_R3, EXTI_PIN_R3);
    SYSCFG_EXTILineConfig(EXTI_PORT_R4, EXTI_PIN_R4);
    SYSCFG_EXTILineConfig(EXTI_PORT_R5, EXTI_PIN_R5);

    EXTI_InitTypeDef m_exti;
    EXTI_StructInit(&m_exti);
    m_exti.EXTI_Line    = 0XFFFF; // GPIO 0-15
    m_exti.EXTI_LineCmd = ENABLE;
    m_exti.EXTI_Mode    = EXTI_Mode_Interrupt;
    m_exti.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&m_exti);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel         = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    // power off leds
    setPinOutput(DC_BOOST_PIN);
    writePinLow(DC_BOOST_PIN);

    setPinInput(DRIVER_LED_CS_PIN);
    setPinInput(DRIVER_SIDE_CS_PIN);

    setPinOutput(DEV_MODE_PIN);
    writePinLow(DEV_MODE_PIN);

    setPinOutput(SYS_MODE_PIN);
    writePinLow(SYS_MODE_PIN);

    setPinOutput(A7);
    writePinLow(A7);
    setPinOutput(DRIVER_SIDE_PIN);
    writePinLow(DRIVER_SIDE_PIN);

    setPinOutput(B5);
    writePinHigh(B5);

    setPinOutput(NRF_WAKEUP_PIN);
    writePinHigh(NRF_WAKEUP_PIN);

    // Enter low power mode and wait for interrupt signal
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

void exit_deep_sleep(void) {
    // 矩阵初始化
    extern void matrix_init_pins(void);
    matrix_init_pins();

    m_uart_gpio_set_low_speed_and_pullup();

    // 恢复IO工作状态
    setPinInputHigh(DEV_MODE_PIN); // PC0
    setPinInputHigh(SYS_MODE_PIN); // PC1

    setPinOutput(NRF_WAKEUP_PIN);

    // 使能DC升压
    setPinOutput(DC_BOOST_PIN);
    writePinHigh(DC_BOOST_PIN);

    // power on LEDs This is missing from Nuphy's logic.
    setPinOutput(DRIVER_LED_CS_PIN);
    writePinLow(DRIVER_LED_CS_PIN);
    setPinOutput(DRIVER_SIDE_CS_PIN);
    writePinLow(DRIVER_SIDE_CS_PIN);

    // 重新初始化系统时钟
    stm32_clock_init();

    /* TIM6 使能 */
    TIM_Cmd(TIM6, ENABLE);

    // 发送一个握手唤醒RF
    uart_send_cmd(CMD_HAND, 0, 1); // 握手

    no_act_time = 0;

    // Scan and update software state.
    // These are the same functions in ansi.c, call it here to determine board state
    uart_receive_pro();
    dev_sts_sync();
    dial_sw_scan();

    // Should re-init USB regardless probably if it was deinitialized.
    if (f_usb_deinit) {
        // USB远程唤醒
        usbWakeupHost(&USB_DRIVER);
        restart_usb_driver(&USB_DRIVER);
        f_usb_deinit = 0;
    }

    // Call the QMK keyboard life cycle to preserve first pressed button on wakeup.
    // If the matrix scan somehow catches the key press it should send it.
    keyboard_task();
    break_all_key(); // clear the key after. This probably causes 2 keystrokes but not a big deal for me.
}

/**
 * @brief  Light sleep by powering off LEDs.
 * @note This is Nuphy's "open sourced" sleep logic. It's not deep sleep.
 */
void enter_light_sleep(void) {
    if (dev_info.rf_state == RF_CONNECT)
        uart_send_cmd(CMD_SET_CONFIG, 5, 5);
    else
        uart_send_cmd(CMD_SLEEP, 5, 5);

    // power off led
    setPinOutput(DC_BOOST_PIN);
    writePinLow(DC_BOOST_PIN);

    setPinInput(DRIVER_LED_CS_PIN);
    setPinInput(DRIVER_SIDE_CS_PIN);
}

/**
 * @brief  Power back up LEDs on exiting light sleep.
 * @note This is Nuphy's "open sourced" wake logic. It's not deep sleep.
 */
void exit_light_sleep(void) {
    setPinOutput(DC_BOOST_PIN);
    writePinHigh(DC_BOOST_PIN);

    // power on LEDs
    setPinOutput(DRIVER_LED_CS_PIN);
    writePinLow(DRIVER_LED_CS_PIN);
    setPinOutput(DRIVER_SIDE_CS_PIN);
    writePinLow(DRIVER_SIDE_CS_PIN);
    uart_send_cmd(CMD_HAND, 0, 1);

    if (dev_info.link_mode == LINK_USB) {
        usb_lld_wakeup_host(&USB_DRIVER);
        restart_usb_driver(&USB_DRIVER);
    }
}

/**
 * @brief  Clears the EXTI's line pending flags.
 * @param  EXTI_Line: specifies the EXTI lines flags to clear.
 *   This parameter can be any combination of EXTI_Linex where x can be (0..19)
 * @retval None
 */
void EXTI_ClearFlag(uint32_t EXTI_Line) {
    EXTI->PR = EXTI_Line;
}

///////////////////////////////////////////////////////////////////////////////////////////

#define STM32_EXTI_0_1_HANDLER Vector54
OSAL_IRQ_HANDLER(STM32_EXTI_0_1_HANDLER) {
    EXTI->PR = 0xffff;
}

#define STM32_EXTI_2_3_HANDLER Vector58
OSAL_IRQ_HANDLER(STM32_EXTI_2_3_HANDLER) {
    EXTI->PR = 0xffff;
}

#define STM32_EXTI_4_15_HANDLER Vector5C
OSAL_IRQ_HANDLER(STM32_EXTI_4_15_HANDLER) {
    EXTI->PR = 0xffff;
}

void EXTI_DeInit(void) {
    EXTI->IMR  = 0x0F940000;
    EXTI->EMR  = 0x00000000;
    EXTI->RTSR = 0x00000000;
    EXTI->FTSR = 0x00000000;
    EXTI->PR   = 0x006BFFFF;
}

void EXTI_Init(EXTI_InitTypeDef *EXTI_InitStruct) {
    uint32_t tmp = 0;

    tmp = (uint32_t)EXTI_BASE;

    if (EXTI_InitStruct->EXTI_LineCmd != DISABLE) {
        /* Clear EXTI line configuration */
        EXTI->IMR &= ~EXTI_InitStruct->EXTI_Line;
        EXTI->EMR &= ~EXTI_InitStruct->EXTI_Line;

        tmp += EXTI_InitStruct->EXTI_Mode;

        *(__IO uint32_t *)tmp |= EXTI_InitStruct->EXTI_Line;

        /* Clear Rising Falling edge configuration */
        EXTI->RTSR &= ~EXTI_InitStruct->EXTI_Line;
        EXTI->FTSR &= ~EXTI_InitStruct->EXTI_Line;

        /* Select the trigger for the selected interrupts */
        if (EXTI_InitStruct->EXTI_Trigger == EXTI_Trigger_Rising_Falling) {
            /* Rising Falling edge */
            EXTI->RTSR |= EXTI_InitStruct->EXTI_Line;
            EXTI->FTSR |= EXTI_InitStruct->EXTI_Line;
        } else {
            tmp = (uint32_t)EXTI_BASE;
            tmp += EXTI_InitStruct->EXTI_Trigger;

            *(__IO uint32_t *)tmp |= EXTI_InitStruct->EXTI_Line;
        }
    } else {
        tmp += EXTI_InitStruct->EXTI_Mode;

        /* Disable the selected external lines */
        *(__IO uint32_t *)tmp &= ~EXTI_InitStruct->EXTI_Line;
    }
}

/**
 * @brief  Fills each EXTI_InitStruct member with its reset value.
 * @param  EXTI_InitStruct: pointer to a EXTI_InitTypeDef structure which will
 *         be initialized.
 * @retval None
 */
void EXTI_StructInit(EXTI_InitTypeDef *EXTI_InitStruct) {
    EXTI_InitStruct->EXTI_Line    = 0;
    EXTI_InitStruct->EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct->EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct->EXTI_LineCmd = DISABLE;
}

void uart_init(uint32_t baud); // qmk uart.c
void m_uart1_init(void) {
    uart_init(460800); // RF通讯串口初始化
    USART_Cmd(USART1, DISABLE);
    // 使能9bit 使能偶校验
    USART1->CR1 |= USART_CR1_M0 | USART_CR1_PCE;
    USART_Cmd(USART1, ENABLE);

    m_uart_gpio_set_low_speed_and_pullup();
}

void m_timer6_init(void) {
    NVIC_InitTypeDef        NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* TIM6 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    /*  TIM6 中断嵌套设计*/
    NVIC_InitStructure.NVIC_IRQChannel         = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Time 定时器基础设置 */
    TIM_TimeBaseStructure.TIM_Period        = 1000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = 48 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    /* TIM6 使能 */
    TIM_Cmd(TIM6, ENABLE);
}

volatile uint8_t idle_sleep_cnt = 0;
OSAL_IRQ_HANDLER(STM32_TIM6_HANDLER) {
    if (TIM_GetFlagStatus(TIM6, TIM_FLAG_Update) != ST_RESET) {
        TIM_ClearFlag(TIM6, TIM_FLAG_Update);
        idle_sleep_cnt++;
    }
}

void idle_enter_sleep(void) {
    TIM6->CNT      = 0;
    idle_sleep_cnt = 0;
    while (idle_sleep_cnt < 1) {
        PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
    }
}
