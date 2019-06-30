#include "spi_memory.h"
#include <debug.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ch554.h>
#include <ch554_usb.h>
#include <debug.h>
#include <spi.h>

void MEM_writeEnable(){
    CH554SPIMasterWrite(MEM_WREN);
}
void MEM_writeDisable(){
    CH554SPIMasterWrite(MEM_WRDI);
}
uint8_t MEM_getStatusRegister(){
    uint8_t stReg;
    CH554SPIMasterWrite(MEM_RDSR);
    stReg = CH554SPIMasterRead();
    return stReg;
}
void MEM_setStatusRegister(uint8_t st){
    CH554SPIMasterWrite(MEM_WRSR);
    CH554SPIMasterWrite(st);
}

void MEM_startRead(uint32_t addr){
    CH554SPIMasterWrite(MEM_READ);
    MEM_writeAddress(addr);
}

uint8_t MEM_read(){
    return CH554SPIMasterRead();
}

uint8_t MEM_readByte(uint32_t addr){
    uint8_t data;
    CH554SPIMasterWrite(MEM_READ);
    MEM_writeAddress(addr);
    data = CH554SPIMasterRead();
}
uint8_t MEM_readFrom(uint32_t addr, uint8_t length, uint8_t * buf){
    uint32_t i;
    CH554SPIMasterWrite(MEM_READ);
    MEM_writeAddress(addr);
    for(i=0; i<length ;i++){
        buf[i] = CH554SPIMasterRead();
    }
}

void MEM_startWrite(uint32_t addr){
    CH554SPIMasterWrite(MEM_WRITE);
    MEM_writeAddress(addr);
}

void MEM_write(uint8_t data){
    CH554SPIMasterWrite(data);
}

void MEM_writeByteAt(uint32_t addr, uint8_t data){
    CH554SPIMasterWrite(MEM_WRITE);
    MEM_writeAddress(addr);
    CH554SPIMasterWrite(data);
}
void MEM_writeAt(uint32_t addr, uint8_t length, uint8_t * buf){
    uint32_t i;
    CH554SPIMasterWrite(MEM_WRITE);
    MEM_writeAddress(addr);
    for(i = 0; i < length; i++){
        CH554SPIMasterWrite(buf[i]);
    }
    
}

void MEM_writeAddress(uint32_t addr){
    CH554SPIMasterWrite((addr>>16)&0x000000FF);
    CH554SPIMasterWrite((addr>>8)&0x000000FF);
    CH554SPIMasterWrite((addr>>0)&0x000000FF);
}

void MEM_waitWriteCycle(){
    uint8_t ans;
    
    MEM_enableSS();
    CH554SPIMasterWrite(MEM_RDSR);
    ans = CH554SPIMasterRead();
    while(ans & 0x01){
        ans = CH554SPIMasterRead();
    };

    MEM_disableSS();
}

void MEM_chipErase(){
    uint8_t ans;

    MEM_enableSS();
	MEM_writeEnable();
	MEM_disableSS();

    MEM_enableSS();
    CH554SPIMasterWrite(MEM_CHER);
    MEM_disableSS();
    
    MEM_enableSS();
    CH554SPIMasterWrite(MEM_RDSR);
    ans = CH554SPIMasterRead();
    while(ans & 0x01){
        ans = CH554SPIMasterRead();
    };

    MEM_disableSS();
}

void MEM_chipEraseFirst64k(){
    uint8_t ans;

    MEM_enableSS();
	MEM_writeEnable();
	MEM_disableSS();

    MEM_enableSS();
    CH554SPIMasterWrite(0xD8);
    MEM_writeAddress(0);
    MEM_disableSS();
    
    MEM_enableSS();
    CH554SPIMasterWrite(MEM_RDSR);
    ans = CH554SPIMasterRead();
    while(ans & 0x01){
        ans = CH554SPIMasterRead();
    };

    MEM_disableSS();
}

void MEM_releasePowerDown(){
    MEM_enableSS();
	CH554SPIMasterWrite(MEM_RPWDN);
	MEM_disableSS();
    mDelaymS(1);
}

void MEM_disableSS(){
    DISABLE_BIT(P3_MOD_OC,FLASH_SS_PIN);
    ENABLE_BIT(P3_DIR_PU,FLASH_SS_PIN);
    FLASH_SS = 1;
}

void MEM_highImpedanceSS(){
    FLASH_SS = 1;
    ENABLE_BIT(P3_MOD_OC,FLASH_SS_PIN);
    DISABLE_BIT(P3_DIR_PU,FLASH_SS_PIN);
}

void MEM_enableSS(){
    DISABLE_BIT(P3_MOD_OC,FLASH_SS_PIN);
    ENABLE_BIT(P3_DIR_PU,FLASH_SS_PIN);
    FLASH_SS = 0;
}
