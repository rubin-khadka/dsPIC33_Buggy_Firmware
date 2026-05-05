/* 
 * File:   acc_spi.c
 * Author: Rubin Khadka
 * 
 * Accelerometer SPI Driver - Handles communication with the accelerometer
 * via SPI interface and data processing.
 */

#include "acc_spi.h"

/* Initialize SPI module in master mode */
void spi_init() {
    // Configure SPI control registers
    SPI1CON1bits.MSTEN = 1;     // Master mode
    SPI1CON1bits.MODE16 = 0;    // 8-bit communication
    SPI1CON1bits.CKP = 1;       // Clock idle high 
    SPI1CON1bits.CKE = 0;       // Data changes on idle->active 
    
    // Clock configuration (4.5MHz)
    SPI1CON1bits.PPRE = 1;      // Primary prescaler 4:1
    SPI1CON1bits.SPRE = 4;      // Secondary prescaler 4:1

    SPI1STATbits.SPIEN = 1;     // Enable SPI module
    
    // Chip select pin setup
    ACC_CS_TRIS = 0;            // Configure as output
    ACC_CS_LAT = 1;             // Deselect accelerometer
}

/* Basic SPI write operation with simultaneous read */
uint16_t spi_write(uint16_t data) {
    while (SPI1STATbits.SPITBF);    // Wait for transmit buffer empty
    SPI1BUF = data;                 // Load data to transmit
    
    while (!SPI1STATbits.SPIRBF);   // Wait for receive complete
    return SPI1BUF;                 // Return received data
}

/* Read from accelerometer register */
uint8_t acc_read_register(uint8_t reg_addr) {
    uint8_t data;
    ACC_CS_LAT = 0;                     // Select device
    spi_write(reg_addr | 0x80);         // Set MSB for read operation
    data = spi_write(0x00);             // Dummy write to read data
    ACC_CS_LAT = 1;                     // Deselect device
    return data;
}

/* Write to accelerometer register */
void acc_write_register(uint8_t reg_addr, uint8_t value) {
    ACC_CS_LAT = 0;                     // Select device
    spi_write(reg_addr & 0x7F);         // Clear MSB for write operation
    spi_write(value);                   // Write data
    ACC_CS_LAT = 1;                     // Deselect device
}

/* Initialize accelerometer with default settings */
void acc_init(void) {
    // Verify device ID
    uint8_t retries = 5;  // Max attempts
    uint8_t chip_id;

    while (retries--) {  // Try up to 5 times
        chip_id = acc_read_register(ACC_CHIP_ID);
        if (chip_id == 0xFA) break;  // Exit if correct ID found
        tmr_wait_ms(TIMER3, 10);     // Short delay between attempts
    }

    // Indication of not connected to accelerometer device (should never happen)
    if (chip_id != 0xFA) { // indication and debug
        LEDG9 ^= 1;             // Toggle LED to indicate error
        tmr_wait_ms(TIMER3, 200);  // Blink delay
    }

    // Configuration settings:
    acc_write_register(ACC_RANGE, 0x03);    // ±2g range
    acc_write_register(ACC_PMU_BW, 0x09);   // 15.63 Hz bandwidth
    acc_write_register(ACC_PMU_CTRL, 0x00); // Normal operation mode
    acc_write_register(ACC_HBW, 0x00);      // Ensure filtered data
    
    tmr_wait_ms(TIMER3, 10);                 // Allow settling time
}

/* Read all three acceleration axes */
AccData read_acc_all(void) {
    AccData data;
    uint8_t acc_buffer[6];

    // Burst read all axis registers
    ACC_CS_LAT = 0;
    spi_write(ACC_X_LSB | 0x80);  // Auto-increment read command
    for (int i = 0; i < 6; i++) {
        acc_buffer[i] = spi_write(0x00);  // Read 6 bytes (X,Y,Z LSB+MSB)
    }
    ACC_CS_LAT = 1;

    // Process raw data (12-bit values with sign extension)
    int16_t raw_x = (int16_t)((acc_buffer[1] << 8) | (acc_buffer[0] & 0x0F)) >> 4;
    int16_t raw_y = (int16_t)((acc_buffer[3] << 8) | (acc_buffer[2] & 0x0F)) >> 4;
    int16_t raw_z = (int16_t)((acc_buffer[5] << 8) | (acc_buffer[4] & 0x0F)) >> 4;

    // Convert to milli-g (3.91 mg/LSB for ±2g range)
    data.x = (int16_t)roundf(raw_x * 3.91f);
    data.y = (int16_t)roundf(raw_y * 3.91f);
    data.z = (int16_t)roundf(raw_z * 3.91f);

    return data;
}

/* Convert 16-bit integer to ASCII string */
void itoa_16(int16_t value, char* buffer) {
    char *ptr = buffer;
    if (value < 0) {
        *ptr++ = '-';
        value = -value;
    }

    // Extract digits in reverse order
    char temp[6];
    uint8_t i = 0;
    do {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    // Reverse digits into buffer
    while (i-- > 0) {
        *ptr++ = temp[i];
    }
    *ptr = '\0';
}

/* Format and send acceleration data via UART */
void send_acc_data(const AccData *data) {
    char buffer[32];  // Holds complete message
    char x_str[7], y_str[7], z_str[7];  // Axis value strings

    // Convert values to strings
    itoa_16(data->x, x_str);
    itoa_16(data->y, y_str);
    itoa_16(data->z, z_str);

    // Build NMEA-style message: $MACC,x,y,z*
    uint8_t len = 0;
    buffer[len++] = '$';
    buffer[len++] = 'M';
    buffer[len++] = 'A';
    buffer[len++] = 'C';
    buffer[len++] = 'C';
    buffer[len++] = ',';
    for (uint8_t i = 0; x_str[i] != '\0'; i++) buffer[len++] = x_str[i];
    buffer[len++] = ',';
    for (uint8_t i = 0; y_str[i] != '\0'; i++) buffer[len++] = y_str[i];
    buffer[len++] = ',';
    for (uint8_t i = 0; z_str[i] != '\0'; i++) buffer[len++] = z_str[i];
    buffer[len++] = '*';

    // Atomic UART transmission
    IEC0bits.U1TXIE = 0;  // Disable TX interrupts
    for (uint8_t i = 0; i < len; i++) {
        UART1_Buffer_Write(&uart1_tx, buffer[i]);
    }
    IEC0bits.U1TXIE = 1;  // Re-enable interrupts
    IFS0bits.U1TXIF = 1;  // Trigger transmission
}