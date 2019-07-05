# Pendulums_trafo_2019 Driver Firmware 1.0
# authors: robert kovacs (hasso plattner institute) + code and soda team
# email robertkovax@yahoo.com
# 2019



# ESP32 parameter set over wifi

## Setup

Don't forget to set up:

- Wifi SSID (default: pendulum)
- Wifi password (default: cicacicacica)
- IP address (default: 192.168.123.100)
- Gateway: (default: 192.168.123.1)

## API

Get current parameters:
```
http://[ip]/
```

Set parameters:
```
http://[ip]/set/[parameter]/[p1],[p2],[p3],[p4]
# example
http://[ip]/set/pull_t/10,20,30,40
```
Where [parameter] can be:

- `period_t` - period
- `pull_t` - pull time
- `pull_f` - pull force
- `hold_t` - hold time
- `hold_f` - hold force
- `rew_t` - rewind time
- `rew_f` - rewind force
- `start` - 0: stop, 1-5: cycle every `nth` period

Set parameters per pendulum:
```
http://[ip]/set/pendulum/[pendulum_id],[period_t],[pull_t],[pull_f],[hold_t],[hold_f],[rew_t],[rew_f],[start]
# example
http://192.168.137.158/set/pendulum/2,8000,1000,70,2000,30,1000,-40,1
```
! pendulum parameters are updated at the end of the period. Start and stop also applies only after finishing a full cycle

Start pendulum:
```
http://[ip]/start/[pendulum_id],[start]
# example
http://192.168.137.158/start/0,1
```

Stop pendulum:
```
http://[ip]/stop/[pendulum_id]
# example
http://192.168.137.158/stop/0
```

##Hardware

- motor input voltage 12V
- max. current per motor ~15A
- ESP is powered from USB
