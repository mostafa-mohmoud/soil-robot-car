# Soil Robot Car

An Arduino-based robot car that avoids obstacles using an ultrasonic sensor mounted on a servo, and measures soil moisture and salinity at regular intervals.

---

## Features

- Autonomous obstacle avoidance (ultrasonic sensor + servo scanning left and right)
- Soil moisture measurement (percentage and status: dry, medium, wet)
- Soil salinity measurement (percentage and status: low, medium, high)
- Sensors are powered only during measurement to reduce probe corrosion
- Results are printed over the Serial Monitor at 9600 baud

---

## Components

| Component | Quantity |
|---|---|
| Arduino Uno / Nano | 1 |
| L298N motor driver | 1 |
| DC gear motors with wheels | 2 |
| HC-SR04 ultrasonic sensor | 1 |
| SG90 servo motor | 1 |
| Soil moisture sensor (capacitive type recommended) | 1 |
| Soil salinity (EC) sensor | 1 |
| Battery pack (7 to 12 V) for the motors | 1 |
| Chassis, jumper wires, caster wheel | - |

---

## Wiring

| Part | Arduino Pin |
|---|---|
| L298N ENA / IN1 / IN2 | 5 / 7 / 8 |
| L298N ENB / IN3 / IN4 | 6 / 9 / 10 |
| HC-SR04 TRIG / ECHO | 12 / 13 |
| Servo signal | 11 |
| Moisture sensor signal / power | A0 / D4 |
| Salinity sensor signal / power | A1 / D2 |

Use a common GND between the Arduino and the motor battery.

The full connection details are in [docs/wiring.md](docs/wiring.md).

![Wiring diagram](images/wiring.svg)

---

## Getting Started

1. Clone the repository:

```bash
git clone https://github.com/mostafa-mohmoud/soil-robot-car.git
```

2. Open `soil_robot_car/soil_robot_car.ino` in the Arduino IDE.
3. Select your board and port, then upload the sketch.
4. Open the Serial Monitor at 9600 baud to see the soil readings.

---

## Calibration

Edit these constants in the code to match your sensors:

```cpp
const int MOISTURE_AIR_VALUE   = 520;  // raw reading in dry air
const int MOISTURE_WATER_VALUE = 260;  // raw reading in water
const int SALINITY_MIN_RAW     = 0;    // fresh water
const int SALINITY_MAX_RAW     = 700;  // saturated salt solution
```

Other useful settings are `MOTOR_SPEED`, `OBSTACLE_DIST_CM`, `TURN_TIME_MS` and `MEASURE_INTERVAL`.

Without calibration the percentages are only approximate.

---

## Simulation

The circuit can be simulated in Proteus. Proteus has no built-in models for the soil sensors, so potentiometers are used to simulate their analog output. See the simulation notes at the end of [docs/wiring.md](docs/wiring.md).

---

## Project Structure

```
soil-robot-car/
|-- soil_robot_car/
|   `-- soil_robot_car.ino
|-- docs/
|   `-- wiring.md
|-- images/
|   `-- wiring.svg
|-- README.md
|-- LICENSE
`-- .gitignore
```

---

## How It Works

1. The robot drives forward while checking the distance ahead.
2. If an obstacle is closer than `OBSTACLE_DIST_CM`, it stops and scans left and right with the servo.
3. It reverses briefly, then turns toward the side with more free space.
4. Every `MEASURE_INTERVAL` milliseconds it stops, reads the soil sensors and prints the results.

Example output:

```
------ Soil Measurement ------
Moisture: 45%  (medium)
Salinity: 20%  (low)
------------------------------
```

---

## Future Improvements

- Bluetooth or Wi-Fi data transmission to a phone
- LCD or OLED display
- A servo arm to push the probe into the soil
- GPS logging of measurement locations
- Data logging to an SD card

---

## License

This project is licensed under the MIT License. See the LICENSE file for details.

---

## ملخص بالعربي

روبوت عربية مبني على Arduino بيتفادى العقبات بسنسور ألتراسونيك متركب على سيرفو، وكل فترة بيقف ويقيس رطوبة التربة ونسبة الملوحة ويعرض النتائج على Serial Monitor. تفاصيل التوصيلات كاملة في ملف docs/wiring.md.
