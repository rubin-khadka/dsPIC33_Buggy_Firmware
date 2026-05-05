/* 
 * File:   message_handler.h
 * Author: Rubin Khadka
 *
 * Sending and Receiving message Header file
 */

#ifndef MESSAGE_HANDLER_H
#define	MESSAGE_HANDLER_H

#include "parser.h"
#include "state_machine.h"

#ifdef	__cplusplus
extern "C" {
#endif
          
extern volatile int16_t speed; 
extern volatile int16_t yawrate;
    
/* Function Prototypes */
void uart_send_string(const char *str);
void process_message(parser_state* ps);

    
#ifdef	__cplusplus
}
#endif

#endif	

