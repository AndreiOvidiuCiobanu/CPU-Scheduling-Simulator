# CPU-Scheduling-Simulator

The processor is one of the Single Processor environment can be carried out it takes allocate resources only from the CPU at most one process at the same time. Therefore, we must face the multiple processes at the same time try to perform CPU operations

# CPU Scheduling Algorithms

# 1 FCFS (First Come First Served)
This approach Preemptive manner if it is one of a process performed by the CPU because, the other processes can not be performed in the CPU until the process is complete, or by assigning the Interrupt CPU has become idle

# 2 SJF (Shortest Job First)
SJF is a small time required to perform the work in a small CPU Burst Time remaining processes, ie the CPU is a way to give priority to the least the rest of the process.

# 3 Priority
Priority algorithm is a method of giving priority to the CPU performs the first program given the highest priority, and the priority given to each process in the process. Like the SJF algorithm Preemptive, bumps may be implemented by Non-Preemptive way.

# 4 Round Robin
Round Robin scheme, which is assigned to the US re-defined, and each time it processes (Time Quantum) to be performed by the CPU for each of the processes at once. The order in which the process is performed basically follows the order in which the request for the CPU to perform, i.e. the same way as the FCFS algorithm. 

# 5 Preemptive & Non-preemptive LIF (Longest I / O First)
The basic structure was the same implementation and SJF algorithm, the only difference is that instead of comparing the CPU burst time and compared with I / O burst time.

# 6 Preemptive & Non-Preemptive LISC (Longest I / O Shortest CPU First)
The advantage of the algorithm of the LIF (5) is clearly, but remain standing between the I / O burst the same time this process can not but the tteoan the disadvantages of the FCFS algorithm as since the Scheduling the FCFS system. Thus, the advantages of LIF algorithm while making disadvantage of FCFS algorithm, but the Scheduling by way of LIF algorithm a lower priority to minimize, among the I / O burst time is the same process M is a CPU burst time is the smallest process It was configured to select.
