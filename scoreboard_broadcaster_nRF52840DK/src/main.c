/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <zephyr/drivers/uart.h>
#include "df2301q.h"

#define COMPANY_ID_CODE 0x0059 // Nordic BLE ID

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define RUN_STATUS_LED DK_LED1
#define RUN_LED_BLINK_INTERVAL 1000

#define USER_BUTTON1 DK_BTN1_MSK
#define USER_BUTTON2 DK_BTN2_MSK

/* Define the size of the UART receive buffer */
#define RECEIVE_BUFF_SIZE 13

/* Define the receiving timeout period */
#define RECEIVE_TIMEOUT 1200

/* RTOS Task properties */
#define SB_STACKSIZE       1024
#define SB_PRIORITY        5 

/* Define semaphore */
K_SEM_DEFINE(sem, 0, 1);

/* Declare the structure for your custom data  */
typedef struct adv_mfg_data {
	uint16_t company_code; /* Company Identifier Code. */
	uint8_t team_home_points;
	uint8_t team_guest_points;
	uint8_t team_home_set;
	uint8_t team_guest_set;
	uint8_t serving; 
} adv_mfg_data_type;

/* Create an LE Advertising Parameters variable */
static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(BT_LE_ADV_OPT_NONE, /* No options specified */
											800, /* Min Advertising Interval 500ms (800*0.625ms) */
											801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
											NULL); /* Set to NULL for undirected advertising */

/* Define and initialize a variable of type adv_mfg_data_type */
static adv_mfg_data_type adv_mfg_data = {COMPANY_ID_CODE, 0x00, 0x00, 0x00, 0x00, 0x00};

static unsigned char url_data[] = { };

LOG_MODULE_REGISTER(Scoreboard, LOG_LEVEL_INF);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	/* Include the Manufacturer Specific Data in the advertising packet. */
	BT_DATA(BT_DATA_MANUFACTURER_DATA, (unsigned char *)&adv_mfg_data, sizeof(adv_mfg_data)),
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_URI, url_data, sizeof(url_data)),
};

/* Get the device pointer of the UART hardware */
const struct device *uart= DEVICE_DT_GET(DT_NODELABEL(uart0));

/* Define the receive buffer */
static uint8_t rx_buf[RECEIVE_BUFF_SIZE] = {0};

uint8_t flag = 0;
static uint8_t DF2301Q_CMD = 0;

/* Define the callback function for UART */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch (evt->type) {

	case UART_RX_RDY:

		uart_rx_disable(dev);

	    if(flag == 0)
		{
			dk_set_led(DK_LED3, 1);	
			flag = 1;
		}
		else if(flag == 1)
		{
			dk_set_led(DK_LED3, 0);
			flag = 0;
		}	

		if((evt->data.rx.len) == 13)
		{
			uart_rx_enable(dev ,rx_buf,sizeof rx_buf,RECEIVE_TIMEOUT);
			DF2301Q_CMD = 1;
			k_sem_give(&sem);

		}
		else if((evt->data.rx.len) == 12)
		{
			dk_set_led(DK_LED1, 0);
			dk_set_led(DK_LED2, 0);
			DF2301Q_CMD = 0;
		}

		uart_rx_enable(dev ,rx_buf,sizeof rx_buf,RECEIVE_TIMEOUT);
	
		k_sem_give(&sem);	
	break;

	case UART_RX_DISABLED:
		uart_rx_enable(dev ,rx_buf,sizeof rx_buf,RECEIVE_TIMEOUT);
	break;
		
	default:
	break;
	}
}

/* Add the definition of callback function and update the advertising data dynamically */
static void button_changed(uint32_t button_state, uint32_t has_changed)
{
	

}

/* Define the initialization function of the buttons and setup interrupt.  */
static int init_button(void)
{
	int err;

	err = dk_buttons_init(button_changed);
	if (err) {

	}

	return err;
}

