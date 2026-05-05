
/* Source file for UART 1 Interrupt Service Routine */

#include "uart.h"

/* UART receive interrupt function */
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
    // Check for hardware error
    if (U1STAbits.OERR) {
        U1STAbits.OERR = 0;  // Clear overrun error to allow new data
    }
    
    while (U1STAbits.URXDA) {  // While data available
        UART1_Buffer_Write(&uart1_rx, U1RXREG);
    }
    
    // Clear interrupt flag
    IFS0bits.U1RXIF = 0;
}

/* UART transmit interrupt function */
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;  // Clear the interrupt flag first

    uint8_t data;
    // Fill the UART hardware FIFO as much as possible
    while (!UART1_Buffer_IsEmpty(&uart1_tx)) {
        if (U1STAbits.UTXBF) {
            break;  // UART hardware FIFO is full
        }
        if (UART1_Buffer_Read(&uart1_tx, &data)) {
            U1TXREG = data;  // Send one byte
        }
    }

    if (UART1_Buffer_IsEmpty(&uart1_tx)) {
        IEC0bits.U1TXIE = 0;  // Disable UART TX interrupt if buffer empty
    }
}
