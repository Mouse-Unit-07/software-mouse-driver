/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : fault_detector.h                                      */
/*                                                                            */
/* Interface for fault_detector library                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef FAULT_DETECTOR_H_
#define FAULT_DETECTOR_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_fault_detector(void);
void deinit_fault_detector(void);
bool is_there_hardware_fault(void);
void print_hardware_state(void);

#endif /* FAULT_DETECTOR_H_ */
