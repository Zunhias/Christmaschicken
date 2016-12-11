#include "Bedtime.h"
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


double Bedtime::get_average_light_value()
{
 return m_average_light_value;
}

//----------------------------------------------------------------------------------------------------------------------

// this function averages the incoming light over m_nr_of_light_values seconds.
void Bedtime::add_light_value(unsigned new_light_value)
{
  if (second() != m_last_timestamp)
  {
    float light_value_for_second = m_cumulator_seconds/m_nr_values_for_second;
    m_cumulator_seconds = 0;
    m_nr_values_for_second = 0;
    m_last_timestamp = second();

    m_pos = (m_pos+1) % m_nr_of_light_values;
    float old_increment = m_light_values[m_pos] / m_nr_of_light_values;
    m_light_values[m_pos] = light_value_for_second;
    float new_increment = m_light_values[m_pos] / m_nr_of_light_values;
    m_average_light_value = m_average_light_value - old_increment + new_increment;
  }

  m_cumulator_seconds += new_light_value;
  m_nr_values_for_second++;
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
