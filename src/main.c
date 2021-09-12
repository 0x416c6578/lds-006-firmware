#include "gd32f1x0.h"
#include "gd32f1x0_libopt.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>

void initUART1() {
  rcu_periph_clock_enable(RCU_GPIOA);
  /* Enable USART clock */
  rcu_periph_clock_enable(RCU_USART1);
  /* Connect port to USARTx_Tx */
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);
  /* Connect port to USARTx_Rx */
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);
  /* Configure USART Tx as alternate function push-pull */
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
                GPIO_PIN_2);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP,
                          GPIO_OSPEED_10MHZ, GPIO_PIN_2);
  /* Configure USART Rx as alternate function push-pull */
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
                GPIO_PIN_3);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP,
                          GPIO_OSPEED_10MHZ, GPIO_PIN_3);
  /* USART configure */
  usart_deinit(USART1);
  usart_baudrate_set(USART1, 9600U);
  usart_parity_config(USART1, USART_PM_NONE);
  usart_word_length_set(USART1, USART_WL_8BIT);
  usart_stop_bit_set(USART1, USART_STB_1BIT);
  usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
  usart_receive_config(USART1, USART_RECEIVE_DISABLE);
  usart_enable(USART1);
}

void printStrUSART1(const char* str) {
  for (size_t i = 0; i < strlen(str); i++) {
    usart_data_transmit(USART1, str[i]);
    //wait for transmission buffer empty (TBE) before sending next data
    while (RESET == usart_flag_get(USART1, USART_FLAG_TBE)) {
    }
  }
}

/*
Quick and dirty function for arbitrary time delay
that will feed FWDGT automatically, assuming minimum
prescalar value of /4
*/
void delay(uint32_t millis) {
  fwdgt_counter_reload();
  uint32_t _millis = millis;
  if (_millis < 200) {
    delay_1ms(_millis);
    return;
  } else {
    while (1) {
      fwdgt_counter_reload();
      if (_millis < 200) {
        delay_1ms(_millis);
        return;
      } else {
        delay_1ms(200);
        _millis -= 200;
      }
    }
  }
}

void printResetSource(void) {
  printStrUSART1("Reset source: ");
  if (rcu_flag_get(RCU_FLAG_V12RST)) printStrUSART1("V12 reset\r\n");
  if (rcu_flag_get(RCU_FLAG_OBLRST)) printStrUSART1("OBL reset\r\n");
  if (rcu_flag_get(RCU_FLAG_EPRST)) printStrUSART1("EPR reset\r\n");
  if (rcu_flag_get(RCU_FLAG_PORRST)) printStrUSART1("Power reset\r\n");
  if (rcu_flag_get(RCU_FLAG_SWRST)) printStrUSART1("SW reset\r\n");
  if (rcu_flag_get(RCU_FLAG_FWDGTRST)) printStrUSART1("FWDGT reset\r\n");
  if (rcu_flag_get(RCU_FLAG_WWDGTRST)) printStrUSART1("WWDGT reset\r\n");
  if (rcu_flag_get(RCU_FLAG_LPRST)) printStrUSART1("LP reset\r\n");
  rcu_all_reset_flag_clear();
}

int main(void) {
  //Init system and systick
  SystemInit();
  systick_config();

  //(Re)init free watchdog with lowest prescalar
  fwdgt_config(0x0fff, FWDGT_PSC_DIV4);
  fwdgt_enable();

  initUART1();
  printResetSource();

  //So anyways, I started counting
  char buf[20];
  for (int i = 0; i < 10000; i++) {
    delay(1000);
    sprintf(buf, "%d\r\n", i);
    printStrUSART1(buf);
    fwdgt_counter_reload(); //Don't forget to feed the watchdog
  }

  return 0;
}