/* 
 * File:   acc_spi.h
 * Author: Rubin Khadka 
 *
 * Header Accelerometer SPI Interface
 */

#ifndef ACC_SPI_H
#define	ACC_SPI_H

#include "uart.h"

#ifdef	__cplusplus
extern "C" {
#endif

// Accelerometer register addresses
#define ACC_X_LSB     0x02   // X-axis LSB
#define ACC_X_MSB     0x03   // X-axis MSB
#define ACC_Y_LSB     0x04   // Y-axis LSB
#define ACC_Y_MSB     0x05   // Y-axis MSB
#define ACC_Z_LSB     0x06   // Z-axis LSB
#define ACC_Z_MSB     0x07   // Z-axis MSB
#define ACC_CHIP_ID   0x00   // Chip ID register
#define ACC_RANGE     0x0F   // Range selection
#define ACC_PMU_CTRL  0x11   // Power mode control
#define ACC_PMU_BW    0x10   // Bandwidth control
#define ACC_HBW       0x13   // Filtered Data control
    
// Accelerometer CS pin - RB3
#define ACC_CS_LAT    LATBbits.LATB3     // Output value
#define ACC_CS_TRIS   TRISBbits.TRISB3   // Direction register
    
// Data structure for acceleration values
typedef struct {
    int16_t x;  // in mg
    int16_t y;  // in mg
    int16_t z;  // in mg
} AccData;

/* SPI Functions */
void spi_init(void);                // Initialize SPI
uint16_t spi_write(uint16_t data);  // Write 16-bit data

/* Accelerometer Functions */
uint8_t acc_read_register(uint8_t reg_addr);
void acc_write_register(uint8_t reg_addr, uint8_t value);
void acc_init(void);
AccData read_acc_all(void);

/* Communication Functions */
void send_acc_data(const AccData *data);
    
#ifdef	__cplusplus
}
#endif

#endif	/* ACC_SPI_H */

