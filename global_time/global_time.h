/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : global_time.h                                         */
/*                                                                            */
/* Interface for global_time library                                          */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef GLOBAL_TIME_H_
#define GLOBAL_TIME_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_global_time(void);
void deinit_global_time(void);
void restart_global_time(void);
uint32_t get_current_global_time_sec(void);
uint32_t get_elapsed_global_time_sec(uint32_t start_time_sec);

#endif /* GLOBAL_TIME_H_ */
