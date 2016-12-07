
//----------------------------------------------------------------------------------------------------------------------
// The amazing Chickenshack controller
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// pins

const int E1Pin = 4;           // id of the digital switch pin E1
const int E2Pin = 3;           // id of the digital switch pin E2
const int motorEnablePin =  0; // id of the digital enable motor pin
const int directionPin1 = 1;   // id of the digital directionPin1
const int directionPin2 = 2;   // id of the digital directionPin2
const int lightSensorPin = 5;  // id of the analog light sensor pin

//----------------------------------------------------------------------------------------------------------------------
// constants

const int sun_is_up_threshold  = 40;      // threshold for light sensor on which we consider the sun to be shining
const int max_sleep_time = 1000*60*60*10; // 1000 millisecs * 60 sec * 60 min * 10h
const int max_awake_time = 1000*60*60*18; // 1000 millisecs * 60 sec * 60 min * 18h
const int motor_speed = 1000000; // guillotine style speed!!!

//----------------------------------------------------------------------------------------------------------------------
// variables

// \todo average some time values so cars/streetlights do not open the chicken shack for intruders
int light_values[300];          //  light values stored for every second over 5 minutes
int smoothed_light_value = 0;   // the current smoothed light sensor value
bool motor_is_running = false;
// \todo for chicken safety count 'locked in' / 'free wildlife' times
int current_sleep_time = 0;
int current_awake_time = 0;

//----------------------------------------------------------------------------------------------------------------------

bool time_to_get_up( int light_value, int current_sleep_time = 0)
{
  return (light_value > sun_is_up_threshold) || (current_sleep_time > max_sleep_time);
}

//----------------------------------------------------------------------------------------------------------------------

bool time_to_sleep(int light_value, int current_awake_time = 0)
{
  return (light_value <= sun_is_up_threshold) || (current_awake_time > max_awake_time);
}

//----------------------------------------------------------------------------------------------------------------------

void set_motor(int speed, bool reverse)
{
  analogWrite (motorEnablePin, speed);
  digitalWrite(directionPin1, !reverse);
  digitalWrite(directionPin2, reverse);
  motor_is_running = ( speed == 0 );
}

//----------------------------------------------------------------------------------------------------------------------

void setup()
{
  // initialize the motor enable pin and the direction pins as outputs:
  pinMode(motorEnablePin, OUTPUT);
  pinMode(directionPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);

  // initialize the switch pin as an input:
  pinMode(E1Pin, INPUT);
  pinMode(E2Pin, INPUT);
}

//----------------------------------------------------------------------------------------------------------------------

void loop()
{
  // read the state of the switch value:
  const bool door_is_up   = digitalRead(E1Pin) == HIGH;
  const bool door_is_down = digitalRead(E2Pin) == HIGH;

  // turn off the motor when any of the up/down switches are triggered
  // \todo probably we have to wait a little bit for the switches to change after activating the motor
  if( motor_is_running && (door_is_up || door_is_down) ) // && (time_since_motor_started > motor_start_time_threshold)
  {
    set_motor(0, false);
  }

  // \todo smoothing of the light values
  const int smoothed_light_value = analogRead(lightSensorPin);

  if (door_is_up && time_to_sleep(smoothed_light_value))
  {
    set_motor(motor_speed, false);
  }

  if (door_is_down && time_to_get_up(smoothed_light_value))
  {
    set_motor(motor_speed, true);
  }
}
