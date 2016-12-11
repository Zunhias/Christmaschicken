#include "Bedtime.hpp"

//----------------------------------------------------------------------------------------------------------------------

Bedtime::Bedtime()
{
  //initialize light value array to complete darkness
  for (unsigned i = 0; i < m_nr_of_light_values; ++i)
  {
    m_light_values[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------


double Bedtime::get_average_light_value()
{
 return m_average_light_value;
}

//----------------------------------------------------------------------------------------------------------------------

// this function averages the incoming light over all data points.
void Bedtime::calculate_light_value(unsigned new_light_value)
{
  m_pos = (m_pos+1) % m_nr_of_light_values;
  double old_increment = (double)m_light_values[m_pos] / (double)m_nr_of_light_values;
  m_light_values[m_pos] = new_light_value;
  double new_increment = (double)m_light_values[m_pos] / (double)m_nr_of_light_values;
  m_average_light_value = m_average_light_value - old_increment + new_increment;
}

//----------------------------------------------------------------------------------------------------------------------

bool Bedtime::time_to_get_up()
{
  return (m_average_light_value > sun_is_up_threshold) || (m_current_sleep_time > m_max_sleep_time);
}

//----------------------------------------------------------------------------------------------------------------------

bool Bedtime::time_to_sleep()
{
  return (m_average_light_value <= sun_is_up_threshold) || (m_current_sleep_time > m_max_awake_time);
}
