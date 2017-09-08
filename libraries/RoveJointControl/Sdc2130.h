#ifndef SDC2130_H_
#define SDC2130_H_

#include "AbstractFramework.h"

class Sdc2130: public OutputDevice
{
  private:
    int PWM_PIN, TX_PIN, RX_PIN;
    int pwmVal;
    int currentPower;
    long currentPosition;
    
    const int PWM_MIN = 0, PWM_MAX = 255;
    const int POS_INC = 2;

    enum ControlTypes {Pwm, c_Serial};
    ControlTypes controlType;

  protected:

    //general move command. Selects the specific move function.
    //Input: Can be either position or power values constrained between POWER_MIN and POWER_MAX or POS_MIN and POS_MAX
    void move(const long movement);

    //move command if inputting speed, input movement is in the range of POWER_MIN to POWER_MAX
    //Simply causes the joint to move a couple of positional ticks, as true speed based movement is not implemented due to strange delays in sdc2130 response
    void moveSpeed(const int movement);

    //move command if inputting position, input movement is in the range of POS_MIN to POS_MAX
    //void movePos(const long movement); not implemented
    
    //tells the device to power on or off.
    void setPower(bool powerOn);
    
    //tells device to stop moving
    void stop();

  public:
    //constructor for control via pwm
    //inputs:
    //pwmPin: GPIO pin that's connected to the pwm input of the SDC2130 device. GPIO pin number id's are defined by energia's pinmapping
    //inType: instance of the ValueType enum that defines what kind of input the device should take, currently pos or spd. The motor controller can move based on either, so pick one
    //upside down: whether or not the motor is mounted in reverse so the input values would also need to be inverted
    Sdc2130(const int pwmPin, ValueType inType, bool upsideDown);

    //constructor for controlling it via serial
    //Sdc2130(const int txPin, const int rxPin, ValueType inType, bool upsideDown); not implemented
    
    //returns how quickly the device is moving if it's set up to use speed control, 
    //or what it's destination position is if it's set up to use positional control
    long getCurrentMove();
};

#endif
