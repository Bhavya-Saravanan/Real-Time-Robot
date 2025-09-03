**Real-Time Robot**

**📌 Project Overview**

This project implements a vision-guided real-time robot using a Raspberry Pi 4B. The robot detects and follows a red laser dot in real time, integrating computer vision, real-time scheduling, and motor control.

**Key highlights:**

Used a Logitech C270 camera with OpenCV for laser dot detection (HSV masking & contour analysis).

Modular C++ services for camera capture, image processing, decision-making, and motor control.

Implemented Rate Monotonic Scheduling for deterministic execution on Core 1, with watchdog & fault tolerance on Core 2.

Achieved 30 ms end-to-end latency from detection to actuation, surpassing the 100 ms design target.

**⚙️ Features**

**Computer Vision (OpenCV)**

HSV masking and contour extraction for red laser dot detection.

**Real-Time Scheduling**

Rate Monotonic Scheduling (RMS) on dedicated core.

Hardware watchdog for fault tolerance.

**Motor Control**

L298N driver controlled via GPIO PWM.

**Performance Optimizations**

Linux kernel tuning (isolcpus, scaling governor).

Profiling using perf and real-time monitoring.

**🖥️ Hardware Used**

Raspberry Pi 4B

Logitech C270 USB Camera

L298N Motor Driver

DC Motors + Robot Chassis

Battery Pack

**🛠️ Tools & Development**

Language: C++

Libraries: OpenCV, V4L2 (non-blocking camera capture)

Kernel: Linux (real-time tuned)

Debugging: perf, JSON-based dynamic behavior tuning

**🚀 Getting Started
Prerequisites**

Install OpenCV on Raspberry Pi

Configure Linux kernel parameters (isolcpus, performance governor)

Connect motor driver (L298N) and camera

Run the Program
git clone
cd Real-Time-Robot
make
./robot_main

**📚 Learnings & Outcomes**

Applied real-time scheduling to embedded Linux applications.

Integrated vision + robotics for autonomous navigation.

Gained experience in kernel tuning, watchdogs, and performance profiling.

Achieved hard real-time responsiveness within 30 ms loop timing.
