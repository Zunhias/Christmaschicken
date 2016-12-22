
#include<Time.h>

class Bedtime
{
public:
  Bedtime();

  double average_light_value();

  // this function adds a light value to the calculation and updates the wake/sleep system
  void add_light_value(unsigned new_light_value);

  // returns true when it is time to get up.
  bool time_to_get_up();

  // returns true when it is time to sleep.
  bool time_to_sleep();

  // set the state of sleeping. This triggers awake/sleeping time counting
  void set_sleep_state(bool sleeping);

private:

  void update_sleep_wake_state( float light_value );
  void update_average_light_value( float light_value );
  bool m_time_to_get_up = false;
  bool m_time_to_sleep  = false;
  int  m_last_timestamp = 0;

  float m_average_light_value = 0.0;
  unsigned m_pos = 0;                           // current position in light value array

  const static unsigned m_nr_of_light_values = 30;                   // nr of data points for light sensor measurement
  float m_light_values[m_nr_of_light_values];  // light values stored for every second over 5 minutes
  float m_cumulator_seconds = 0.0;
  unsigned m_nr_values_for_second = 0;
  unsigned m_time_since_sun_down = 0;
  unsigned m_time_since_sun_up = 0;


};
