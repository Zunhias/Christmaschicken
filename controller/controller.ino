
//----------------------------------------------------------------------------------------------------------------------
// The amazing Chickenshack controller
//
// general ideas for improvement:
// - we need a 'manual control mode' plus buttons for manually opening/closing the Chickenshack
// - automatic 'fox defence system' (rocket based / laser based)
// - chicken disco feature (twice a year it's party time!!!)
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// pins

const int E1Pin = 2;           // id of the digital switch pin E1 - upper sensor
const int E2Pin = 3;           // id of the digital switch pin E2 - lower sensor
const int motorEnablePin = 7;  // id of the digital enable motor pin
const int directionPin1 = 5;   // id of the digital directionPin1
const int directionPin2 = 6;   // id of the digital directionPin2
const int lightSensorPin = A0;  // id of the analog light sensor pin; for now done with a potentiometer

//----------------------------------------------------------------------------------------------------------------------
// constants

// \todo separate daytime/nighttime thresholds for precise opening/closing times
//       delay closing time 30 minutes after light is off.
const int sun_is_up_threshold  = 40;      // threshold for light sensor on which we consider the sun to be shining
const long max_sleep_time = 1000L * 60 * 60 * 10; // 1000 millisecs * 60 sec * 60 min * 10h
const long max_awake_time = 1000L * 60 * 60 * 18; // 1000 millisecs * 60 sec * 60 min * 18h
const int motor_speed = 1000;          // guillotine style speed!!! ToDo: adjust motor Speed
const int nr_of_light_values = 300;       // nr of data points for light sensor measurement

//----------------------------------------------------------------------------------------------------------------------
// variables

// \todo average some time values so cars/streetlights do not open the chicken shack for intruders
double light_values[nr_of_light_values];  // light values stored for every second over 5 minutes
double average_light_value = 0.0;
bool   motor_is_running = false;
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

void set_motor(int speed, bool reverse = false)
{
  analogWrite (motorEnablePin, speed);
  digitalWrite(directionPin1, !reverse);
  digitalWrite(directionPin2, reverse);
  motor_is_running = ( speed != 0 );
  delay(200); // wait 200 ms so the switches connected to E1/E2 pins are changed in the real world accordingly
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

  //initialize light value array to complete darkness
  for (int i = 0; i < nr_of_light_values; ++i)
  {
    light_values[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------

// this function averages the incoming light over all data points.

double calculate_light_value()
{
  // \todo the interval of measurement should not be 1 milliseconds. Also think about what happens when 'delay' is called
  const unsigned long current_time = millis();
  const int pos = current_time % nr_of_light_values;
  int old_increment = light_values[pos] / nr_of_light_values;
  light_values[pos] = analogRead(lightSensorPin);
  int new_increment = light_values[pos] / nr_of_light_values;
  return average_light_value - old_increment + new_increment;
}

//----------------------------------------------------------------------------------------------------------------------

void loop()
{
  // read the state of the E1/E2 pins for the chicken shack door
  bool door_is_up   = digitalRead(E1Pin) == HIGH;
  bool door_is_down = digitalRead(E2Pin) == HIGH;

  // turn off the motor when any of the E1/E2 switches are closed
  if ( motor_is_running && (door_is_up || door_is_down) )
  {
    set_motor(0);
    motor_is_running = false; // should fix the issue of 08.12.2016 (FZ)
  }
  
  Serial.print("Door is up: ");
  Serial.print(door_is_up);

  Serial.print("; Door is down: ");
  Serial.println(door_is_down);

  Serial.print("Motor is running: ");
  Serial.println(motor_is_running);
  
  // do nothing as long as the motor is running
  if (!motor_is_running)
  {
    average_light_value = calculate_light_value();
    Serial.print("Light value: ");
    Serial.print(average_light_value);
    Serial.println();

    if (door_is_up && time_to_sleep(average_light_value))
    {
      set_motor(motor_speed, false);
    }

    if (door_is_down && time_to_get_up(average_light_value))
    {
      set_motor(motor_speed, true);
    }
  }
 delay(500);
}
