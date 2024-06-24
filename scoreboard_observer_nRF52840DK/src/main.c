/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <dk_buttons_and_leds.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/sys/util.h>

/* RTOS Task properties */
#define SB_STACKSIZE       1024
#define SB_PRIORITY        5 

#define TEAM_HOME_SERVING_BIT           1  // 1
#define TEAM_GUEST_SERVING_BIT          2  // 2

#define NAME_LEN 30
#define MAN_LEN  8
#define BT_DEVICE "Score Board"

#define STRIP_NODE		DT_ALIAS(led_strip)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)

#define RGB_LEDS_PER_DIGIT 14

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(0xFF, 0x00, 0x00), /* red */
	RGB(0x00, 0xFF, 0x00), /* green */
	RGB(0x00, 0x00, 0xFF), /* blue */
};

static const struct led_rgb black = {
	.r = 0x00,
	.g = 0x00,
	.b = 0x00,
};

struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

/* Define semaphore */
K_SEM_DEFINE(sem, 0, 1);

char bt_device_name[NAME_LEN] = {0,};
bool bt_device_found = false;
uint8_t bt_man_data[MAN_LEN] = {0,};
uint8_t bt_man_data_curr[MAN_LEN] = {0,};

long numbers[] = {
  0b00111111111111,  // [0] 0
  0b00110000000011,  // [1] 1
  0b11111100111100,  // [2] 2
  0b11111100001111,  // [3] 3
  0b11110011000011,  // [4] 4
  0b11001111001111,  // [5] 5
  0b11001111111111,  // [6] 6
  0b00111100000011,  // [7] 7
  0b11111111111111,  // [8] 8
  0b11111111000011,  // [9] 9
 };


void update_points(uint8_t homepoints, uint8_t guestpoints)
{
	uint8_t guest_digit_zero_index = 0;
	uint8_t guest_digit_one_index = 14;
	uint8_t home_digit_zero_index = 28;
	uint8_t home_digit_one_index = 42;
	uint8_t d1 = guestpoints / 10;  
	uint8_t d0 = guestpoints % 10; 
	uint8_t d3 = homepoints / 10; 
	uint8_t d2 = homepoints % 10;  
	uint8_t i = 0;

	for(i = 0; i < RGB_LEDS_PER_DIGIT; i++)
	{
		if((numbers[d0] & 1 << i) == 1 << i)
		{
	  		memcpy(&pixels[i + guest_digit_zero_index], &colors[0], sizeof(struct led_rgb));
        }
        else
        {
	  		memcpy(&pixels[i + guest_digit_zero_index], &black, sizeof(struct led_rgb));	
		}
	}

	for(i = 0; i < RGB_LEDS_PER_DIGIT; i++)
	{
		if((numbers[d1] & 1 << i) == 1 << i)
		{
			memcpy(&pixels[i + guest_digit_one_index], &colors[0], sizeof(struct led_rgb));
		}
		else
		{
			memcpy(&pixels[i + guest_digit_one_index], &black, sizeof(struct led_rgb));
		}
	}	

	for(i = 0; i < RGB_LEDS_PER_DIGIT; i++)
	{
		if((numbers[d2] & 1 << i) == 1 << i)
		{
	  		memcpy(&pixels[i + home_digit_zero_index], &colors[0], sizeof(struct led_rgb));
        }
        else
        {
	  		memcpy(&pixels[i + home_digit_zero_index], &black, sizeof(struct led_rgb));	
		}
	}

	for(i = 0; i < RGB_LEDS_PER_DIGIT; i++)
	{
		if((numbers[d3] & 1 << i) == 1 << i)
		{
			memcpy(&pixels[i + home_digit_one_index], &colors[0], sizeof(struct led_rgb));
		}
		else
		{
			memcpy(&pixels[i + home_digit_one_index], &black, sizeof(struct led_rgb));
		}
	}

	led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
}

