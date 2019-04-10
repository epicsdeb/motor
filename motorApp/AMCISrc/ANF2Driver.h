/*
FILENAME...   ANF2Driver.h
USAGE...      Motor driver support for the AMCI ANF2 controller.

Kevin Peterson

Based on the AMCI ANG1 Model 3 device driver written by Kurt Goetze

*/

#include "asynMotorController.h"
#include "asynMotorAxis.h"
//#include <asynInt32Array.h>
#include <asynInt32ArraySyncIO.h>

#define MAX_AXES 12

#define MAX_INPUT_REGS 10
#define MAX_OUTPUT_REGS 10

#define AXIS_REG_OFFSET 10

/*** Input CMD Registers (16-bit) ***/
#define STATUS_1   0
#define STATUS_2   1
#define POS_RD_UPR 2
#define POS_RD_LWR 3
#define EN_POS_UPR 4
#define EN_POS_LWR 5
#define EN_CAP_UPR 6
#define EN_CAP_LWR 7
// Not used must equal zero #define RESERVED 8
#define NET_CONN   9

/*** Output Command Registers (32-bit) ***/
#define COMMAND       0
#define POSITION      1
#define SPEED         2
#define ACCEL_DECEL   3
#define CMD_REG_4     4

/*** Output Configuration Registers (32-bit) ***/
#define CONFIGURATION 0
#define BASE_SPEED    1
#define HOME_TIMEOUT  2
#define CONFIG_REG_3  3
#define CONFIG_REG_4  4

// No. of controller-specific parameters
#define NUM_ANF2_PARAMS 2

/** drvInfo strings for extra parameters that the ACR controller supports */
#define ANF2ResetErrorsString       "ANF2_RESET_ERRORS"
#define ANF2GetInfoString           "ANF2_GET_INFO"
//#define ANF2ReconfigString          "ANF2_RECONFIG"

class ANF2Axis : public asynMotorAxis
{
public:
  /* These are the methods we override from the base class */
  ANF2Axis(class ANF2Controller *pC, int axisNo, epicsInt32 config, epicsInt32 baseSpeed, epicsInt32 homingTimeout);
  void report(FILE *fp, int level);
  asynStatus move(double position, int relative, double min_velocity, double max_velocity, double acceleration);
  asynStatus moveVelocity(double min_velocity, double max_velocity, double acceleration);
  asynStatus home(double min_velocity, double max_velocity, double acceleration, int forwards);
  asynStatus stop(double acceleration);
  asynStatus poll(bool *moving);
  asynStatus setPosition(double position);
  asynStatus setClosedLoop(bool closedLoop);

private:
  ANF2Controller *pC_;          /**< Pointer to the asynMotorController to which this axis belongs.
                                   *   Abbreviated because it is used very frequently */
  asynStatus sendAccelAndVelocity(double accel, double velocity);
  double correctAccel(double minVelocity, double maxVelocity, double acceleration);
  asynStatus resetErrors();
  void getInfo();
  epicsInt32 inputReg_[10];
  //void reconfig(epicsInt32 value);
  void zeroRegisters(epicsInt32 *reg);
  asynUser *pasynUserForceRead_;
  int axisNo_;
  epicsInt32 baseSpeed_;
  epicsInt32 homingTimeout_;
  epicsInt32 config_;
  epicsInt32 motionReg_[5];
  epicsInt32 confReg_[5];
  epicsInt32 zeroReg_[5];
  bool jogging_;
  // Configuration bits
  short CaptInput_;
  short ExtInput_;
  short HomeInput_;
  short CWInput_;
  short CCWInput_;
  short BHPO_;
  short QuadEnc_;
  short DiagFbk_; 
  short OutPulse_;
  short HomeOp_;
  short CardAxis_;
  short OpMode_;
  // LSW
  short CaptInputAS_;
  short ExtInputAS_;
  short HomeInputAS_;
  short CWInputAS_;
  short CCWInputAS_;

friend class ANF2Controller;
};

class ANF2Controller : public asynMotorController {
public:
  ANF2Controller(const char *portName, const char *ANF2InPortName, const char *ANF2OutPortName, int numAxes);
  void doStartPoller(double movingPollPeriod, double idlePollPeriod);
  
  void report(FILE *fp, int level);
  ANF2Axis* getAxis(asynUser *pasynUser);
  ANF2Axis* getAxis(int axisNo); 
  asynUser *pasynUserInReg_[MAX_AXES][MAX_INPUT_REGS];
  asynUser *pasynUserOutReg_[MAX_AXES];

  /* These are the methods that we override from asynMotorDriver */
  asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  //asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
  //void report(FILE *fp, int level);

  /* These are the methods that are new to this class */
  
protected:
  int ANF2ResetErrors_;     /** Reset Errors parameter index */
  int ANF2GetInfo_;          /**< Get Info parameter index */        
  //int ANF2Reconfig_;          /**< Reconfig parameter index */        

private:
  asynStatus writeReg16(int, int, int, double);
  asynStatus writeReg32(int, int, int, double);
  asynStatus writeReg32Array(int, epicsInt32*, int, double);
  asynStatus readReg16(int, int, epicsInt32*, double);
  asynStatus readReg32(int, int, epicsInt32*, double);
  char *inputDriver_;
  int numAxes_;
  int numModules_;
  int axesPerModule_;
  double movingPollPeriod_;
  double idlePollPeriod_;
  int axesCreated_;

friend class ANF2Axis;
};
