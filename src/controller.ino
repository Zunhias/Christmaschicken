
//----------------------------------------------------------------------------------------------------------------------
// The amazing Chickenshack controller
//
// general ideas for improvement:
// - we need a 'manual control mode' plus buttons for manually opening/closing the Chickenshack
// - automatic 'fox defence system' (rocket based / laser based)
// - chicken disco feature (twice a year it's party time!!!)
//----------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
//----------------------------------------------------------------------------------------------------------------------
// pins

const int E1Pin = 13;           // id of the digital switch pin E1 - upper sensor
const int E2Pin = 12;           // id of the digital switch pin E2 - lower sensor
const int manualControlPin = 9;
const int manualDownPin = 10;
const int manualUpPin = 11;

// H-bridge L293D Pins
const int directionPin1 = 5;   // id of the digital directionPin1
const int directionPin2 = 6;   // id of the digital directionPin2
const int motorEnablePin = 7;  // id of the digital enable motor pin

const int lightSensorPin = A0; // id of the analog light sensor pin; for now done with a potentiometer

//----------------------------------------------------------------------------------------------------------------------
// constants
// \todo separate daytime/nighttime thresholds for precise opening/closing times
//       delay closing time 30 minutes after light is off.
const unsigned sun_is_up_threshold = 40;                   // threshold for light sensor on which we consider the sun to be shining
const unsigned long max_sleep_time = 1000L * 60 * 60 * 10; // 1000 millisecs * 60 sec * 60 min * 10h
const unsigned long max_awake_time = 1000L * 60 * 60 * 18; // 1000 millisecs * 60 sec * 60 min * 18h
const unsigned motor_speed = 1000;                         // guillotine style speed!!! ToDo: adjust motor Speed
const unsigned nr_of_light_values = 300;                   // nr of data points for light sensor measurement

//----------------------------------------------------------------------------------------------------------------------
// variables
double average_light_value = 0.0;
unsigned pos = 0;                           // current position in light value array
unsigned light_values[nr_of_light_values];  // light values stored for every second over 5 minutes
bool motor_is_running = false;
bool manual_control = false;
// \todo for chicken safety count 'locked in' / 'free wildlife' times
unsigned current_sleep_time = 0;
unsigned current_awake_time = 0;

//----------------------------------------------------------------------------------------------------------------------

bool time_to_get_up( unsigned light_value, unsigned current_sleep_time = 0)
{
  return (light_value > sun_is_up_threshold) || (current_sleep_time > max_sleep_time);
}

//----------------------------------------------------------------------------------------------------------------------

bool time_to_sleep(unsigned light_value, unsigned current_awake_time = 0)
{
  return (light_value <= sun_is_up_threshold) || (current_awake_time > max_awake_time);
}

//----------------------------------------------------------------------------------------------------------------------

void set_motor(unsigned speed, bool reverse = false)
{
  analogWrite (motorEnablePin, speed);
  digitalWrite(directionPin1, !reverse);
  digitalWrite(directionPin2, reverse);
  motor_is_running = ( speed != 0 );
}

//----------------------------------------------------------------------------------------------------------------------

void setup()
{
  // initialize serial print for analysis
  Serial.begin(9600);

  // initialize the motor enable pin and the direction pins as outputs:
  pinMode(motorEnablePin, OUTPUT);
  pinMode(directionPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);

  // initialize E1/E2 pins for the chicken shack door as inputs:
  pinMode(E1Pin, INPUT);
  pinMode(E2Pin, INPUT);

  // initialize the pins for manual door control
  pinMode(manualUpPin, INPUT);
  pinMode(manualDownPin, INPUT);
  pinMode(manualControlPin, INPUT);

  //initialize light value array to complete darkness
  for (unsigned i = 0; i < nr_of_light_values; ++i)
  {
    light_values[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------

// this function averages the incoming light over all data points.
double calculate_light_value()
{
  pos = (pos+1) % nr_of_light_values;
  double old_increment = (double)light_values[pos] / (double)nr_of_light_values;
  light_values[pos] = analogRead(lightSensorPin);
  double new_increment = (double)light_values[pos] / (double)nr_of_light_values;
  return average_light_value - old_increment + new_increment;
}

//----------------------------------------------------------------------------------------------------------------------

void loop()
{
  // read the state of the manual control pin
  bool new_manual_control = digitalRead(manualControlPin) == HIGH;
  bool manual_up          = digitalRead(manualUpPin) == HIGH;
  bool manual_down        = digitalRead(manualDownPin) == HIGH;
  // read the state of the E1/E2 pins for the chicken shack door
  bool door_is_up   = digitalRead(E1Pin) == HIGH;
  bool door_is_down = digitalRead(E2Pin) == HIGH;

  Serial.print("Door is up: ");
  Serial.print(door_is_up);

  Serial.print("; Door is down: ");
  Serial.println(door_is_down);

  Serial.print("Motor is running: ");
  Serial.println(motor_is_running);

  Serial.print("Light value: ");
  Serial.print(average_light_value);
  Serial.println();

  Serial.print("Manual control: ");
  Serial.println(manual_control);
  Serial.print("Manual up: ");
  Serial.print(manual_up);
  Serial.print(" Manual down: ");
  Serial.println(manual_down);

  if (motor_is_running)
  {
    // turn off the motor when any of the E1/E2 switches are closed or when manual_control has been changed
    if (  door_is_up || door_is_down || (new_manual_control != manual_control))
    {
      set_motor(0);
    }
  }
  manual_control = new_manual_control;

  if (manual_control)
  {
    if (manual_up && !door_is_up)
    {
      set_motor(motor_speed, true);
    }
    if (manual_down && !door_is_down)
    {
      set_motor(motor_speed, false);
    }
  }
  else
  {

    // do nothing as long as the motor is running
    if (!motor_is_running)
    {
      average_light_value = calculate_light_value();

      if (!door_is_down && time_to_sleep(average_light_value))
      {
        set_motor(motor_speed, false);
      }

      if (!door_is_up && time_to_get_up(average_light_value))
      {
        set_motor(motor_speed, true);
      }
    }
  }

 delay(50); // wait a bit so the real world can change accordingly
}
