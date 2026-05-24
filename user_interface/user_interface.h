/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : user_interface.h                                      */
/*                                                                            */
/* Interface for user_interface library                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef USER_REQUEST_H_
#define USER_REQUEST_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_user_interface(void);
void deinit_user_interface(void);
uint32_t get_button_press_count(void);
void clear_button_press_count(void);

#endif /* USER_REQUEST_H_ */
