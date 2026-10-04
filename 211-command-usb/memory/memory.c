#include "memory.h"
#include "command.h"
#include "device.h"
#include <stdio.h>
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"
#include <stdlib.h>

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

uint32_t data_variable = 100;
uint32_t bss_variable;



int main(void);


static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}


void mem_info(void)
{

unsigned boot2_size = (unsigned)((uintptr_t)&__boot2_end__ - (uintptr_t)&__boot2_start__);
unsigned text_size  = (unsigned)((uintptr_t)&__etext - (uintptr_t)&__boot2_end__);
unsigned data_size  = (unsigned)((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__);
unsigned bss_size   = (unsigned)((uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__);

unsigned image_size = boot2_size + text_size + data_size;
unsigned flash_total = PICO_FLASH_SIZE_BYTES;
unsigned flash_free  = flash_total - image_size;

unsigned ram_used = data_size + bss_size;
unsigned heap_free = (unsigned)((uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__);
unsigned stack_size = (unsigned)((uintptr_t)&__StackTop - (uintptr_t)&__StackBottom);
    printf("%-10s %-10s %-10s %8s\n", "area", "start", "end", "size");
    row("flash",   XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("sram",   SRAM_BASE, SRAM_BASE+264*1024);
    row("rom",   ROM_BASE, ROM_BASE+16*1024);
    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    row("free", (uintptr_t)&__flash_binary_end, XIP_BASE+PICO_FLASH_SIZE_BYTES);
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);
    row("data flash", (uintptr_t)&__etext, (uintptr_t)&__etext+(uintptr_t)&__data_end__-(uintptr_t)&__data_start__);
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    
    

printf("\n");
printf("total\n");
printf("  %-11s %8u = boot2 %u + text %u + data %u\n",
       "flash image", image_size, boot2_size, text_size, data_size);
printf("  %-11s %8u of %u\n",
       "flash free", flash_free, flash_total);
printf("  %-11s %8u = data %u + bss %u\n",
       "ram used", ram_used, data_size, bss_size);
printf("  %-11s %8u for heap and %u for stack\n",
       "ram free", heap_free, stack_size);

}

void fw_info(void)
{
   
    data_variable++;
    bss_variable++;

    
    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));
    if (heap_variable != NULL)
        *heap_variable = 1951;

 
    
    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    uint16_t *fw_code = (uint16_t *)((uintptr_t)fw_info & ~1u);



  
    static const char dev_name[]    = DEVICE_NAME;
    static const char dev_version[] = FIRMWARE_VERSION;
    static const char dev_project[] = DEVICE_PROJECT;
    static const char dev_board[]   = DEVICE_BOARD;
    static const char dev_repo[]    = DEVICE_REPO;

    
    printf("%-16s %-12s %s\n", "object", "address", "value");
    printf("%-16s %-12s %s\n", "----------------",
                               "------------",
                               "------------------");


    printf("%-16s 0x%08lx   0x%04x\n",
           "main",    (unsigned long)(uintptr_t)main,    *main_code);
    printf("%-16s 0x%08lx   0x%04x\n",
           "fw_info", (unsigned long)(uintptr_t)fw_info, *fw_code);


    printf("%-16s 0x%08lx\n", "commands", (unsigned long)(uintptr_t)commands);

   
    for (uint i = 0; i < command_count; i++) {
        printf("- %-14s 0x%08lx\n",
               commands[i].name,
               (unsigned long)(uintptr_t)commands[i].handler);
    }

  
    printf("%-16s 0x%08lx   %s\n", "DEVICE_NAME",
           (unsigned long)(uintptr_t)dev_name,    dev_name);
    printf("%-16s 0x%08lx   %s\n", "FIRMWARE_VERSION",
           (unsigned long)(uintptr_t)dev_version, dev_version);
    printf("%-16s 0x%08lx   %s\n", "DEVICE_PROJECT",
           (unsigned long)(uintptr_t)dev_project, dev_project);
    printf("%-16s 0x%08lx   %s\n", "DEVICE_BOARD",
           (unsigned long)(uintptr_t)dev_board,   dev_board);
    printf("%-16s 0x%08lx   %s\n", "DEVICE_REPO",
           (unsigned long)(uintptr_t)dev_repo,    dev_repo);


    printf("%-16s 0x%08lx   %lu  (.data)\n", "data_variable",
           (unsigned long)(uintptr_t)&data_variable,
           (unsigned long)data_variable);

   
    printf("%-16s 0x%08lx   %lu  (.bss)\n", "bss_variable",
           (unsigned long)(uintptr_t)&bss_variable,
           (unsigned long)bss_variable);

  
    printf("%-16s 0x%08lx   %lu  (stack)\n", "stack_variable",
           (unsigned long)(uintptr_t)&stack_variable,
           (unsigned long)stack_variable);

    /* --- куча: печатаем адрес БЛОКА, а не адрес указателя --- */
    if (heap_variable != NULL) {
        printf("%-16s 0x%08lx   %lu  (heap)\n", "heap_variable",
               (unsigned long)(uintptr_t)heap_variable,
               (unsigned long)*heap_variable);
    } else {
        printf("%-16s (malloc failed)\n", "heap_variable");
    }

    
    free(heap_variable);
}

void cmd_fw_info(void)
{
    fw_info();
}
