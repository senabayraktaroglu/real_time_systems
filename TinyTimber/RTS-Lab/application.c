#include "TinyTimber.h"
#include "sciTinyTimber.h"
#include "canTinyTimber.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    Object super;
    int count;
    char c[100];
    int nums[3];
    int nums_count ;

} App;

App app = { initObject(), 0, 'X', {0},0  };

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
void three_history(App *self,int num){
    int sum = 0;
    
    if ( self -> nums_count < 3){
       self -> nums[self->nums_count] = num;
       self -> nums_count ++; 
       
    }else{
        self-> nums[0] = num;
    }
    
    for(int i= 0 ; i < self-> nums_count ; i ++)
    {
        sum += self -> nums[i];
    }
    int median = 0;

    if ( self -> nums_count == 1)
        median = self -> nums[0];
    else if ( self -> nums_count == 2)
        median = (self -> nums[0] + self -> nums[1])/2;
    else{ 
        for(int i = 0 ; i < 3 ; i ++)
        {
            if ((self -> nums [(i-1) % 3] <=  self -> nums [(i) % 3])
                && (self -> nums [(i) % 3] <=  self -> nums [(i+1) % 3]))
                {
                    median = self -> nums [i];
                    break;
                }
            if ((self -> nums [(i-1) % 3] >=  self -> nums [(i) % 3])
                && (self -> nums [(i) % 3] >=  self -> nums [(i+1) % 3]))
                {
                    median = self -> nums [i];
                    break;
                }    
        }
    }   
    char output[200];
    snprintf(output,200,"Entered interger %d: sum = %d, median = %d \n",num,sum,median);

    SCI_WRITE(&sci0,output);
    
    
    
   
    
}
void reader(App* self, int c)
{

    if(c == 'e') {
        int num;
        self->c[self->count] = '\0';
        num = atoi(self->c);
        
        SCI_WRITE(&sci0, "Rcv: \'");
        SCI_WRITE(&sci0, self->c);

        SCI_WRITE(&sci0, "\'\n");
        self->count = 0;
        three_history(self,num);
        
    } else if ((c >='0'&&c<='9') || (self->count==0 && c == '-')){
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
}

int main()
{
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
    INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&app, startApp, 0);
    return 0;
}
