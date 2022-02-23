
/*
User guide:
 
 Compile and upload the .s19 file to the experiment board, type "go" to start the execution.
 
 VOLUME CONTROL
 
 - For increasing volume press "q"
 - For decreasing press "a"
 
 MUTE CONTROL
 
 - For mute and unmute press "m"
 
 LOAD CONTROL
 
 - For increasing background load press "w"
 - For decreasing background load press "s"
 
 DEADLINE CONTROL
  
 - For enable and disable deadline press "d"
 
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

/* Application class for background loop
*  loop number: number of iterations for the empty loop
* deadline_enabled: the deadline enabled state, 0 for false(disabled) 1 for true(enabled)
*/
typedef struct {
    Object super;
    int loop_number;
	int  deadline_enabled;
} Bg_Loop;

/* Application class for sound generator
*  flag: whether to write 0 or 1 to the DAC port
* volumn: the current output volumn for sound generator
* pre_volumn: keeps the previous volumn before muted
* deadline_enabled: the deadline enabled state, 0 for false(disabled) 1 for true(enabled)
*/
typedef struct {
    Object super;
	int flag;
	int volumn;
	int prev_volumn;
	int  deadline_enabled;
}Sound;

App app = { initObject(), 0, 'X', {0},0  };
Sound generator = { initObject(), 0 , 10,0,0};
Bg_Loop load =  { initObject(), 1000,0};
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
	if(inc==1&&self->volumn<20)
		self->volumn ++;
	else if (inc==0&&self->volumn>1)
		self->volumn --;
}	
void load_control (Bg_Loop* self, int inc){
	if(inc==1){
	//&&self->loop_number<8000)
		self->loop_number += 500;
		char loopmsg[100];
		snprintf(loopmsg,100,"loop number is  %d \n",self->loop_number);
		 SCI_WRITE(&sci0,loopmsg);
	}
	else if (inc==0&&self->loop_number>1000){
		self->loop_number -= 500;	
		char loopmsg[100];
		snprintf(loopmsg,100,"loop number is  %d \n",self->loop_number);
		 SCI_WRITE(&sci0,loopmsg);
	}
}	
void mute (Sound* self){
	if(self->volumn == 0){
		self->volumn = self->prev_volumn;
	}else{
		self->prev_volumn = self->volumn;
		self->volumn = 0;
	}	
}
void startLoop(Bg_Loop* self,int arg){
	for(int i=0;i<self->loop_number;i++){
		
	}
	if(self->deadline_enabled){
		SEND(USEC(1300),USEC(1300),self,startLoop,0);
	}else{
		AFTER(USEC(1300),self,startLoop,0);
	}
}
/* 1kHZ : 500us  
769HZ: 650us
537HZ: 931us
*/
void startSound(Sound* self, int arg){
    self->flag = !self->flag;
	if(self->flag){
		*DAC_port = self->volumn;
	}else{
		*DAC_port = 0x00;
	}
	if(self->deadline_enabled){
		SEND(USEC(500),USEC(100),self,startSound,0);
	}else{
    	AFTER(USEC(500),self,startSound,0);
	}
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
		case 'q':
			//volume_control(&generator,1);
			ASYNC(&generator,volume_control,1);
			break;
		
		case 'a':
			//SCI_WRITE(&sci0, "Down is pressed");
			//volume_control(&generator,0);
			ASYNC(&generator,volume_control,0);
			break;
		case 'm':
			//mute(&generator);
			ASYNC(&generator,mute,0);
			break;
		case 'w':
			//load_control(&load,1);
			ASYNC(&load,load_control,1);
			break;
		case 's':
			//load_control(&load,0);
			ASYNC(&load,load_control,0);
			break;
		case 'd':
			if(load.deadline_enabled==0){
				load.deadline_enabled = 1;
				generator.deadline_enabled = 1;
				SCI_WRITE(&sci0, "Deadline is enabled \n");
			}else{
				load.deadline_enabled = 0;
				generator.deadline_enabled = 0;
				SCI_WRITE(&sci0, "Deadline is disabled \n");
			}
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