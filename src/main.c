#include "gd32f1x0.h"
#include "gd32f1x0_libopt.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>

//Use USART1 on PA2, PA3 for connection on main board
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCU_USART1

#define EVAL_COM1_TX_PIN                 GPIO_PIN_2
#define EVAL_COM1_RX_PIN                 GPIO_PIN_3

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_1

//Use USART1 on PA2, PA3 for connection on main board
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCU_USART1

#define EVAL_COM1_TX_PIN                 GPIO_PIN_2
#define EVAL_COM1_RX_PIN                 GPIO_PIN_3

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_1

void init_uart() {
  rcu_periph_clock_enable(EVAL_COM_GPIO_CLK);
  /* enable USART clock */
  rcu_periph_clock_enable(EVAL_COM1_CLK);

  /* connect port to USARTx_Tx */
  gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, EVAL_COM1_TX_PIN);

  /* connect port to USARTx_Rx */
  gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, EVAL_COM1_RX_PIN);

  /* configure USART Tx as alternate function push-pull */
  gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                EVAL_COM1_TX_PIN);
  gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP,
                          GPIO_OSPEED_10MHZ, EVAL_COM1_TX_PIN);

  /* configure USART Rx as alternate function push-pull */
  gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                EVAL_COM1_RX_PIN);
  gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP,
                          GPIO_OSPEED_10MHZ, EVAL_COM1_RX_PIN);

  /* USART configure */
  usart_deinit(EVAL_COM1);
  /* 8N1 @ 115200 baud (standard) */
  usart_baudrate_set(EVAL_COM1, 9600U);
  usart_parity_config(EVAL_COM1, USART_PM_NONE);
  usart_word_length_set(EVAL_COM1, USART_WL_8BIT);
  usart_stop_bit_set(EVAL_COM1, USART_STB_1BIT);
  usart_transmit_config(EVAL_COM1, USART_TRANSMIT_ENABLE);
  usart_receive_config(EVAL_COM1, USART_RECEIVE_DISABLE);
  usart_enable(EVAL_COM1);
}

void print_str(const char* str) {
  for (size_t i = 0; i < strlen(str); i++) {
    usart_data_transmit(EVAL_COM1, str[i]);
    //wait for transmission buffer empty (TBE) before sending next data
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE)) {
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

int main(void) {
  SystemInit();
  systick_config();

  fwdgt_config(0x0fff, FWDGT_PSC_DIV4);
  fwdgt_enable();

  // rcu_periph_clock_enable(RCU_GPIOA);
  // gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
  // gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
  // GPIO_PIN_0);
  // GPIO_OCTL(GPIOA) ^= GPIO_PIN_0;

  init_uart();

  if (rcu_flag_get(RCU_FLAG_V12RST)) print_str("V12 reset\r\n");
  if (rcu_flag_get(RCU_FLAG_OBLRST)) print_str("OBL reset\r\n");
  if (rcu_flag_get(RCU_FLAG_EPRST)) print_str("EPR reset\r\n");
  if (rcu_flag_get(RCU_FLAG_PORRST)) print_str("Power reset\r\n");
  if (rcu_flag_get(RCU_FLAG_SWRST)) print_str("SW reset\r\n");
  if (rcu_flag_get(RCU_FLAG_FWDGTRST)) print_str("FWDGT reset\r\n");
  if (rcu_flag_get(RCU_FLAG_WWDGTRST)) print_str("WWDGT reset\r\n");
  if (rcu_flag_get(RCU_FLAG_LPRST)) print_str("LP reset\r\n");
  rcu_all_reset_flag_clear();

  char buf[20];
  for (int i = 0; i < 10000; i++) {
    delay(1000);
    sprintf(buf, "%d\r\n", i);
    print_str(buf);
    fwdgt_counter_reload();
  }
  return 0;
}