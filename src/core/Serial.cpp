
#include <SPI.h>

#include "Configuration.h"

#include "core/Serial.h"
#include "core/Utils.h"
#include "core/State.h"
#include "core/Cmd.h"
#include "core/Calibration.h"
#include "core/Controler.h"

#include "SAMD51/interrupts.h"

#include "language/en.h"

#include "modules/custommath.h"

void init_menu() {

  cmdAdd(help_command, Serial_menu);
  cmdAdd(calibrate_command, calibration);
  cmdAdd(state_command, state);
  cmdAdd(reset_command, SoftReset);
  cmdAdd(step_response_command, step_response);
  cmdAdd(interrupt_command, set_TC);
  cmdAdd("loop", get_max_frequency);
  cmdAdd("motion", set_motion);
  cmdAdd("gains", set_gains);
  cmdAdd("spline", splineMovement);
  cmdAdd("pos", reportPostition);
  cmdAdd("para", paramter_streamer);

}


void reportPostition(int arg_cnt, char **args) {
  Serial.print("Target = ");
  Serial.print(r);
  Serial.print(", ");
  Serial.print("Postion = ");
  Serial.println(y);
}


void set_gains(int arg_cnt, char **args) {


  Kp = return_float_argument(args, arg_cnt, "-Kp", Kp, 0.0, 100.0);
  Ki = return_float_argument(args, arg_cnt, "-Ki", Ki, 0.0, 1000.0);
  Kd = return_float_argument(args, arg_cnt, "-Kd", Kd, 0.0, 100.0);
  Kf = return_float_argument(args, arg_cnt, "-Kf", Kf, 0.0, 100.0);
  Kv = return_float_argument(args, arg_cnt, "-Kv", Kv, 0.0, 100.0);
  Kac = return_float_argument(args, arg_cnt, "-Kac", Kac, 0.0, 100.0);

  Serial.println(" ");
  Serial.println("Setting new controller gains:");
  Serial.println("-----------------------------");
  Serial.print("Kp = ");
  Serial.println(Kp);
  Serial.print("Ki = ");
  Serial.println(Ki);
  Serial.print("Kd = ");
  Serial.println(Kd);
  Serial.print("Kf = ");
  Serial.println(Kf);
  Serial.print("Kv = ");
  Serial.println(Kv);
  Serial.print("Kac = ");
  Serial.println(Kac);

  myPID.disable();
  myPID.setTunings(Kp, Ki, Kd, Kf, Kv, Kac);
  myPID.enable();
}

void splineMovement(int arg_cnt, char **args) {
  // check for given velocity
  velLimit = return_float_argument(args, arg_cnt, "-v", velLimit, 100, 5000);
  accLimit = return_float_argument(args, arg_cnt, "-a", accLimit, 100, 50000);

  // calculate Target
  steps = ((return_float_argument(args,  arg_cnt, "-r", r , -100000.0, 100000.0) * steps_per_revolution * microstepping) / 360.0  ) + 0.5;
  mystepInterface.writesteps(steps);


}


void set_motion(int arg_cnt, char **args) {
  float velLimit_temp = return_float_argument(args, arg_cnt, "-v", velLimit, 1, 1000000);
  float accLimit_temp = return_float_argument(args, arg_cnt, "-acc", accLimit, 1, 1000000);
  velLimit = velLimit_temp;
  accLimit = accLimit_temp;
}


void state(int arg_cnt, char **args) {
  Serial.println(state_header);

  if (check_argument(args,  arg_cnt, "-off")) {
    Serial.println("disabling");
    myPID.disable();
  }
  if (check_argument(args,  arg_cnt, "-on")) {
    Serial.println("enabling");
    myPID.enable();
  }

  Serial.print(" Motor state = ");
  Serial.println(myPID.getState());


}



void SoftReset(int arg_cnt, char **args) {
  Serial.println(reset_header);
  NVIC_SystemReset();      // processor software reset
}


void set_TC(int arg_cnt, char **args) {
  Serial.println(interrupt_header);

  bool TC5_bool = return_bool_argument(args, arg_cnt, "-TC5", true);

  if (TC5_bool) {
    Serial.println("TC5 enabled!");
    enableTC5Interrupts();
  }
  else {
    Serial.println("TC5 disabled!");
    disableTC5Interrupts();
  }


}


