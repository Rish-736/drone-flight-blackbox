\# 🚁 Drone Flight Black Box (ESP32)



An external ESP32-based flight black box that records critical motion events

(free fall and impact) into internal flash memory and recovers them after power loss.



\## Why this project?

Drone crashes are difficult to debug after the fact. This project acts as a

passive observer that survives resets and provides post-crash insights.



\## Features (v1)

\- MPU6050-based free fall detection

\- Impact detection with peak g-force

\- Event persistence using ESP32 flash (NVS)

\- Crash recovery after reset

\- Finite state machine based logic



\## Hardware Used

\- ESP32 Dev Module

\- MPU6050 IMU



\## Current Status

✅ Motion detection working  

✅ Flash logging working  

🚧 Ring buffer and voltage monitoring in progress

