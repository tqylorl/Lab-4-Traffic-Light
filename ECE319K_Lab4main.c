

/* ECE319K_Lab4main.c
 * Traffic light FSM
 * August 30, 2023
  */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"  // student's Lab 3
#include <stdio.h>
#include <string.h>
// put both EIDs in the next two lines
const char EID1[] = "TPL468"; //  ;replace abc123 with your EID
const char EID2[] = "EN6637"; //  ;replace abc123 with your EID


#define WHITE (RED|GREEN|BLUE)

// initialize all 6 LED outputs and 3 switch inputs
// assumes LaunchPad_Init resets and powers A and B
void Traffic_Init(void){
 IOMUX -> SECCFG.PINCM[PB2INDEX] = 0x00000081;
 IOMUX -> SECCFG.PINCM[PB1INDEX] = 0x00000081;
 IOMUX -> SECCFG.PINCM[PB0INDEX] = 0x00000081;

 IOMUX -> SECCFG.PINCM[PB8INDEX] = 0x00000081;
 IOMUX -> SECCFG.PINCM[PB7INDEX] = 0x00000081;
 IOMUX -> SECCFG.PINCM[PB6INDEX] = 0x00000081;

 IOMUX -> SECCFG.PINCM[PB17INDEX] = 0x00040081;
 IOMUX -> SECCFG.PINCM[PB16INDEX] = 0x00040081;
 IOMUX -> SECCFG.PINCM[PB15INDEX] = 0x00040081;

 IOMUX -> SECCFG.PINCM[PB27INDEX] = 0x00000081;
 IOMUX -> SECCFG.PINCM[PB26INDEX] = 0x00000081;
 IOMUX -> SECCFG.PINCM[PB22INDEX] = 0x00000081;
 GPIOB->DOE31_0 |= 0x62001C7;
 GPIOB->DOE31_0 |= WHITE;



}

/* Activate LEDs
* Inputs: west is 3-bit value to three east/west LEDs
*         south is 3-bit value to three north/south LEDs
*         walk is 3-bit value to 3-color positive logic LED on PB22,PB26,PB27
* Output: none
* - west =1 sets west green
* - west =2 sets west yellow
* - west =4 sets west red
* - south =1 sets south green
* - south =2 sets south yellow
* - south =4 sets south red
* - walk=0 to turn off LED
* - walk bit 22 sets blue color
* - walk bit 26 sets red color
* - walk bit 27 sets green color
* Feel free to change this. But, if you change the way it works, change the test programs too
* Be friendly*/
struct State{
    uint32_t outPin;
    uint32_t Time;
    uint32_t Next[8];
};
typedef const struct State State_t;
  #define gSouth 0
  #define ySouth 1
  #define rSouth 2

  #define gWest 3
  #define yWest 4
  #define rWest 5

  #define walkW 6
  #define walkOff1 7
  #define walkR1 8
  #define walkOff2 9
  #define walkR2 10
  #define walkOff3 11
  #define walkR3 12



State_t FSM[13]= {/*                 000,    001,   010,   011,   100,   101,  110,  111 */
                 {0x4000101, 100, {gSouth, ySouth,gSouth ,ySouth ,ySouth , ySouth, ySouth, ySouth}}, //green south

                 {0x4000102, 100, {rSouth, rSouth, rSouth, rSouth, rSouth, rSouth, rSouth, rSouth}}, // yellow south
                 {0x4000104, 100, {gWest,gWest, gWest, gWest,walkW,walkW,walkW, walkW}}, // red south



                 {0x4000044, 100, {gWest, gWest, yWest, yWest, yWest,yWest,yWest,yWest}}, // green west
                 {0x4000084, 100, {rWest, rWest, rWest, rWest, rWest, rWest, rWest, rWest}}, //yellow west
                 {0x4000104, 100, {gSouth, gSouth, gSouth, gSouth, walkW, walkW, walkW, walkW}}, //red south



                 {0xC400104, 100, {walkOff1, walkOff1, walkOff1, walkOff1, walkOff1, walkOff1, walkOff1, walkOff1}}, // walk white

                 {0x0000104, 100, {walkR1, walkR1, walkR1, walkR1, walkR1, walkR1, walkR1, walkR1}}, // walk off
                 {0x4000104, 100, {walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2}}, // walk red
                 {0x0000104, 100, {walkR2, walkR2, walkR2, walkR2, walkR2, walkR2, walkR2, walkR2}}, // walk off x2
                 {0x4000104, 100, {walkOff3, walkOff3, walkOff3, walkOff3, walkOff3, walkOff3, walkOff3, walkOff3}}, // walk red x2
                 {0x0000104, 100, {walkR3, walkR3, walkR3, walkR3, walkR3, walkR3, walkR3, walkR3}}, //walk off x3
                 {0x4000104, 100, {gSouth, gWest, gSouth, gWest, gSouth, gWest, gSouth, gWest}} //final red for walk

};


