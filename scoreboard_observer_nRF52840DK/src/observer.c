/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <stdio.h>
#include <string.h>

#define NAME_LEN 30
#define BT_DEVICE "Score Board"

static bool data_cb(struct bt_data *data, void *user_data)
{
	char *name = user_data;
	uint8_t len;
	int res = 0;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
		len = MIN(data->data_len, NAME_LEN - 1);
		(void)memcpy(name, data->data, len);
		res = memcmp(name, BT_DEVICE, 11);
		if(res == 0)
		{
			name[len] = '\0';
			printk("Device Name: %s ", name);
			printk("\n");
			return true;				
		}
		else
		{
			return false;
		}

	case BT_DATA_MANUFACTURER_DATA:
		printk("Manufacturer data: ");
		for (uint16_t i=0; i<data->data_len; i++)
		{
			printk("%02x:", data->data[i]);
		}
		printk("\n");
    	return false;

	default:
		return true;
	}
}


static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	char name[NAME_LEN];

	bt_data_parse(ad, data_cb, name);
	//printk("scan_recv: %s\n", name);
}


int observer_start(void)
{
	int err;
	
	struct bt_le_scan_param scan_param = {
		.type       = BT_LE_SCAN_TYPE_ACTIVE,
		.options    = BT_LE_SCAN_OPT_FILTER_DUPLICATE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
	};

	err = bt_le_scan_start(&scan_param, device_found);
	if (err) {
		printk("Start scanning failed (err %d)\n", err);
		return err;
	}
	printk("Started scanning...\n");

	return 0;
}
