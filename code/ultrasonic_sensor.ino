#define LOG_OUT 1 //use the log output function
#define FFT_N 256 //set to 256 point fft
#include <FFT.h> //include the library

volatile int counter = 0;
volatile int16_t adcVal[257];
uint8_t CTRLC_val;
uint8_t CTRLA_val;
uint8_t MUXPOS_val;
uint8_t bin1 = 58; 
uint8_t bin2 = 59; 
uint8_t bin3 = 60; 
uint8_t max_fft;
int enA = 10; 
int in1 = 2; 
int in2 = 3;
int enB = 9; 
int in3 = 5; 
int in4 = 4; 
int motorSpeedMedForwardL = 110;
int motorSpeedMedForwardR = 110;
const int triggerPIN = 8;
const int echoPIN = 7;
float soundDuration, distToObjectInCM;
uint8_t flag = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  pinMode(triggerPIN, OUTPUT);
  pinMode(echoPIN, INPUT);
  digitalWrite(triggerPIN, LOW);
  /* Set LED pin as OUTPUT and turn it OFF */
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  /* Store state of CTRLC, CTRLA, MUXPOS registers */
  CTRLC_val = ADC0.CTRLC;
  CTRLA_val = ADC0.CTRLA;
  MUXPOS_val = ADC0.MUXPOS;
  /* Disable digital input buffer */
  PORTD.PIN4CTRL &= ~PORT_ISC_gm; 
  PORTD.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  /* Disable pull-up resistor */
  PORTD.PIN4CTRL &= ~PORT_PULLUPEN_bm;
  ADC0.CTRLC = ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc;
  ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;
  /* Select ADC channel */
  ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc;
  /* Enable FreeRun mode */
  /* CLK_PER divided by 4 */
  /* Internal reference */
  /* ADC Enable: enabled */ /* 10-bit mode */
  ADC0.CTRLA |= ADC_FREERUN_bm;
  
  ADC0.COMMAND = ADC_STCONV_bm; 

  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;  

  TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);  

  TCA0.SINGLE.PER = 0x67; //0.00041667*16000000/64-1 
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;

  sei();
}

ISR(TCA0_OVF_vect) {
  /* read the ADC value */
  adcVal[counter] = ADC0_read();
  /* increment counter */
  counter = counter + 1;
  /* clear interrupt flag */
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

int ADC0_read(void) {
  /* Clear the interrupt flag by writing 1: */
  ADC0.INTFLAGS = ADC_RESRDY_bm;
  return ADC0.RES;
}

void loop() {
  if (counter > 256) {
    /* Disable TCA interrupts */
    TCA0.SINGLE.INTCTRL = 0;  //disable TCA interrupts

    
    //convert to signed 16 bit
    for(int i = 0; i< 256; i++) {
      
      //subtracting the offset to include negative values 
      adcVal[i+1] = adcVal[i+1] - 512;
      
      //shifting the value to convert it into a 16 bit number 
      adcVal[i+1] = adcVal[i+1] << 6;
    }

    for(int i = 0; i <= 512; i++) {
      if (i%2!=0) {
        fft_input[i] = 0;
      }
      else {
        fft_input[i] = adcVal[(i/2) + 1];
        
      }
    }
    fft_window();
    fft_reorder();
    fft_run();
    fft_mag_log();

    if ((fft_log_out[bin1] + fft_log_out[bin2] + fft_log_out[bin3]) > 250) {
      /* Restore state of ADC registers */
      ADC0.CTRLC = CTRLC_val;
      ADC0.CTRLA = CTRLA_val;
      ADC0.MUXPOS = MUXPOS_val;

      digitalWrite(triggerPIN, HIGH);
      analogWrite(enA, motorSpeedMedForwardL);
      analogWrite(enB, motorSpeedMedForwardR);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      unsigned long current = millis();
      unsigned long start = millis();
      while (current - start <= 5000) {
        Serial.println("Moving the motors");
        digitalWrite(triggerPIN, LOW);
        soundDuration = pulseIn(echoPIN, HIGH);
        unsigned long current_pulse = millis();
        unsigned long start_pulse = millis();
        while(current_pulse-start_pulse<=60){
          current_pulse = millis();
        }
        digitalWrite(triggerPIN, HIGH);
        distToObjectInCM = (soundDuration * 0.0343) / 2;
        if ((distToObjectInCM <= 10 && distToObjectInCM >= 5) || (distToObjectInCM <= 40 && distToObjectInCM >= 50)) {
          digitalWrite(LED_BUILTIN, HIGH);
        }
        else {
          digitalWrite(LED_BUILTIN, LOW);
        }
        Serial.println("distToObjectInCM: ");
        Serial.println(distToObjectInCM);
        current = millis();
      }
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
    }
    else {
      // if 550Hz not detected
      TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
      //enable interrupts again
      counter = 0;
    }
  }
}
