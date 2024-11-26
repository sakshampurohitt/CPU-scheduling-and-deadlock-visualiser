 <h1>Operating System Resource Management and Process Scheduling Simulator</h1>

    <h2>Overview</h2>
    <p>This program is a simulator designed to demonstrate concepts in operating systems, including:</p>
    <ul>
        <li>CPU Scheduling Algorithms</li>
        <li>Banker's Algorithm for Deadlock Avoidance</li>
        <li>Deadlock Detection in Single Instance Resources</li>
    </ul>

    <h2>Features</h2>
    <h3>1. CPU Scheduling</h3>
    <p>Simulates various CPU scheduling algorithms, such as:</p>
    <ul>
        <li>First-Come, First-Served (FCFS)</li>
        <li>Shortest Job Next (SJN)</li>
        <li>Round Robin (RR)</li>
        <li>Priority Scheduling</li>
    </ul>
    <p>The program allows tracing timeline execution or gathering scheduling statistics.</p>

    <h3>2. Banker's Algorithm</h3>
    <p>Implements the Banker's Algorithm to avoid deadlocks by ensuring the system remains in a safe state.</p>
    <p>Steps include:</p>
    <ol>
        <li>Input maximum resource requirements for each process</li>
        <li>Allocate resources</li>
        <li>Run the safety algorithm to determine if the system is in a safe state</li>
    </ol>

    <h3>3. Deadlock Detection</h3>
    <p>Detects deadlocks in systems with single-instance resources by:</p>
    <ul>
        <li>Inputting allocated and requested resources</li>
        <li>Generating a Resource Allocation Graph (RAG)</li>
        <li>Generating a Wait-For Graph (WFG)</li>
        <li>Detecting cycles in the WFG to identify deadlocks</li>
    </ul>

    <h2>How to Run</h2>
    <p>Compile and run the program using a C++ compiler. Follow the on-screen instructions to select the desired feature:</p>
    <ol>
        <li>CPU Scheduling</li>
        <li>Banker's Algorithm</li>
        <li>Single Instance Deadlock Detection</li>
        <li>Exit</li>
    </ol>

    <h2>Input Requirements</h2>
    <ul>
        <li>Number of processes and resources</li>
        <li>Maximum resource needs for each process</li>
        <li>Current allocation and available resources</li>
        <li>Requests for resources (for Deadlock Detection)</li>
    </ul>

    <h2>Output</h2>
    <ul>
        <li>Safety state for Banker's Algorithm</li>
        <li>Deadlock status for single-instance resource systems</li>
        <li>Resource allocation and wait-for graphs (saved as images)</li>
    </ul>

    <h2>Dependencies</h2>
    <ul>
        <li>Graphviz for generating and rendering graphs</li>
        <li>A C++ compiler with support for the Standard Template Library (STL)</li>
    </ul>

    <h2>Credits</h2>
    <p>Created as a simulation tool for understanding operating system concepts in resource management and scheduling.</p>

    <h2>License</h2>
    <p>This project is licensed under the MIT License.</p>
