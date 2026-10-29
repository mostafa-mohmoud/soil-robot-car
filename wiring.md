# Wiring Guide

Complete connections for the Soil Robot Car. A visual diagram is available in `images/wiring.svg`.

Always connect all grounds together: Arduino GND, L298N GND, battery GND, and every sensor GND.

---

## 1. L298N Motor Driver to Arduino

| L298N Pin | Arduino Pin | Purpose |
|---|---|---|
| ENA | D5 (PWM) | Right motor speed |
| IN1 | D7 | Right motor direction |
| IN2 | D8 | Right motor direction |
| IN3 | D9 | Left motor direction |
| IN4 | D10 | Left motor direction |
| ENB | D6 (PWM) | Left motor speed |
| GND | GND | Common ground |

Remove the ENA and ENB jumpers on the L298N board so the PWM pins can control the speed.

## 2. L298N Power and Motors

| L298N Pin | Connect To |
|---|---|
| 12V (VS) | Battery positive (7 to 12 V) |
| GND | Battery negative and Arduino GND |
| 5V | Leave empty if the 12V jumper is in place and the battery is 12 V or less. Do not feed power into it from the Arduino. |
| OUT1, OUT2 | Right motor |
| OUT3, OUT4 | Left motor |

If the car drives backward when it should drive forward, swap the two wires of that motor.

## 3. HC-SR04 Ultrasonic Sensor

| HC-SR04 Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| TRIG | D12 |
| ECHO | D13 |
| GND | GND |

## 4. Servo Motor (SG90)

| Servo Wire | Connect To |
|---|---|
| Signal (orange / yellow) | D11 |
| VCC (red) | 5V |
| GND (brown / black) | GND |

The servo carries the ultrasonic sensor. Mount the HC-SR04 on the servo horn so it rotates with it.

If the servo causes the Arduino to reset, power it from a separate 5V source and connect that source's GND to the Arduino GND.

## 5. Soil Moisture Sensor

| Sensor Pin | Arduino Pin |
|---|---|
| VCC | D4 (powered only while measuring) |
| GND | GND |
| AOUT / SIG | A0 |

## 6. Salinity (EC) Sensor

| Sensor Pin | Arduino Pin |
|---|---|
| VCC | D2 (powered only while measuring) |
| GND | GND |
| AOUT / SIG | A1 |

Powering the sensors from digital pins keeps them off between measurements, which reduces corrosion of the probes. If your sensor draws more than 20 mA, connect its VCC to 5V directly instead, and remove the power pin logic from the code.

---

## Full Pin Summary

| Arduino Pin | Connected To |
|---|---|
| D2 | Salinity sensor VCC |
| D4 | Moisture sensor VCC |
| D5 | L298N ENA |
| D6 | L298N ENB |
| D7 | L298N IN1 |
| D8 | L298N IN2 |
| D9 | L298N IN3 |
| D10 | L298N IN4 |
| D11 | Servo signal |
| D12 | HC-SR04 TRIG |
| D13 | HC-SR04 ECHO |
| A0 | Moisture sensor output |
| A1 | Salinity sensor output |
| 5V | HC-SR04 VCC, servo VCC |
| GND | All grounds |

---

## Simulation Notes (Proteus)

When simulating, replace the two soil sensors with potentiometers (middle pin to A0 or A1, outer pins to 5V and GND). Use the HC-SR04 test pin with a potentiometer to change the measured distance. The Arduino and HC-SR04 libraries must be installed in Proteus first.