void Serial_menu(int arg_cnt, char **args) {

  Serial.println(help_header);
  Serial.println(help_command " - " help_menu);
  Serial.println(calibrate_command " - " calibrate_menu);
  Serial.println(diagnostics_command " - " diagnostics_menu);
  Serial.println(state_command " - " state_menu);
  Serial.println(stream_command " - " stream_menu);
  Serial.println(parameter_command " - " parameter_menu);
  Serial.println(error_command " - " error_menu);
  Serial.println(dirac_command " - " dirac_menu);
  Serial.println(step_response_command " - " step_response_menu);
  Serial.println(error_command " - " error_menu);
  Serial.println(autotune_command " - " autotune_menu);
  Serial.println(downhill_command " - " downhill_menu);
  Serial.println(test_command " - " test_menu);
  Serial.println(interrupt_command " - " interrupt_menu);

}





void step_response(int arg_cnt, char **args) {
  Serial.println(step_response_header);

  int response_steps = return_float_argument(args, arg_cnt, "-s", 1000, 1, 10000);
  
  // check for given velocity
  velLimit = return_float_argument(args, arg_cnt, "-v", velLimit, 100, 5000);
  accLimit = return_float_argument(args, arg_cnt, "-a", accLimit, 100, 50000);


  int  frequency = 5000;

  Serial.print("Steps = ");
  Serial.println(response_steps);
  Serial.print("Frequency = ");
  Serial.println(frequency);


  bool last_dir = dir;


  float answer[2000];
  float target[2000];
  float effort[2000];


  int counter = 0;

  unsigned int current_time = micros();
  unsigned int next_time = current_time;

  float dt = 1000000.0 / (float)frequency;


  // calculate the target vector
  target[0] = mystepInterface.getsteps();
  for (int i = 1; i < 2000; i++) {
    target[i] = target[i - 1];

    if (i == 200) {
      target[i] = target[i - 1]  + response_steps;
    }
  }

  dir = true;
  //enabled = true;
  myPID.enable();
  mystepInterface.writesteps(target[0]);
  delay(2000);
  while (counter < 2000) {
    current_time = micros();

    if (current_time >= next_time) {
      next_time = current_time + dt;

      answer[counter] = myAS5047D.getAngle();
      effort[counter] = u;
      mystepInterface.writesteps(target[counter]);
      target[counter] = r;
      //steps = target[counter];

      counter += 1;
    }
  }

  /*
    for (int i = 0; i < 2000; i++) {
      target[i] = target[i] * (360.0 / (steps_per_revolution * microstepping));
    }
  */

  Serial.println("time;target;anlge;effort");
  for (int i = 0; i < 2000; i++) {
    Serial.print((float)((float)i * (float)dt / 1000000.0), 5);
    Serial.print(';');
    Serial.print(target[i], 5);
    Serial.print(';');
    Serial.print(answer[i], 5);
    Serial.print(';');
    Serial.println(effort[i], 5);

  }

  /*
    // move back to last position
    while (steps > last_steps) {
      current_time = micros();
      if (current_time >= next_time) {
        next_time = current_time + 150;
        steps -= 1;
      }
    }

  */

  // set parameters back to the values before the
  //enabled = last_enabled;
  myPID.disable();
  dir = last_dir;
}














void get_max_frequency(int arg_cnt, char **args) {
  disableTC5Interrupts();

  uint32_t max_counter = 10000;
  float frequency[10];

  myPID.enable();

  uint32_t starting_test = micros();

  for (uint8_t k = 0; k < 10; k++) {

    uint32_t starting = micros();

    for (uint32_t i = 0; i < max_counter; i++) {
      ControlerLoop();
    }

    frequency[k] = 1000000.0 / ((float)(micros() - starting) / (float)max_counter);


    Serial.print("loop : ");
    Serial.print(k);
    Serial.print(" / 10 frequency [Hz] = ");
    Serial.println(frequency[k]);

  }

  myPID.disable();
  
  float mean = 0.0;
  for (uint8_t k = 0; k < 10; k++) {
    mean += frequency[k];
  }
  mean = mean/10.0;

  
  Serial.print("Mean frequency [Hz] = ");
  Serial.println(mean);
  Serial.print("execution time = ");
  Serial.println(1000000.0/mean);
  

  Serial.print("Overalltime [s] = ");
  Serial.println((float)(micros() - starting_test) / 1000000.0);
  enableTC5Interrupts();
}






void paramter_streamer (int arg_cnt, char **args) {

    Serial.print("steps_per_revolution = ");
    Serial.println(steps_per_revolution);

    Serial.print("microstepping = ");
    Serial.println(microstepping);

    Serial.print("iMAX = ");
    Serial.println(iMAX);

    Serial.print("M_max = ");
    Serial.println(M_max);

    Serial.print("I_rated = ");
    Serial.println(I_rated);

    Serial.print("USE_ENABLE_PIN = ");
    Serial.println(USE_ENABLE_PIN);

    Serial.print("INVERT = ");
    Serial.println(INVERT);


}
