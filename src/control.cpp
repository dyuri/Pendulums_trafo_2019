#include <motor.cpp>

class Control
{
private:
    Motor *motors[4];
    int recieved_drv_period_t[4], recieved_drv_pull_t[4], recieved_drv_pull_f[4], recieved_drv_hold_t[4], recieved_drv_hold_f[4], recieved_drv_rew_t[4], recieved_drv_rew_f[4], recieved_drv_start[4];
    int current_drv_period_t[4], current_drv_pull_t[4], current_drv_pull_f[4], current_drv_hold_t[4], current_drv_hold_f[4], current_drv_rew_t[4], current_drv_rew_f[4], current_drv_start[4];
    int state_per_motor[4] = {5, 5, 5, 5}, last_period_start_t[4] = {0, 0, 0, 0}, values_dirty[4], first_values_recieved = 0;

public:
    Control()
    {
        Motor::motorSetup(motors);
    }

    void receivedMessage(int drv_period_t[], int drv_pull_t[], int drv_pull_f[], int drv_hold_t[], int drv_hold_f[], int drv_rew_t[], int drv_rew_f[], int drv_start[])
    {
        memcpy(recieved_drv_period_t, drv_period_t, sizeof(drv_period_t[0]) * 4);
        memcpy(recieved_drv_pull_t, drv_pull_t, sizeof(drv_pull_t[0]) * 4);
        memcpy(recieved_drv_pull_f, drv_pull_f, sizeof(drv_pull_f[0]) * 4);
        memcpy(recieved_drv_hold_t, drv_hold_t, sizeof(drv_hold_t[0]) * 4);
        memcpy(recieved_drv_hold_f, drv_hold_f, sizeof(drv_hold_f[0]) * 4);
        memcpy(recieved_drv_rew_t, drv_rew_t, sizeof(drv_rew_t[0]) * 4);
        memcpy(recieved_drv_rew_f, drv_rew_f, sizeof(drv_rew_f[0]) * 4);
        memcpy(recieved_drv_start, drv_start, sizeof(drv_start[0]) * 4);
        memset(values_dirty, 1, sizeof(values_dirty[0] * 4));
        first_values_recieved = 1;
    }

    void update_values(int motor_id)
    {
        current_drv_period_t[motor_id] = recieved_drv_period_t[motor_id];
        current_drv_pull_t[motor_id] = recieved_drv_pull_t[motor_id];
        current_drv_pull_f[motor_id] = recieved_drv_pull_f[motor_id];
        current_drv_hold_t[motor_id] = recieved_drv_hold_t[motor_id];
        current_drv_hold_f[motor_id] = recieved_drv_hold_f[motor_id];
        current_drv_rew_t[motor_id] = recieved_drv_rew_t[motor_id];
        current_drv_rew_f[motor_id] = recieved_drv_rew_f[motor_id];
        current_drv_start[motor_id] = recieved_drv_start[motor_id];
        //values_dirty[motor_id] = 0;
    }

    void tick()
    {
        if (first_values_recieved == 1)
        {
            //Serial.println("Entered Tick");
            unsigned long current_time = millis();
            for (int motor_id = 0; motor_id < 4; motor_id++)
            {
                switch (state_per_motor[motor_id])
                {
                case 5:
                    //PERIOD INIT
                    last_period_start_t[motor_id] = current_time;
                    update_values(motor_id); //update the calues only once at the beginning of the period
                    if (current_drv_period_t[motor_id] != 0)
                    {
                        state_per_motor[motor_id] = 0;
                        Serial.print("pendulum ");
                        Serial.print(motor_id);
                        Serial.println(" state 0 - PULL");
                    }

                    break;
                case 0:
                    // STATE PULL
                    motors[motor_id]->driveMotor(current_drv_start[motor_id], current_drv_pull_f[motor_id]);
                    if ((current_time - last_period_start_t[motor_id]) > current_drv_pull_t[motor_id])
                    {
                        state_per_motor[motor_id] = 1;
                        Serial.print("pendulum ");
                        Serial.print(motor_id);
                        Serial.println(" state 1 - HOLD");
                    }
                    break;
                case 1:
                    // STATE HOLD
                    motors[motor_id]->driveMotor(current_drv_start[motor_id], current_drv_hold_f[motor_id]);
                    if (((current_time - last_period_start_t[motor_id]) > (current_drv_pull_t[motor_id] + current_drv_hold_t[motor_id])))
                    {
                        state_per_motor[motor_id] = 2;
                        Serial.print("pendulum ");
                        Serial.print(motor_id);
                        Serial.println(" state 2 - REWIND");
                    }
                    break;
                case 2:
                    // STATE REW
                    motors[motor_id]->driveMotor(current_drv_start[motor_id], current_drv_rew_f[motor_id]);
                    if ((current_time - last_period_start_t[motor_id]) > (current_drv_pull_t[motor_id] + current_drv_hold_t[motor_id] + current_drv_rew_t[motor_id]))
                    {
                        state_per_motor[motor_id] = 3;
                        Serial.print("pendulum ");
                        Serial.print(motor_id);
                        Serial.println(" state 3 - WAIT");
                    }
                    break;
                case 3:
                    // STATE WAIT
                    motors[motor_id]->driveMotor(0, 0);
                    if ((current_time - last_period_start_t[motor_id]) >= current_drv_period_t[motor_id])
                    {
                        state_per_motor[motor_id] = 4;
                        Serial.print("pendulum ");
                        Serial.print(motor_id);
                        Serial.println(" state 4 - RESTART");
                    }
                    break;
                case 4:
                    // STATE RESTART PERIOD
                    state_per_motor[motor_id] = 5;
                    motors[motor_id]->driveMotor(0, 0);
                    break;
                }
            }
        }
    }
};