#include "Temperature.h"

Temperature::Temperature(float newTemp) : m_temperature(newTemp) {}

float Temperature::getTemperature() {
    return m_temperature;
}