
/*
User guide:
Compile and upload the .s19 file to the experiment board, type "go 20000000" to start the execution.
Use keyboard to input or clear numbers for three-history function. Type positive or negative numbers on keyboard and type an 'e' to represent the end of input. 
After inputing each number, the application will print out the current input, the sum and the median of the numbers.
'F' is used to clear the history of numbers. 

Example input:
F 13e -7e 20e -1e

Example output:
Rcv: ’F’
The 3-history has been erased
Rcv: ’1’
Rcv: ’3’
Rcv: ’e’
Entered integer 13: sum = 13, median = 13
Rcv: ’-’
Rcv: ’7’
Rcv: ’e’
Entered integer -7: sum = 6, median = 3
Rcv: ’2’
Rcv: ’0’
Rcv: ’e’
Entered integer 20: sum = 26, median = 13
Rcv: ’-’
Rcv: ’1’
Rcv: ’e’
Entered integer -1: sum = 12, median = -1
*/

#include "TinyTimber.h"
#include "sciTinyTimber.h"
#include "canTinyTimber.h"
#include <stdlib.h>
#include <stdio.h>
#include "periods.h"

#define DAC_port ((volatile unsigned char*) 0x4000741C)

typedef struct {
    Object super;
    int count;
    char c[100];
    int nums[3];
    int nums_count ;
} App;

/*Task for loop load runnning in background*/
typedef struct {
    Object super;
    int loop_number;
} Bg_Loop;

typedef struct {
    Object super;
	int flag;
	int volumn;
}Sound;

App app = { initObject(), 0, 'X', {0},0  };
Sound generator = { initObject(), 0 , 5};
Bg_Loop load =  { initObject(), 1000};
void reader(App*, int);
void receiver(App*, int);
void three_history(App *,int);

Serial sci0 = initSerial(SCI_PORT0, &app, reader);

Can can0 = initCan(CAN_PORT0, &app, receiver);

void receiver(App* self, int unused)
{
    CANMsg msg;
    CAN_RECEIVE(&can0, &msg);
    SCI_WRITE(&sci0, "Can msg received: ");
    SCI_WRITE(&sci0, msg.buff);
}


void volume_control (Sound* self, int inc){
	if(inc==1&&self->volumn<10)
		self->volumn ++;
	else if (inc==0&&self->volumn>1)
		self->volumn --;
}	
void load_control (Bg_Loop* self, int inc){
	if(inc==1&&self->loop_number<8000)
		self->loop_number += 500;
	else if (inc==0&&self->loop_number>1000)
		self->loop_number -= 500;
}	
void mute (Sound* self){
	if(self->volumn == 0){
		self->volumn = 5;
	}else{
		self->volumn = 0;
	}	
}
void startLoop(Bg_Loop* self,int arg){
	for(int i=0;i<self->loop_number;i++){
		
	}
	AFTER(USEC(1300),self,startLoop,0);
}
void startSound(Sound* self, int arg){
    self->flag = !self->flag;
	if(self->flag){
		*DAC_port = self->volumn;
	}else{
		*DAC_port = 0x00;
	}
    AFTER(USEC(650),self,startSound,0);
}


void reader(App* self, int c)
{
     SCI_WRITE(&sci0, "Rcv: \'");
     SCI_WRITECHAR(&sci0,c);
     SCI_WRITE(&sci0, "\'\n");
	 int num;
	switch(c) {
		case 'e':
			
			self->c[self->count] = '\0';
			num = atoi(self->c);
	   
			self->count = 0;
		   // print_key(num);
		   break;
		case 'u':
			volume_control(&generator,1);
			break;
		
		case'd':
			//SCI_WRITE(&sci0, "Down is pressed");
			volume_control(&generator,0);
			break;
		case 'm':
			mute(&generator);
			break;
		case 'w':
			load_control(&load,1);
			break;
		case 's':
			load_control(&load,0);
			break;
		
	}
	if ((c >='0'&&c<='9') || (self->count==0 && c == '-')){
        self->c[self->count++] = c;
	}
}
void startApp(App* self, int arg)
{
    CANMsg msg;

    CAN_INIT(&can0);
    SCI_INIT(&sci0);
    SCI_WRITE(&sci0, "Hello, hello...\n");

    msg.msgId = 1;
    msg.nodeId = 1;
    msg.length = 6;
    msg.buff[0] = 'H';
    msg.buff[1] = 'e';
    msg.buff[2] = 'l';
    msg.buff[3] = 'l';
    msg.buff[4] = 'o';
    msg.buff[5] = 0;
    CAN_SEND(&can0, &msg);
	ASYNC(&generator,startSound,0);
	ASYNC(&load,startLoop,0);

}

int main()
{
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
    INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}