void Traffic_Out(uint32_t walk, uint32_t south, uint32_t west){
    GPIOB->DOUT31_0 = 0x0;
    GPIOB->DOUT31_0 = (south) | (west); // & 0x1C7;


        GPIOB->DOUT31_0 |= walk;

    if(walk == 0){
        GPIOB->DOUTCLR31_0 = 0x31<<22;

    }



}
/* Read sensors
 * Input: none
 * Output: sensor values
 * - bit 0 is west car sensor
 * - bit 1 is south car sensor
 * - bit 2 is walk people sensor
* Feel free to change this. But, if you change the way it works, change the test programs too
 */
uint32_t Traffic_In(void){


    uint32_t input = (GPIOB->DIN31_0 >> 15) & 0x7;


  return input;
}
// use main1 to determine Lab4 assignment
void Lab4Grader(int mode);
int main1(void){ // main1
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Lab4Grader(0); // print assignment, no grading
  while(1){
  }
}
// use main2 to debug LED outputs
 int main2(void){ // main2
  Clock_Init80MHz(0);
  LaunchPad_Init();
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  UART_OutString("Lab 4, Fall 2023, Step 1. Debug LEDs\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  while(1){
    for(uint32_t i=1; i<8; i = i<<1){ //1,2,4
      Traffic_Out(0,0,i); // Your Lab 4 output
      Debug_Dump(i);
      Clock_Delay(40000000); // 0.5s
    }
    for(uint32_t i=1; i<8; i = i<<1){ //1,2,4
      Traffic_Out(0,i,0); // Your Lab 4 output
      Debug_Dump(i<<3);
      Clock_Delay(40000000); // 0.5s
    }
    Traffic_Out(RED,0,0); // Your Lab 4 output
    Debug_Dump(RED);
    Clock_Delay(40000000); // 0.5s
    Traffic_Out(WHITE,0,0); // Your Lab 4 output
    Debug_Dump(WHITE);
    Clock_Delay(40000000); // 0.5s
  }
}
// use main3 to debug the three input switches
int main3(void){ // main3
  uint32_t last=0,now;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Traffic_Init(); // your Lab 4 initialization
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Fall 2023, Step 2. Debug switches\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  while(1){
    now = Traffic_In(); // Your Lab4 input
    if(now != last){ // change
      UART_OutString("Switch= 0x"); UART_OutUHex(now); UART_OutString("\n\r");
      Debug_Dump(now);
    }
    last = now;
    Clock_Delay(800000); // 10ms, to debounce switch
  }
}
// use main4 to debug using your dump
// proving your machine cycles through all states
int main4(void){// main4
uint32_t input;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Traffic_Init(); // your Lab 4 initialization
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Fall 2023, Step 3. Debug FSM cycle\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  UART_OutString("EID2= "); UART_OutString((char*)EID2); UART_OutString("\n\r");
  // initialize your FSM


  // Initialize SysTick for software waits
 SysTick_Init();

 uint32_t Pt;
  Pt = 0;
  uint32_t bit2=0;
    uint32_t bit1=0;
    uint32_t bit0=0;
    uint32_t Output=0;
    uint32_t temp=0;
    uint32_t temp2=0;

while(1){

    Output = FSM[Pt].outPin;
    bit0 = (Output&0x1C0);


    bit2 =  (Output & 0xC400000);

    bit1 = Output&0x07;




     // output using Traffic_Out
    Traffic_Out(bit2, bit1, bit0);
    // call your Debug_Dump logging your state number and output



    // wait
    input = 7;

   Clock_Delay(40000000);
   Pt = FSM[Pt].Next[input];



    // hard code this so input always shows all switches


    // next depends on state and input




  }
}
// use main5 to grade
 int main(void){// main5
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Traffic_Init(); // your Lab 4 initialization
  // initialize your FSM, SysTick
  Lab4Grader(1); // activate UART, grader and interrupts
  uint32_t Pt;
  uint32_t Input;
    Pt = 0;
    SysTick_Init();
  while(1){
     // output using Traffic_Out
     // wait
     // input using your Traffic_In
     // next depends on state and input


      GPIOB->DOUT31_0 = FSM[Pt].outPin;
      SysTick_Wait10ms(FSM[Pt].Time);
      Input = Traffic_In();
      Pt = FSM[Pt].Next[Input];









  }
}
