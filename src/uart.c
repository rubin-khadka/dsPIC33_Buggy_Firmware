
/* Source file for UART 1: Initialization and Functions */

#include "uart.h"

/* Global buffer instances for UART1 */
uint8_t uart1_rx_storage[UART_RX_BUF_SIZE];
uint8_t uart1_tx_storage[UART_TX_BUF_SIZE];
volatile UART_Buffer uart1_rx;
volatile UART_Buffer uart1_tx;

/* Initialize UART1 peripheral */
void UART1_Init(uint32_t baudrate) {
    
    // UART1 Remapping
    RPOR0bits.RP64R = 0b000001;  // Map U1TX to RD0 (RP64)
    RPINR18bits.U1RXR = 0x4B;    // Map U1RX to RD11 (RP75)
       
    // Baud rate calculation 
    U1BRG = (uint16_t)((FCY / (16UL * baudrate)) - 1);
    
    // Configure UART mode
    U1STAbits.URXISEL = 0b00; // Interrupt on every received character
    U1STAbits.UTXISEL0 = 1;   // Interrupt when transmit buffer is empty
    U1STAbits.UTXISEL1 = 0;
    
    // UART control registers setup
    U1MODEbits.UARTEN = 1;   // Enable UART module
    U1STAbits.UTXEN = 1;     // Enable transmitter
    
    // Initialize circular buffers
    UART1_Buffer_Init(&uart1_rx, uart1_rx_storage, UART_RX_BUF_SIZE);
    UART1_Buffer_Init(&uart1_tx, uart1_tx_storage, UART_TX_BUF_SIZE);
    
    // Clear flags
    IFS0bits.U1TXIF = 0;
    IFS0bits.U1RXIF = 0;

    // Configure interrupts
    IEC0bits.U1RXIE = 1;   // Enable receive interrupt
    IEC0bits.U1TXIE = 0;   // Disable transmit interrupt until needed
    
}

/* Initialize a UART buffer */
void UART1_Buffer_Init(volatile UART_Buffer *buf, uint8_t *storage, uint16_t size) {
    buf->buffer = storage;
    buf->size = size;
    buf->head = 0;
    buf->tail = 0;
    buf->overflow = false;
}

/* Write to a UART buffer */
bool UART1_Buffer_Write(volatile UART_Buffer *buf, uint8_t data) {
    uint16_t next_head = (buf->head + 1) % buf->size;
    
    // Always overwrite oldest data if full
    if (next_head == buf->tail) {
        buf->tail = (buf->tail + 1) % buf->size;  // Move tail forward
        buf->overflow = true;  // Set overflow flag
    }
    
    buf->buffer[buf->head] = data;
    buf->head = next_head;
    return true;
}

/* Read from a UART buffer */
bool UART1_Buffer_Read(volatile UART_Buffer *buf, uint8_t *data) {
    if (buf->head == buf->tail) {
        return false;  // Buffer empty
    }
    
    *data = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1) % buf->size;
    return true;
}

/* Helper function to minimize the disabling of RX interrupt during whole parsing process */
bool UART1_Buffer_Critical_Read(uint8_t *data) {
    
    bool result;
    
    uint8_t rxie_prev = IEC0bits.U1RXIE;  // Save current state
    IEC0bits.U1RXIE = 0;                 // Disable RX interrupt
    
    result = UART1_Buffer_Read(&uart1_rx, data);
    
    IEC0bits.U1RXIE = rxie_prev;          // Restore previous state
    
    return result;
    
}

/* Check if buffer is empty */
bool UART1_Buffer_IsEmpty(volatile UART_Buffer *buf) {
    return (buf->head == buf->tail);
}

/* Check if buffer is full */
bool UART1_Buffer_IsFull(volatile UART_Buffer *buf) {
    return ((buf->head + 1) % buf->size == buf->tail);
}