void update_serving(uint8_t serving)
{
	if((serving & TEAM_HOME_SERVING_BIT) == TEAM_HOME_SERVING_BIT)
	{
		memcpy(&pixels[56], &colors[0], sizeof(struct led_rgb));		
		memcpy(&pixels[57], &colors[0], sizeof(struct led_rgb));		
		memcpy(&pixels[58], &black, sizeof(struct led_rgb));		
		memcpy(&pixels[59], &black, sizeof(struct led_rgb));		
	}
	else if((serving & TEAM_GUEST_SERVING_BIT) == TEAM_GUEST_SERVING_BIT)
	{
		memcpy(&pixels[56], &black, sizeof(struct led_rgb));		
		memcpy(&pixels[57], &black, sizeof(struct led_rgb));		
		memcpy(&pixels[58], &colors[0], sizeof(struct led_rgb));		
		memcpy(&pixels[59], &colors[0], sizeof(struct led_rgb));		
	}
	else if(serving == 0)
	{
		memcpy(&pixels[56], &black, sizeof(struct led_rgb));		
		memcpy(&pixels[57], &black, sizeof(struct led_rgb));		
		memcpy(&pixels[58], &black, sizeof(struct led_rgb));		
		memcpy(&pixels[59], &black, sizeof(struct led_rgb));		
	}

	led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
}

void update_sets(uint8_t homesets, uint8_t guestsets)
{
	uint8_t guest_digit_index = 60;
	uint8_t home_digit_index = 74;
	uint8_t i = 0;

	for(i = 0; i < RGB_LEDS_PER_DIGIT; i++)
	{
		if((numbers[guestsets] & 1 << i) == 1 << i)
		{
	  		memcpy(&pixels[i + guest_digit_index], &colors[0], sizeof(struct led_rgb));
        }
        else
        {
	  		memcpy(&pixels[i + guest_digit_index], &black, sizeof(struct led_rgb));	
		}
	}

	for(i = 0; i < RGB_LEDS_PER_DIGIT; i++)
	{
		if((numbers[homesets] & 1 << i) == 1 << i)
		{
	  		memcpy(&pixels[i + home_digit_index], &colors[0], sizeof(struct led_rgb));
        }
        else
        {
	  		memcpy(&pixels[i + home_digit_index], &black, sizeof(struct led_rgb));	
		}
	}

	led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
}

static bool data_cb(struct bt_data *data, void *user_data)
{
	uint8_t len;
	int cmp, res = 0;

	switch (data->type) 
	{
		case BT_DATA_NAME_SHORTENED:
		case BT_DATA_NAME_COMPLETE:
			len = MIN(data->data_len, NAME_LEN - 1);
			(void)memcpy(bt_device_name, data->data, len);
			res = memcmp(bt_device_name, BT_DEVICE, 11);
			bt_device_name[len] = '\0';
			if(res == 0)
			{
				bt_device_found = true;				
				return true;				
			}
			else
			{
				return false;
			}

		case BT_DATA_MANUFACTURER_DATA:

			if(bt_device_found == true)
			{
				bt_device_found = false;
				(void)memcpy(bt_man_data, data->data, data->data_len);
				cmp = memcmp(bt_man_data_curr, bt_man_data, MAN_LEN);
				if(cmp != 0)
				{
					k_sem_give(&sem);
				}
				
			}
			return false;	

		default:
			return true;
	}
}


static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	bt_data_parse(ad, data_cb, NULL);	
}


int thread0(void)
{
	int err;
	
	struct bt_le_scan_param scan_param = {
		.type       = BT_LE_SCAN_TYPE_ACTIVE,
		.options    = BT_LE_SCAN_OPT_FILTER_DUPLICATE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
	};

	printk("Starting Observer Demo\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	err = bt_le_scan_start(&scan_param, device_found);
	if (err) {
		printk("Start scanning failed (err %d)\n", err);
		return err;
	}

	if (device_is_ready(strip)) {
		printk("Found LED strip device %s", strip->name);
	} else {
		printk("LED strip device %s is not ready", strip->name);
		return 0;
	}

	memset(&pixels, 0x00, sizeof(pixels));
	led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);

	update_points(0, 0);
	update_serving(0);
	update_sets(0, 0);	

	printk("Started scanning...\n");
	
	while(1)
	{		
		k_sem_take(&sem, K_FOREVER);

		printk("Device Name: %s ", bt_device_name);
		printk("\n");

		printk("Manufacturer data: ");
		for (uint16_t i=0; i<MAN_LEN; i++)
		{
			printk("%02x:", bt_man_data[i]);
		}
		printk("\n");

		update_points(bt_man_data[2], bt_man_data[3]);		
		update_sets(bt_man_data[4], bt_man_data[5]);
		update_serving(bt_man_data[6]);	

		memcpy(bt_man_data_curr, bt_man_data, MAN_LEN);
	}	
}

K_THREAD_DEFINE(thread0id, SB_STACKSIZE, thread0, NULL, NULL, NULL,
				SB_PRIORITY, 0, 0);
