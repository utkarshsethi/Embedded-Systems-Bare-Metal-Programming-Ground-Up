#ifndef	ADC_H_
  #define  ADC_H_

#include <stdint.h>

//Expose Functions
  void adc_pa1_init(void);
  void start_conversion(void);
  uint32_t adc_read (void);

#endif /* ADC_H_ */