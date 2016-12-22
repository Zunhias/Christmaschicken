#include "Bedtime.h"


namespace
{
  const unsigned sun_is_up_threshold = 200;      // threshold for light sensor on which we consider the sun to be shining
  const unsigned sun_is_down_threshold = 150;    // threshold for light sensor on which we consider the sun to be gone

  const unsigned long max_sleep_time = 60; // change from seconds to this value after testing -> 10 * 3600UL; // 10h * 60 min * 60sec
  const unsigned long max_awake_time = 60; // change from seconds to this value after testing -> 18 * 3600UL; // 18h * 60 min * 60sec
  const unsigned int sun_down_delay_time = 10; //\todo change from 10sec to 30 min when testing is done
}

//----------------------------------------------------------------------------------------------------------------------

Bedtime::Bedtime() : m_last_timestamp(second())
{
  //initialize light value array to complete darkness
  for (unsigned i = 0; i < m_nr_of_light_values; ++i)
  {
    m_light_values[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------

bool Bedtime::time_to_get_up()
{
  return m_time_to_get_up;
}

//----------------------------------------------------------------------------------------------------------------------

bool Bedtime::time_to_sleep()
{
  return m_time_to_sleep;
}

//----------------------------------------------------------------------------------------------------------------------

double Bedtime::average_light_value()
{
  return m_average_light_value;
}

//----------------------------------------------------------------------------------------------------------------------

void Bedtime::update_average_light_value( float light_value )
{
  m_pos = (m_pos+1) % m_nr_of_light_values;
  float old_increment   = m_light_values[m_pos] / m_nr_of_light_values;
  m_light_values[m_pos] = light_value;
  float new_increment   = m_light_values[m_pos] / m_nr_of_light_values;
  m_average_light_value = m_average_light_value - old_increment + new_increment;
}

//----------------------------------------------------------------------------------------------------------------------

void Bedtime::update_sleep_wake_state( float light_value )
{
  bool sun_is_down = ( light_value <= sun_is_down_threshold );
  bool sun_is_up   = ( light_value >  sun_is_up_threshold );
  int time_difference =  second() - m_last_timestamp;
  m_time_since_sun_down = sun_is_down ? (m_time_since_sun_down + time_difference) : 0;
  m_time_since_sun_up   = sun_is_up   ? (m_time_since_sun_up + time_difference) : 0;

  m_time_to_get_up = sun_is_up;
  m_time_to_sleep = (sun_is_down && m_time_since_sun_down > sun_down_delay_time);
  //m_time_to_get_up = sun_is_up || (m_time_since_sun_down > max_sleep_time);
  //m_time_to_sleep = ( sun_is_down && m_time_since_sun_down > sun_down_delay_time )
  //               || ( m_time_since_sun_up > max_awake_time );
}


//----------------------------------------------------------------------------------------------------------------------

void Bedtime::add_light_value(unsigned new_light_value)
{
  if (second() != m_last_timestamp)
  {
    update_average_light_value( m_cumulator_seconds/m_nr_values_for_second );
    update_sleep_wake_state( m_average_light_value );
    m_cumulator_seconds = 0;
    m_nr_values_for_second = 0;
    m_last_timestamp = second();
  }
  m_cumulator_seconds += new_light_value;
  m_nr_values_for_second++;
}
