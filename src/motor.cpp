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

  static void motorSetup(Motor *motors[])
  {
    const int mot_EN_PIN[4] = {33, 32, 26, 17};
    const int mot_Dir_PIN[4] = {22, 19, 16, 5};
    const int mot_PWM_PIN[4] = {21, 18, 4, 27};
    const int mot_PWM_channel[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; i++)
    {
      pinMode(mot_EN_PIN[i], OUTPUT);
      pinMode(mot_Dir_PIN[i], OUTPUT);
      pinMode(mot_PWM_PIN[i], OUTPUT);
      ledcSetup(mot_PWM_channel[i], 5000, 8);
      ledcAttachPin(mot_PWM_PIN[i], mot_PWM_channel[i]);
      motors[i] = new Motor(mot_EN_PIN[i], mot_Dir_PIN[i],mot_PWM_PIN[i], mot_PWM_channel[i]);
    }

  };

  void driveMotor(int en, int force)
  {
    if (en == 1)
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
