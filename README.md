Operating System Resource Management and Process Scheduling Simulator

Features:
1. CPU Scheduling:
   - Allows simulation of various CPU scheduling algorithms.
   - Options for tracing execution or viewing statistics.
   - Algorithms supported include First Come First Serve (FCFS), Shortest Job Next (SJN), and others.

2. Banker's Algorithm:
   - Simulates the Banker's Algorithm for deadlock avoidance.
   - Includes matrix management, safety checks, and resource allocation/release.

3. Single Instance Deadlock Detection:
   - Handles deadlock detection using Wait-For Graphs (WFG).
   - Features graphical representation using Graphviz.

Usage:
1. Compile and run the program in a C++ environment.
2. Follow the main menu to navigate through features:
   - Select CPU Scheduling for scheduling simulations.
   - Select Banker's Algorithm for deadlock avoidance checks.
   - Select Single Instance Deadlock Detection for WFG-based deadlock detection.

Requirements:
- Graphviz installed for graphical outputs.
- C++ compiler supporting standard libraries like iostream, vector, and Windows-specific headers.

Note:
This simulator is designed for educational purposes to demonstrate operating system concepts like resource allocation, scheduling, and deadlock detection.
