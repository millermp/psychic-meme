/**
 * Temperature.h
 * 
 * February 24, 2020
 */
#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

class Temperature
{
    float m_temperature;

public:
    float getTemperature();
    Temperature(float newTemp);
};
#endif