int thread0(void)
{	
	int err;

	/* Setup leds on your board  */
	err = dk_leds_init();
	if (err) {		
		return -1;
	}	

	/* Setup buttons on your board  */
	err = init_button();
	if (err) {		
		return -1;
	}

	/* Verify that the UART device is ready */ 
	if (!device_is_ready(uart)){		
		return -1;
	}

	/* Register the UART callback function */
	err = uart_callback_set(uart, uart_cb, NULL);
	if (err) {
		return -1;
	}	

	/* Bluetooth enable */
	err = bt_enable(NULL);
	if (err) {		
		return -1;
	}	

	err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {		
		return -1;
	}	

	err = uart_rx_enable(uart ,rx_buf, 13,RECEIVE_TIMEOUT);
	if (err) {			
		return -1;
	}	

	while(1)
	{		
		k_sem_take(&sem, K_FOREVER);
		if(DF2301Q_CMD == 1)
		{
			DF2301Q_CMD = 0;
			if((rx_buf[0] == 0xF4) && (rx_buf[1] == 0xF5))
			{			
				if(rx_buf[2] == 0x03)
				{				
					if(rx_buf[7] == TEAM_HOME_PLUS_ONE_POINT)	
					{
						if(adv_mfg_data.team_home_points == 99)
						{
							adv_mfg_data.team_home_points = 99;
						}
						else
						{
							adv_mfg_data.team_home_points += 1; 
						}
					}
					else if(rx_buf[7] == TEAM_HOME_MINUS_ONE_POINT)	
					{
						if(adv_mfg_data.team_home_points == 0)
						{
							adv_mfg_data.team_home_points = 0;
						}
						else
						{
							adv_mfg_data.team_home_points -= 1; 
						}
					}	
					else if(rx_buf[7] == TEAM_GUEST_PLUS_ONE_POINT)	
					{
						if(adv_mfg_data.team_guest_points == 99)
						{
							adv_mfg_data.team_guest_points = 99;
						}
						else
						{
							adv_mfg_data.team_guest_points += 1; 
						}
					}	
					else if(rx_buf[7] == TEAM_GUEST_MINUS_ONE_POINT)	
					{
						if(adv_mfg_data.team_guest_points == 0)
						{
							adv_mfg_data.team_guest_points = 0;
						}
						else
						{
							adv_mfg_data.team_guest_points -= 1; 
						}
					}	
					else if(rx_buf[7] == TEAM_HOME_PLUS_ONE_SET)	
					{
						if(adv_mfg_data.team_home_set == 9)
						{
							adv_mfg_data.team_home_set = 9;
						}
						else
						{
							adv_mfg_data.team_home_set += 1; 
						}
					}	
					else if(rx_buf[7] == TEAM_HOME_MINUS_ONE_SET)	
					{
						if(adv_mfg_data.team_home_set == 0)
						{
							adv_mfg_data.team_home_set = 0;
						}
						else
						{
							adv_mfg_data.team_home_set -= 1; 
						}
					}	
					else if(rx_buf[7] == TEAM_GUEST_PLUS_ONE_SET)	
					{
						if(adv_mfg_data.team_guest_set == 9)
						{
							adv_mfg_data.team_guest_set = 9;
						}
						else
						{
							adv_mfg_data.team_guest_set += 1; 
						}
					}	
					else if(rx_buf[7] == TEAM_GUEST_MINUS_ONE_SET)	
					{
						if(adv_mfg_data.team_guest_set == 0)
						{
							adv_mfg_data.team_guest_set = 0;
						}
						else
						{
							adv_mfg_data.team_guest_set -= 1; 
						}
					}
					else if(rx_buf[7] == TEAM_HOME_SERVING)	
					{
						adv_mfg_data.serving |= 1 << TEAM_HOME_SERVING_BIT;
						adv_mfg_data.serving &= ~(1 << TEAM_GUEST_SERVING_BIT);						
					}	
					else if(rx_buf[7] == TEAM_GUEST_SERVING)	
					{
						adv_mfg_data.serving |= 1 << TEAM_GUEST_SERVING_BIT;
						adv_mfg_data.serving &= ~(1 << TEAM_HOME_SERVING_BIT);	
					}
					else if(rx_buf[7] == SCORE_BOARD_RESET)	
					{
						adv_mfg_data.team_home_points = 0;
						adv_mfg_data.team_guest_points = 0;
						adv_mfg_data.team_home_set = 0;
						adv_mfg_data.team_guest_set = 0;
						adv_mfg_data.serving = 0;
					}
				}
				else if(rx_buf[2] == 0x02)
				{
					dk_set_led(DK_LED1, 0);
					dk_set_led(DK_LED2, 0);					
				}	

				bt_le_adv_update_data(ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
				memset(rx_buf, 0, sizeof(rx_buf));				
			}
			//k_sleep(K_MSEC(500));
		}		
	}
}


K_THREAD_DEFINE(thread0id, SB_STACKSIZE, thread0, NULL, NULL, NULL,
				SB_PRIORITY, 0, 0);

