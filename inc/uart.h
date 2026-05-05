/* 
 * File:   UART.h
 * Author: Rubin Khadka 
 *
 * Header File for UART 
 */

#ifndef UART_H
#define	UART_H

#include "init.h"
#include "timer.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* Configuration */
#define BAUDRATE            115200      // Default UART baud rate
    
/* UART Buffers @115200 baud (10 bits/byte) */
#define UART_RX_BUF_SIZE    64   // Covers 23B/2ms bursts (2.8x margin)
#define UART_TX_BUF_SIZE    128  // Handles worst-case 60B bursts (2.1x margin)

#ifdef	__cplusplus
}
#endif

/* Circular Buffer Structure */
typedef struct {
    volatile uint8_t *buffer;           // Pointer to buffer storage
    volatile uint16_t size;             // Size of the buffer
    volatile uint16_t head;             // Write position
    volatile uint16_t tail;             // Read position
    volatile bool overflow;             // Overflow flag
} UART_Buffer;

// Global Buffer Instances
extern volatile UART_Buffer uart1_rx;
extern volatile UART_Buffer uart1_tx;

// Initialization
void UART1_Init(uint32_t baudrate);

// Buffer Operations
void UART1_Buffer_Init(volatile UART_Buffer *buf, uint8_t *storage, uint16_t size);
bool UART1_Buffer_Write(volatile UART_Buffer *buf, uint8_t data);
bool UART1_Buffer_Read(volatile UART_Buffer *buf, uint8_t *data);
bool UART1_Buffer_Critical_Read(uint8_t *data);

// Status Checks
bool UART1_Buffer_IsEmpty(volatile UART_Buffer *buf);
bool UART1_Buffer_IsFull(volatile UART_Buffer *buf);

#endif	/* UART_H */

