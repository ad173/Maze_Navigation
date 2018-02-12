#ifndef __MY_ENCODERS__
#define __MY_ENCODERS__

class eclass{
  public:
    eclass();
  
    //this function sets the tick counts to 0
    void resetCounts();

    //this function should return the left and right tickcounts 
    // since either the start of the program or the last
    //call to the function resetCounts()
    void getCounts(int* counts);

    //this function should return the instantaneous speeds of the wheels
    //meassured in revolutions per second.
    void getSpeeds(float* speeds);

    //this function should include whatever code necessary to initialize this module
    void initEncoders();

    void ReadEncoderValues();
    void resetSpeed(int LR);
  private:
    
};

extern eclass encoders;

#endif
