#ifndef PERIODS_H
#define PERIODS_H
/*
0 2 4 0
0 2 4 0
4 5 7 -
! ! ! !
4 5 7 -

*/
float frequencies[32] = {
	440.00, 493.88, 554.37, 440.00, //0240
	440.00, 493.88, 554.37, 440.00, //0240
	554.37, 587.33, 659.25, 554.37, //4574
	587.33, 659.25, 659.25, 739.99, //5779
	659.25, 587.33, 554.37, 440.00, //7540
	659.25, 739.99, 659.25, 587.33, //7975
	554.37, 440.00, 440.00, 329.63, //400-5
	440.00, 440.00, 329.63, 440.00 //00-50
};
const int myIndex[32]={
	0,2,4,0,
	0,2,4,0,
	4,5,7,
	4,5,7,
	7,9,7,5,4,0,
	7,9,7,5,4,0,
	0,-5,0,
	0,-5,0	
};
const float periods[25] = {0.00202478, 0.0019111,  0.00180388, 0.00170265, 0.00160705, 0.00151685,
 0.00143172, 0.00135139, 0.00127551, 0.00120395, 0.00113636, 0.00107259,
 0.00101239, 0.00095557, 0.00090192, 0.00085131, 0.00080354, 0.00075844,
 0.00071586, 0.00067568, 0.00063776, 0.00060197, 0.00056818, 0.00053629,
 0.00050619};

const int beats[32] = {2,2,2,2,2,
    2,2,2,2,2,
    4,2,2,4,1,
    1,1,1,2,2,
    1,1,1,1,2,
    2,2,2,4,2,
    2,4};


#endif