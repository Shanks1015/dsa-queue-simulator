
# DSA Assignment 1 – Traffic Queue Simulator

## Overview
This project is a simulation of a traffic junction with four roads (A, B, C, D) and three lanes per road. Vehicles arrive randomly and are managed using **queue data structures**, following specific rules for normal and priority lanes. The simulation demonstrates **basic queue operations, priority handling, and file-based communication**.

---

## Features

- **Random Vehicle Generation**  
  Vehicles are generated randomly for each road and lane using the `traffic_generator.cpp` program.  

- **Queue-based Traffic Management**  
  Each lane uses a custom queue structure to store vehicles waiting to pass the junction.

- **Priority Lane Handling**  
  Lane AL2 is a priority lane. If more than 10 vehicles are waiting, it is served first until the count drops below 5.

- **File-based Communication**  
  The generator writes vehicles to files (`laneA.txt`, `laneB.txt`, etc.), and the simulator reads these files to update queues.

- **Simulator Logic**  
  The simulator processes vehicles in a continuous loop, serving priority and normal lanes in FIFO order.

---

## Project Structure
dsa-queue-simulator/
├── queue.h
├── queue.cpp
├── simulator.cpp
├── traffic_generator.cpp
├── .gitignore
└── README.md

- **queue.h / queue.cpp** : Implements a simple queue for storing vehicles.  
- **simulator.cpp** : Reads vehicle data from files, handles queues, and simulates traffic light logic.  
- **traffic_generator.cpp** : Generates random vehicles and writes them to lane files.  
- **.gitignore** : Excludes generated lane files and executables from Git.  

---





 