This is the code that records in txt for cpu utilization to resolve cpu overload in the version of Ubuntu ROS2 Foxy.

I made it in cpp and python versions, and mainly used cpp due to the nature of the code.
The cpp branch has two codes.

cpu_test.cpp:
    inf:
        In the case of cpu_test.cpp, it uses the top command to monitor CPU usage on Ubuntu. It specifically selects the top 10 processes by CPU usage and checks if their CPU usage exceeds a certain threshold (assumed to be 100%, but it's adjustable). When a process exceeds this threshold, its information is saved to a file named cur_time.txt.

    Steps:

        1. Launch the top command and arrange the information about it.
        2. Monitor only the CPU usage part.
        3. Save the information of processes that exceed the CPU usage threshold to a file.
    To run this code, you can use the following command:

        $ ros2 run cpu_test cpu_test


test_node.cpp:
    inf:
        In the case of test_node.cpp, it also utilizes the top command, but the order of operations is slightly different.

    Steps:

        1. Create a list of nodes to check (specific nodes you want to monitor; these may need modification).
        Note: This code assumes that the node name and process (command) are the same.
            If that's not the case, you may need to modify it.
            Alternatively, each node's code can publish its own PID using getpid() since each node receives a different PID every time it's run.
        2. Use the pgrep command to find the PID assigned to each node.
        3. Retrieve information based on the PID and use the top command to obtain information about the CPU usage of those PIDs. 
        4. Save this information to a file.
    
    To run this code, you can use the following command:

        $ros2 run cpu_test test_node

if there are any specific details or code segments you'd like further clarification on, feel free to ask.
