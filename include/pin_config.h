 /*
 * Remote Feelings: Adam Curtis, Aymeric Wang, Xinying Hu
 * 11/30/21
 * Prototype version. Author: Adam Curtis, Aymeric Wang
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#define vibrator -1
#define vibrator2 -1

// Enter a 1 next to the user profile you wish to use and a zero next to the others
#define NICOLE 0
#define NITZAN 1
#define NEWUSER 0
#define ADAM 0
#define JAMES 0
#if ((ADAM + NEWUSER + JAMES + NITZAN) == 1) // Only one user profile can be active.

#if NITZAN == 1 || NEWUSER == 1

#define PinkyEnc 13
#define RingEnc 14
#define MiddleEnc 26
#define IndexEnc 27
#define ThumbEnc 4

#define LOGOLED -1

#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  20000
#define INA 2
#define INB 15

#define FF1 32
#define FF2 35
#define FF3 39
#define FF4 34
#define FF5 36

//Change pin order to change directions the motor spins

#define pinkyMotor1 12
#define pinkyMotor2 33
#define ringMotor1 23
#define ringMotor2 25
#define middleMotor1 21
#define middleMotor2 22
#define indexMotor1 19
#define indexMotor2 18
#define thumbMotor1 17
#define thumbMotor2 16
#endif

#if JAMES == 1
#define PinkyEnc 4
#define RingEnc 26
#define MiddleEnc 27
#define IndexEnc 14
#define ThumbEnc 13

#define LOGOLED 23

#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  20000
#define INA 2
#define INB 15

#define FF1 36
#define FF2 39
#define FF3 34
#define FF4 35
#define FF5 32

#define pinkyMotor1 17
#define pinkyMotor2 16
#define ringMotor1 22
#define ringMotor2 21
#define middleMotor1 19
#define middleMotor2 18
#define indexMotor1 25
#define indexMotor2 23
#define thumbMotor1 33
#define thumbMotor2 12
#endif

#if ADAM == 1
#define PinkyEnc 25
#define RingEnc 26
#define MiddleEnc 27
#define IndexEnc 14
#define ThumbEnc 4

#define LOGOLED 23

#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  20000
#define INA 2
#define INB 15

#define FF1 36
#define FF2 39
#define FF3 34
#define FF4 35
#define FF5 32

#define pinkyMotor1 17
#define pinkyMotor2 16
#define ringMotor1 22
#define ringMotor2 21
#define middleMotor1 19
#define middleMotor2 18
#define indexMotor1 5
#define indexMotor2 23
#define thumbMotor1 15
#define thumbMotor2 12
#endif
 
const int motorPins[] = {pinkyMotor1, pinkyMotor2, ringMotor1, ringMotor2, // continue on next line
middleMotor1, middleMotor2, indexMotor1, indexMotor2, thumbMotor1, thumbMotor2};

const int FFPins[] = {FF1,FF2,FF3,FF4,FF5};
const int SPins[] = {pinkyMotor1,pinkyMotor2,ringMotor1,ringMotor2,middleMotor1};
const int EncPins[] = {PinkyEnc, RingEnc, MiddleEnc, IndexEnc, ThumbEnc};

#endif

#endif


// 