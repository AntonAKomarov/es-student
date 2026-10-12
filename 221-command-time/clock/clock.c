#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#define CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY _u(0x01)
#define CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY _u(0x02)
#define CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC _u(0x03)
#define CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC_PH _u(0x04)
#define CLOCKS_FC0_SRC_VALUE_XOSC_CLKSRC _u(0x05)
#define CLOCKS_FC0_SRC_VALUE_CLKSRC_GPIN0 _u(0x06)
#define CLOCKS_FC0_SRC_VALUE_CLKSRC_GPIN1 _u(0x07)
#define CLOCKS_FC0_SRC_VALUE_CLK_REF _u(0x08)
#define CLOCKS_FC0_SRC_VALUE_CLK_SYS _u(0x09)
#define CLOCKS_FC0_SRC_VALUE_CLK_PERI _u(0x0a)
#define CLOCKS_FC0_SRC_VALUE_CLK_USB _u(0x0b)
#define CLOCKS_FC0_SRC_VALUE_CLK_ADC _u(0x0c)
#define CLOCKS_FC0_SRC_VALUE_CLK_RTC _u(0x0d)

static void row(const char *name, uint32_t set_khz, uint32_t measured_khz)
{
    printf("%-8s %9u %12u\n", name, (unsigned)set_khz, (unsigned)measured_khz);
}

void clk_info(void){
    printf("%-8s %9s %12s\n", "signal", "set_khz", "measured_khz"); 

    row("clk_ref", clock_get_hz(clk_ref) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_REF));
    row("clk_sys", clock_get_hz(clk_sys) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS));
    row("clk_peri", clock_get_hz(clk_peri) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI));
    row("clk_usb", clock_get_hz(clk_usb) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB));
    row("clk_adc", clock_get_hz(clk_adc) / 1000, frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC));
    printf("%-8s %9s %12u\n", "rosc", "-", frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC));    
}


