#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <future>
#include <memory>
#include <chrono>
#include <vector>
#include <array>
#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include <sstream>
#include <map>

struct ProcessInfo {
            int pid;
            double pr;
            double ni;
            double virt;
            double res;
            double shr;
            std::string name;
            std::string s;
            double cpuUsage;
            double mem;
            std::string time;
            std::string command;
};

int getPID(const std::string &node_name) {
        
                // 특정 노드가 발견됨  
                std::string command = "pgrep -n " + node_name;   //-n :select most recently started ,-x : match exactly with the command name , -f : full process name to match
                FILE* pipe = popen(command.c_str(), "r");
                
                if (!pipe) {
                    return -1; // 명령 실행 실패
                }
                

                int pid=0;
                if (fscanf(pipe, "%d", &pid) == 1) {
                    pclose(pipe);
                    return pid; // 특정 노드의 PID 반환
                }
                pclose(pipe);
            
        
        
        return -1; // 특정 노드를 찾지 못함
    }


std::vector<ProcessInfo> getcpu(const int &node_pid){
    
    std::string command ="top -b -n 1 -d 0.01 -p"+ std::to_string(node_pid);
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
         // 명령 실행 실패
         std::cerr << "top 명령어 실행 실패" << std::endl;
        return {};
    }
    while (!feof(pipe.get())) {
                if (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
                    result += buffer.data();
                    
                }
    }
    //std::cout<< result <<std::endl;
    // 결과 파싱
            std::istringstream stream(result);
            std::vector<ProcessInfo> processList;
            std::string line;

            while (std::getline(stream, line)) {
                std::istringstream lineStream(line);
                ProcessInfo processInfo;
                lineStream >> processInfo.pid >>processInfo.name>> processInfo.pr>>processInfo.ni>>processInfo.virt>>processInfo.res>>processInfo.shr>>processInfo.s>>processInfo.cpuUsage>>processInfo.mem>>processInfo.time>> processInfo.command;
                if(processInfo.cpuUsage >100.0){
                    processList.push_back(processInfo);
                    
                }
                
            }

            return processList;

}

class getCPU : public rclcpp::Node
{
    public:
    getCPU()    
    : Node("node_cpu")
    {       

            //reset file
                std::ofstream file("mid_node_cpu.txt");
                file.close();
            //progress -> find pid   
                std::vector<int> node_pids = {};
                std::vector<std::string> node_names = {"cpu_test","rqt"}; // 원하는 노드 이름으로 변경
            
                /* 
                std::vector<std::string> node_names = 
                 {"r1.a", "r2.c", "r2.s", "r3.c", "r3.s", "r4.a", "r5.a", 
                "avoidance_manager", "avoidance_handler", "line_up_manager","line_up_handler", "recovery_manager",
                "recovery_handler", "robot_manager", "agent_handler","priority_manager", "fleet_manager",
                "one_way_filter_saver", "one_way_filter_server", "one_way_detector"};
                */



                for (const std::string &node_name : node_names) {
                        // 각 노드의 PID를 가져오기
                        int pid = getPID(node_name);
                        if (pid > 0) {
                            node_pids.push_back(pid);
                            RCLCPP_INFO(rclcpp::get_logger(node_name), "Node %s has PID: %d", node_name.c_str(), pid);
                        } else {
                            RCLCPP_ERROR(rclcpp::get_logger(node_name), "Failed to find PID for node %s", node_name.c_str());
                        }
                }

                std::map<int,std::string> node_inf;

                for (size_t i = 0; i < node_pids.size(); ++i) {
                    node_inf[node_pids[i]] = node_names[i];
                }

                if(node_pids.size()==0){
                    RCLCPP_ERROR(rclcpp::get_logger("shutdown"), " dont have Node for monitoring, So End now");
                    rclcpp::shutdown();
                    return ;
                }

            //pid ->find cpu 
                std::cout<<" Ready for monitoring PID"<<std::endl;
                while(1){

                    //time
                        rclcpp::Clock clock(RCL_ROS_TIME);
                        rclcpp::Time current_time = clock.now();
                        // std::string cur_time=std::to_string(current_time.seconds());
                        time_t seconds_since_epoch = static_cast<time_t>(current_time.seconds());
                        tm local_time;
                        localtime_r(&seconds_since_epoch, &local_time);
            
                        std::stringstream time_stream;
                        time_stream << local_time.tm_hour << ":" << local_time.tm_min<<":"<<local_time.tm_sec;
                        std::string cur_time=time_stream.str();

                    //log saver
                        std::ofstream file("mid_node_cpu.txt",std::ios::app);  
                        for(const int &node_pid : node_pids){
                            std::vector<ProcessInfo> logtrig = getcpu(node_pid);
                            std::string pid_name = node_inf[node_pid];
                            if (logtrig.empty()) {
                            //std::cerr << "프로세스 정보를 가져오는 데 실패했습니다." << std::endl;
                            } else{
                                for (const ProcessInfo& process : logtrig) {
                                    std::cout<<"NODE: "<< pid_name<<" | TIME: "<< cur_time<< " | PID: " << process.pid << " | CPU 사용량(%): " << process.cpuUsage << " | time: " << process.time << " | cmd: " << process.command  <<std::endl;
                                    
                                    if(file.is_open()){
                                        file << "NODE: "<< pid_name<<" | TIME: "<<cur_time<<" | PID: " << process.pid << " | CPU 사용량(%): " << process.cpuUsage << " | time: " << process.time << " | cmd: " << process.command<< std::endl;
                                        
                                    }
                            
                                }
                            }
                        }
                        
                        file.close();
                    rclcpp::shutdown();
                }
            
        
    }


    
};



int main(int argc, char ** argv) {

    rclcpp::init(argc, argv);
    auto node=std::make_shared<getCPU>(); 
    rclcpp::spin(node);
    
    return 0;
}
