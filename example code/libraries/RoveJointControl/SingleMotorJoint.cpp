#include "SingleMotorJoint.h"

//constructor for single motor joints with feedback device
//inputType: What kind of movement this joint should be controlled by, such as speed or position input.
//alg: The IOAlgorithm to be used by this joint
//cont: The output device controlling the motor on this joint
//feed: The feedback device used with this joint
SingleMotorJoint::SingleMotorJoint(ValueType inputType, IOAlgorithm *alg, OutputDevice* cont, FeedbackDevice* feed) : JointInterface()
{
	//assignments
	inType = inputType;
	controller1 = cont;
	feedback = feed;
	manip = alg;
	manip -> setFeedDevice(feed);
  
  algorithmUsed = true;
  
  //checks to make sure the passed arguments all work with each other, that is that the algorithm's input type is the same as what the user is putting in, and
  //that the algorithm's output value type is what the output device expects to take in, etc
  if((inputType == alg->inType) && (cont->inType == alg->outType) && (alg->feedbackInType == feed->fType))
  {
    validConstruction = true;
  }
  else
  {
    validConstruction = false;
  }
}

//constructor for single motor joints without feedback or an IO algorithm
//inputType: What kind of movement this joint should be controlled by, such as speed or position input.
//cont: The output device controlling the motor on this joint
//feed: The feedback device used with this joint
SingleMotorJoint::SingleMotorJoint(ValueType inputType, OutputDevice* cont) : JointInterface()
{
	//assignments
	inType = inputType;
	controller1 = cont;

	algorithmUsed = false;

  //checks to make sure the passed arguments all work with each other, that is that device's input type is the same as what the user is putting in
  if(inputType == cont->inType)
  {
    validConstruction = true;
  }
  else
  {
    validConstruction = false;
  }
}

SingleMotorJoint::~SingleMotorJoint()
{
  //deliberately left blank; we don't want to destroy the pointers to other classes as the main program could still be handling them
}

//run the output algorithm for this tilt joint correctly (I mean, hopefully).
//calls the algorithm to manipulate the input and sends it to the motor device.
//input: a long that represents the desired movement. What values this int is constrained to depends on what this joint was set up to take as an input.
//For example, if this joint runs off of speed input then the values are constrained between SPEED_MIN and SPEED_MAX, and otherwise for the similar
//ranges defined in the framework's .h file
//returns: The status of attempting to control this joint. Such as if the output is now running, or if it's complete, or if there was an error
JointControlStatus SingleMotorJoint::runOutputControl(const long movement)
{
	long mov; //var used as interrum value since algorithm can change the value
  bool motionComplete;
  JointControlStatus returnStatus;

  if(!enabled)
  {
    returnStatus = DeviceDisabled;
  }
  else if(verifyInput(movement) == false)
  {
    returnStatus = InvalidInput;
  }

  else if(validConstruction)
  {
  	//calls algorithm if there's one used. If not, output passed directly to output device
    if(algorithmUsed)
    {
      mov = manip->runAlgorithm(movement, &motionComplete);
    }
    else
    {
      mov = movement;
      motionComplete = true;
    }
    
    //if motionComplete returned false but movement is 0, that's an indication that an error state occured
    if(motionComplete == false && mov == 0)
    {
      returnStatus = InvalidInput;
    }
    
    else if(motionComplete == true)
    {
      returnStatus = OutputComplete;
    }
    else
    {
      returnStatus = OutputRunning;
    }
    
  	//moves device with output decided on by the algorithm
  	controller1->move(mov);
  }

  //if this joint wasn't properly constructed, nothing is run
  else
  {
    returnStatus = InvalidConstruction;
  }

	return(returnStatus);
}

void SingleMotorJoint::stop()
{
  controller1->stop();
}

void SingleMotorJoint::disableJoint()
{
  controller1->setPower(false);
  enabled = false;
}

void SingleMotorJoint::enableJoint()
{
  controller1->setPower(true);
  enabled = true;
}
