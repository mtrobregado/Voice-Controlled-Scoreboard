#include <zephyr/device.h>
#include <zephyr/toolchain.h>

/* 1 : /soc/clock@40000000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_78[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 2 : /soc/gpio@50000300:
 * Supported:
 *    - /soc/spi@4002f000
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_107[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, 10, DEVICE_HANDLE_ENDS };

/* 3 : /soc/gpio@50000000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_15[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 4 : /soc/random@4000d000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_100[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 5 : /entropy_bt_hci:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_5[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 6 : /soc/crypto@5002a000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_120[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 7 : /soc/uart@40002000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_115[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 8 : /soc/i2s@40025000:
 * Supported:
 *    - /ws2812
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_12[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, 11, DEVICE_HANDLE_ENDS };

/* 9 : /soc/spi@40004000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_105[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 10 : /soc/spi@4002f000:
 * Direct Dependencies:
 *    - /soc/gpio@50000300
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_108[] = { 2, DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 11 : /ws2812:
 * Direct Dependencies:
 *    - /soc/i2s@40025000
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_13[] = { 8, DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };
