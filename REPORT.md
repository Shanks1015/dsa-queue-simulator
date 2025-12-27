# Technical Report: Traffic Junction Simulation
**Student:** Aayush Acharya  
**Roll No:** 01  
**Subject:** Data Structures & Algorithms (COMP202)

---

## 1. Executive Summary
This project implements a real-time traffic management system for a 4-way junction using a **Circular Queue**. The system is built with C++ and SDL3 for visualization.

## 2. Data Structure Implementation
The core of this project is the **Circular Queue**. Unlike a standard queue, a circular queue connects the last position back to the first, forming a circle.



### Why Circular Queue?
- **Memory Efficiency:** It reuses empty spaces left by dequeued elements.
- **Time Complexity:** Both Enqueue and Dequeue operations are **$O(1)$**.

| Component | Implementation |
| :--- | :--- |
| **Storage** | Fixed-size Circular Array |
| **Logic** | Head and Tail pointer movement |
| **Priority** | Conditional Lane Override (AL2 > 10) |

## 3. Priority Handling (Lane AL2)
The system monitors the vehicle count in lane **AL2**. 
- If count > 10: The system stops the normal rotation and clears AL2.
- Normal rotation resumes only when AL2 count < 5.

## 4. Conclusion
The implementation successfully demonstrates how linear data structures can be applied to flow-control problems. The use of $O(1)$ operations ensures the simulation runs smoothly even with high vehicle generation rates.