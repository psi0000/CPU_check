#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/register_node_macro.hpp"
#include <future>
#include <memory>
#include <chrono>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <cstdio>
#include <unistd.h>
int numProcesses = 10; // 상위 몇 개의 프로세스를 가져올 것인지 설정하세요.

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
        std::vector<ProcessInfo> GetTopProcesses(int numProcesses) {
            std::string command = "top -b -n 1 -d 0.01 -o %CPU | tail -n +" + std::to_string(8) + " | head -n " + std::to_string(numProcesses);
            std::array<char, 128> buffer;
            std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
            std::string result;
            if (!pipe) {
                std::cerr << "top 명령어 실행 실패" << std::endl;
                return {};
            }
            buffer.empty();
            // 결과 읽기
            while (!feof(pipe.get())) {
                if (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
                    result += buffer.data();
                    
                }
            }
            //std::cout <<result<<std::endl;

            // 결과 파싱
            std::istringstream stream(result);
            std::vector<ProcessInfo> processList;
            std::string line;

            while (std::getline(stream, line)) {
                std::istringstream lineStream(line);
                ProcessInfo processInfo;
                lineStream >> processInfo.pid >>processInfo.name>> processInfo.pr>>processInfo.ni>>processInfo.virt>>processInfo.res>>processInfo.shr>>processInfo.s>>processInfo.cpuUsage>>processInfo.mem>>processInfo.time>> processInfo.command;
                if(processInfo.cpuUsage >0.0){
                    processList.push_back(processInfo);
                    
                }
                
            }

            return processList;
        }
class CpuClient : public rclcpp::Node
{
    public:
        CpuClient()
        : Node("cpu_test")
        {
            pid_t pid_;
            pid_ = getpid();
            std::cout <<pid_ <<std::endl;
            //reset file
            std::ofstream file("cur_time.txt");
            file.close();
            while(rclcpp::ok()){            
                rclcpp::Clock clock(RCL_ROS_TIME);
                rclcpp::Time current_time = clock.now();
                // std::string cur_time=std::to_string(current_time.seconds());
                time_t seconds_since_epoch = static_cast<time_t>(current_time.seconds());
                tm local_time;
                localtime_r(&seconds_since_epoch, &local_time);
    
                std::stringstream time_stream;
                time_stream << local_time.tm_hour << ":" << local_time.tm_min<<":"<<local_time.tm_sec;
                std::string cur_time=time_stream.str();
                
                std::vector<ProcessInfo> topProcesses = GetTopProcesses(numProcesses);
                
                
                //add data in file
                std::ofstream file("cur_time.txt",std::ios::app);        
                if (topProcesses.empty()) {
                    //std::cerr << "프로세스 정보를 가져오는 데 실패했습니다." << std::endl;
                } else {
                    std::cout << "상위 " << numProcesses << " CPU 사용량이 높은 프로세스:" << std::endl;
                    
                    for (const ProcessInfo& process : topProcesses) {
                        std::cout<<"TIME: "<< cur_time<< " | PID: " << process.pid << " | CPU 사용량(%): " << process.cpuUsage << " | time: " << process.time << " | cmd: " << process.command  <<std::endl;
                        
                        if(file.is_open()){
                            file << "TIME: "<<cur_time<<" | PID: " << process.pid << " | CPU 사용량(%): " << process.cpuUsage << " | time: " << process.time << " | cmd: " << process.command<< std::endl;
                            
                        }
                    
                    }
                   
                }
                file.close();
                
            }
             file.close();

            
        }

    private :
};
int main(int argc, char ** argv)
{
        rclcpp::init(argc, argv);
        
            
        auto node=std::make_shared<CpuClient>(); 
        rclcpp::spin(node);
        
        return 0;
}
