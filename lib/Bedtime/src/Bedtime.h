

#include<Time.h>

class Bedtime
{
public:
  Bedtime();

  double get_average_light_value();

  // this function averages the incoming light over all data points.
  void add_light_value(unsigned new_light_value);

  // returns true when it is time to get up.
  bool time_to_get_up();

  // returns true when it is time to sleep.
  bool time_to_sleep();

  // set the state of sleeping. This triggers awake/sleeping time counting
  void set_sleep_state(bool sleeping);

private:
  int m_last_timestamp;

  // \todo for chicken safety count 'locked in' / 'free wildlife' times
  unsigned m_current_sleep_time = 0;
  unsigned m_current_awake_time = 0;

  // \todo separate daytime/nighttime thresholds for precise opening/closing times
  //       delay closing time 30 minutes after light is off.
  const unsigned long m_max_sleep_time = 1000L * 60 * 60 * 10; // 1000 millisecs * 60 sec * 60 min * 10h
  const unsigned long m_max_awake_time = 1000L * 60 * 60 * 18; // 1000 millisecs * 60 sec * 60 min * 18h


  const unsigned sun_is_up_threshold = 40;      // threshold for light sensor on which we consider the sun to be shining
  double m_average_light_value = 0.0;
  unsigned m_pos = 0;                           // current position in light value array

  const static unsigned m_nr_of_light_values = 300;                   // nr of data points for light sensor measurement
  float m_light_values[m_nr_of_light_values];  // light values stored for every second over 5 minutes
  float m_cumulator_seconds = 0.0;
  unsigned m_nr_values_for_second = 0;

};
