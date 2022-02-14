/*back up for part 0, three history function and reader*/

void three_history(App *self,int num){
    int sum = 0;
    
    if ( self -> nums_count < 3){
       self -> nums[self->nums_count] = num;
       self -> nums_count ++;
       
    }else{
        self-> nums[0] = num;
        //made swap if more than 3 elements added
        int temp = 0;
        temp = self -> nums[0];
        self-> nums[0] = self -> nums[1];
        self -> nums[1] = self -> nums[2];
        self -> nums[2] = temp;
    }
    
    //calculate sum in array
    for(int i= 0 ; i < self-> nums_count ; i ++)
    {
        sum += self -> nums[i];
    }
    
    //calculate median based on num of elements in array
    int median = 0;
    if ( self -> nums_count == 1)
        median = self -> nums[0];
    else if ( self -> nums_count == 2)
        median = (self -> nums[0] + self -> nums[1])/2;
    else{
        for(int i = 0 ; i < 3 ; i ++)
        {
            if ((self -> nums [(i+2) % 3] <=  self -> nums [(i) % 3])
                && (self -> nums [(i) % 3] <=  self -> nums [(i+1) % 3]))
                {
                    median = self -> nums [i];
                    break;
                }
            if ((self -> nums [(i+2) % 3] >=  self -> nums [(i) % 3])
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
    SCI_WRITE(&sci0, "Rcv: \'");
    SCI_WRITECHAR(&sci0, c);

    SCI_WRITE(&sci0, "\'\n");
    if(c == 'e') {
        int num;
        self->c[self->count] = '\0';
        num = atoi(self->c);
        self->count = 0;
        // call three history each time a number is added
        three_history(self,num);
        
    } else if ((c >='0'&&c<='9') || (self->count==0 && c == '-')){
        self->c[self->count++] = c;
    } else if (c=='F'){
        self -> nums_count = 0;
         SCI_WRITE(&sci0, "The 3-history has been erased \n'");
    }
}
