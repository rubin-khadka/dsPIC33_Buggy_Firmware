# Buggy Robot Firmware

Embedded C firmware for a differential-drive robotic buggy with UART control,
obstacle detection, and real-time sensor logging.

## State Machine

<img width="778" height="585" alt="state_machine" src="https://github.com/user-attachments/assets/c67e7197-ca34-4616-950c-23e309bcd45f" />

## What It Does
- 500 Hz main loop with 10 kHz PWM motor control
- Three FSM states: **Wait for Start**, **Moving**, **Emergency**
- IR obstacle detection with automatic emergency braking
- UART command interface: receive speed/yawrate, switch states
- Sensor telemetry over UART: battery (1 Hz), distance (10 Hz), accelerometer (10 Hz)
- LED heartbeat and emergency blink indicators

## UART Messages
| Type | Message |
|------|---------|
| Battery | `$MBATT,X.YZ*` at 1 Hz |
| Distance | `$MIDST,distance*` at 10 Hz |
| Acceleration | `$MACC,x,y,z*` at 10 Hz |
| Emergency | `$MEMRG,1*` / `$MEMRG,0*` |
| Commands | `$PCRF,speed,yawrate*`, `$PCTP*`, `$PCTT*` |

## Key Technical Details
- Circular buffers for UART TX/RX (no message loss at full bandwidth)
- ISR-driven timing, shared data properly handled
- ADC for battery voltage (via divider), IR sensor, accelerometer
- Output Compare peripherals for PWM generation