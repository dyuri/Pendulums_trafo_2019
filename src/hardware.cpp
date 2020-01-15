#pragma once
#include <Arduino.h>

class Motor
{
private:
  int mot_EN_PIN;
  int mot_Dir_PIN;
  int mot_PWM_PIN;
  int mot_PWM_channel;

public:
  Motor(int en_pin, int dir_pin, int pwm_pin, int pwm_channel) : mot_EN_PIN(en_pin), mot_Dir_PIN(dir_pin), mot_PWM_PIN(pwm_pin), mot_PWM_channel(pwm_channel){};

  static void motorSetup(int motorCount, Motor *motors[], const int mot_EN_PIN[], const  int mot_Dir_PIN[], const int mot_PWM_PIN[], const int mot_PWM_channel[])
  {
    for (int i = 0; i < motorCount; i++)
    {
      pinMode(mot_EN_PIN[i], OUTPUT);
      pinMode(mot_Dir_PIN[i], OUTPUT);
      pinMode(mot_PWM_PIN[i], OUTPUT);
      ledcSetup(mot_PWM_channel[i], 32000, 8);
      ledcAttachPin(mot_PWM_PIN[i], mot_PWM_channel[i]);
      motors[i] = new Motor(mot_EN_PIN[i], mot_Dir_PIN[i], mot_PWM_PIN[i], mot_PWM_channel[i]);
    }
  };

  void driveMotor(int en, int force)
  {
    if (en != 0)
    {
      digitalWrite(this->mot_EN_PIN, HIGH);
    }
    else
    {
      digitalWrite(this->mot_EN_PIN, LOW);
    }

    digitalWrite(this->mot_Dir_PIN, HIGH ? force > 0 : LOW);
    ledcWrite(this->mot_PWM_channel, abs(force));
  };
};

class LED
{
private:
  int led_PIN;
  int led_PWM_channel;

public:
  LED(int p_led_pin, int p_led_pwm_channel)
  {
    led_PIN = p_led_pin;
    led_PWM_channel = p_led_pwm_channel;

    //define signal LED
    pinMode(this->led_PIN, OUTPUT);
    ledcSetup(this->led_PWM_channel, 20000, 10);
    ledcAttachPin(this->led_PIN, this->led_PWM_channel);
  };
  
  void driveLED(int intensity)
  {
    ledcWrite(led_PWM_channel, intensity);
  };

};
