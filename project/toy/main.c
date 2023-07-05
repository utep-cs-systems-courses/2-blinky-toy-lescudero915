#include <msp430.h>
#include "libTimer.h"

#define LED_GREEN BIT0             // P1.0
#define LED_RED BIT6               // P1.6
#define LEDS (BIT0 | BIT6)

#define SW1 BIT3		   /* switch1 is p1.3 */
#define SWITCHES SW1		   /* only 1 switch on this board */

void buzzer_init(){
  timerAUpmode();
  P2SEL2 &= ~(BIT6 | BIT7);
  P2SEL &= ~BIT7;
  P2SEL |= BIT6;
  P2DIR =BIT6;
}
void buzzer_set_period(short cycles){
  CCR0 = cycles;
  CCR1 = cycles >> 1;
}
void main(void){  
  configureClocks();
  P1DIR |= LEDS;
  P1OUT &= ~LEDS;
  P1REN |= SWITCHES;
  P1IE |= SWITCHES;
  P1OUT |= SWITCHES;
  P1DIR &= ~SWITCHES;
  or_sr(0x18);
} 
void switch_interrupt_handler(){
  char p1val = P1IN;                
  /* update switch interrupt sense to detect changes from current buttons */
  P1IES |= (p1val & SWITCHES);	    /* if switch up, sense down */
  P1IES &= (p1val | ~SWITCHES);	    /* if switch down, sense up */ 
  if(p1val & SW1){
    /* If I let go of button: */
    P1OUT |= LED_RED;
    P1OUT &= ~LED_GREEN;
  }else{
    /* If I press button: */
    enableWDTInterrupts();
  }
}
int blinkLimit = 100;
int blinkCount = 0;
int secondCount = 0;
void __interrupt_vec(WDT_VECTOR) WDT(){
  blinkCount++;
  if(blinkCount >= blinkLimit){
    blinkCount = 0;
    P1OUT |= LED_GREEN;
    P1OUT |= LED_RED;
    buzzer_init();
    buzzer_set_period(1000);
  }else{
    P1OUT &= ~LED_GREEN;
    P1OUT &= ~LED_RED;
  }
}
/* RESET BUTTON (S2) */
void __interrupt_vec(PORT1_VECTOR) Port_1(){
  if(P1IFG & SWITCHES){
    P1IFG &= ~SWITCHES;
    switch_interrupt_handler();
  }
}
