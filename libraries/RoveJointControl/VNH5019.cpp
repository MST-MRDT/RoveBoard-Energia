#include "VNH5019.h"
#include "RoveBoard.h"

VNH5019::VNH5019(const int PwmPin, const int InaPin, const int InbPin, bool upsideDown)
{
  PWM_PIN = PwmPin;
  INA_PIN = InaPin;
  INB_PIN = InbPin;
  currentPower = 0;
  inType = InputPower;
  invert = upsideDown;
  
  //brake motor by default
  digitalPinWrite(INA_PIN, LOW);
  digitalPinWrite(INB_PIN, LOW);
}

void VNH5019::move(const long movement)
{
  int mov = movement;
  int pwm = 0;
  
  if(enabled)
  {
    //if mounted upside down then invert the signal passed to it and move accordingly
    if (invert)
    {
      mov = -mov;
    }
    
    currentPower = mov;
    
    //if supposed to move backwards
    if(mov < 0)
    {
      mov = abs(mov);
      pwm = map(mov, 0, POWER_MAX, PWM_MIN, PWM_MAX);

      //set InB to 0 and InA to 1 for "reverse" rotation
      digitalPinWrite(INA_PIN, HIGH);
      digitalPinWrite(INB_PIN, LOW);
      
      //pulsate enable pin to control motor
      pwmWrite(PWM_PIN, pwm);
    }
    
    //if forwards
    else if(mov > 0)
    {
      pwm = map(mov, 0, POWER_MAX, PWM_MIN, PWM_MAX);
        
      //set InB to 1 and InA to 0 for forward rotation
      digitalPinWrite(INA_PIN, LOW);
      digitalPinWrite(INB_PIN, HIGH);
      
      //pulsate enable pin to control motor
      pwmWrite(PWM_PIN, pwm);
    }
    
    //stop
    else if(mov == 0)
    {
      pwmWrite(PWM_PIN, 0);//set all pins to 0 to brake motor
      digitalPinWrite(INA_PIN, LOW);
      digitalPinWrite(INB_PIN, LOW);
    }
  }
  
  return;
}

void VNH5019::setPower(bool powerOn)
{
  if(powerOn == false)
  {
    stop();
  }
  
  enabled = powerOn;
}

long VNH5019::getCurrentMove()
{
  if(invert) //if we're inverted, then we technically move negatively even if we're moving in the 'positive' direction. The direction is the important part
  {
    return(currentPower * -1); 
  }
  else
  {
    return(currentPower);
  }
}

void VNH5019::stop()
{
  pwmWrite(PWM_PIN, 0);//set all pins to 0 to brake motor
  digitalPinWrite(INA_PIN, LOW);
  digitalPinWrite(INB_PIN, LOW);
  
  currentPower = 0;
}
