/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : user_request.h                                        */
/*                                                                            */
/* Interface for user_request library                                         */
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
void init_user_request(void);
void deinit_user_request(void);
uint32_t get_user_request(void);
void clear_user_request(void);

#endif /* USER_REQUEST_H_ */
