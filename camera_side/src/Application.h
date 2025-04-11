#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once



class Output;
class I2SSampler;
class Transport;
class OutputBuffer;
class IndicatorLed;

class Application
{
private:
  Output *m_output;
  I2SSampler *m_input;
  Transport *m_transport;
  // IndicatorLed *m_indicator_led;
  OutputBuffer *m_output_buffer;

public:
  Application();
  void begin();
  void loop();
};

// class Application2
// {
// private:
//   Output *m_output;
//   I2SSampler *m_input;
//   Transport *m_transport;
//   // IndicatorLed *m_indicator_led;
//   OutputBuffer *m_output_buffer;

// public:
//   Application2();
//   void begin();
//   void loop();
// };
#endif