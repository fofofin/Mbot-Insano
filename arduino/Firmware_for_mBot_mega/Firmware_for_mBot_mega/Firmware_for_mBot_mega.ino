#include <Arduino.h>
#include <MeMegaPi.h>
#include "MeEEPROM.h"
#include <Wire.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
//#define DEBUG_INFO
//#define DEBUG_INFO1
int deg=0;
String datos;
char bufferserial[10];
QueueHandle_t Q;
#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
//////////////////////////////////////////////////////////////////////////////////////

#define BARRIER_S2_PIN  A7
volatile bool colision = false;
void ISR_colision();

MeMegaPiDCMotor dc_motor;


MeMegaPiDCMotor motor_foward_left(PORT2B);
MeMegaPiDCMotor motor_foward_right(PORT1A);
MeMegaPiDCMotor motor_back_left(PORT2A);
MeMegaPiDCMotor motor_back_right(PORT1B);

void motor_foward_left_run(int16_t speed)
{
  motor_foward_left.reset(PORT2B);
  motor_foward_left.run(-speed*1.175);
}

void motor_foward_right_run(int16_t speed)
{
  motor_foward_right.reset(PORT1A);
  motor_foward_right.run(speed);
}

void motor_back_left_run(int16_t speed)
{
  motor_back_left.reset(PORT2A);
  motor_back_left.run(-speed);
}

void motor_back_right_run(int16_t speed)
{
  motor_back_right.reset(PORT1B);
  motor_back_right.run(speed);
}

// vx：left or right
// vy：forward or backward
// vw：clockwise or counterclockwise
void move_control(int16_t vx, int16_t vy, int16_t vw)
{
  int16_t foward_left_speed;
  int16_t foward_right_speed;
  int16_t back_left_speed;
  int16_t back_right_speed;

  foward_left_speed = vy + vx + vw;
  foward_right_speed = vy - vx - vw;
  back_left_speed = vy - vx + vw;
  back_right_speed = vy + vx - vw;

  motor_foward_left_run(foward_left_speed);    
  motor_foward_right_run(foward_right_speed);   
  motor_back_left_run(back_left_speed);   
  motor_back_right_run(back_right_speed);   
}

//nuestro codigo sin arduino
void angular_move(void *pvParameters){
  //move_control(0,100,0);
  BaseType_t qStatus;
  const TickType_t ticks=pdMS_TO_TICKS(100);
  int angulo;
  int w=0;
  while(1){
    if (colision) {
      move_control(0,0,0);
      vTaskDelay(ticks*2);

      move_control(0,0,120);
      vTaskDelay(pdMS_TO_TICKS(1300));

      move_control(0,0,0);
      vTaskDelay(ticks*2);

      xQueueReset(Q);

      colision = false;
    }
    qStatus = xQueueReceive(Q,&angulo,ticks);
    if (qStatus==pdPASS){
      if(angulo <= 0){
        w=-100;
      }
      if(angulo > 0){
        w=100;
      }
      if(angulo < 50 && angulo >= -50){
        move_control(0,100,0);
      }else{
        move_control(0,100,w);
      }
    }
  }
}

void lecturas(void *pvParameters){
  int index=0;
  bool linea_completa=false;
  TickType_t  Espera = xTaskGetTickCount();
  const TickType_t ticks=pdMS_TO_TICKS(100);
  while(1){
    while (UCSR0A & (1 << RXC0)){
      char recibido=UDR0;
      if (recibido == '\r'){
        continue;
      }
      if (recibido == '\n') {
        linea_completa = true;
      } 
      else if (index < 9) {
        bufferserial[index++] = recibido;
      }
    }
    if (linea_completa) {
      bufferserial[index] = '\0';
      deg=atoi(bufferserial);  
      xQueueSend(Q,&deg,ticks);
      index = 0; 
      linea_completa = false; 
    }
  }
}

ISR(ADC_vect)
{
  uint16_t lectura = ADC;
  if(lectura < 500){
    colision = true;
  }
  ADCSRA |= (1<<ADSC); 
}

void setup()
{
  
  ADMUX  = (1<<REFS0) | 7;      

  ADCSRA = (1<<ADEN) | (1<<ADIE)  | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

  ADCSRA |= (1<<ADSC);   
  Q=xQueueCreate(3,sizeof(int));
  xTaskCreate(angular_move, "Movimiento", 200, NULL, 1, NULL);
  xTaskCreate(lecturas, "LECTURA_SERIAL", 200, NULL, 1, NULL);

  UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
  UBRR0L = (uint8_t)UBRR_VALUE;
  UCSR0C = 0x06;
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
  
  //PWM
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22);
  TCCR3A = _BV(WGM30);
  TCCR3B = _BV(CS31) | _BV(CS30) | _BV(WGM32);
  TCCR4A = _BV(WGM40);
  TCCR4B = _BV(CS41) | _BV(CS40) | _BV(WGM42);
  sei();
}

void loop(){
}