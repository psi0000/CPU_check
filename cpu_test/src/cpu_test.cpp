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
            std::string command = "top -b -n 1 -o %CPU | tail -n +" + std::to_string(8) + " | head -n " + std::to_string(numProcesses);
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
            // std::cout <<result<<std::endl;

            // 결과 파싱
            std::istringstream stream(result);
            std::vector<ProcessInfo> processList;
            std::string line;

            while (std::getline(stream, line)) {
                std::istringstream lineStream(line);
                ProcessInfo processInfo;
                lineStream >> processInfo.pid >>processInfo.name>> processInfo.pr>>processInfo.ni>>processInfo.virt>>processInfo.res>>processInfo.shr>>processInfo.s>>processInfo.cpuUsage>>processInfo.mem>>processInfo.time>> processInfo.command;
                if(processInfo.cpuUsage >90.0){
                    processList.push_back(processInfo);
                    
                    std::string command1 = "top -b -n1 > cpu_log.txt";
                    std::array<char, 128> buffer1;
                    std::shared_ptr<FILE> pipe1(popen(command1.c_str(), "r"), pclose);
                    std::string result1;

                    if (!pipe1) {
                        std::cerr << "top save fail" << std::endl;
                        return {};
                    }
                    buffer1.empty();
                    // 결과 읽기
                    while (!feof(pipe1.get())) {
                        if (fgets(buffer1.data(), static_cast<int>(buffer1.size()), pipe1.get()) != nullptr) {
                            result1 += buffer1.data();
                            
                        }
                    }

                    std::cout <<result<<std::endl;
                }
                
            }

            return processList;
        }
class CpuClient : public rclcpp::Node
{
    public:
        CpuClient()
        : Node("cpu_tester")
        {
            while(1){
                std::vector<ProcessInfo> topProcesses = GetTopProcesses(numProcesses);

                if (topProcesses.empty()) {
                    std::cerr << "프로세스 정보를 가져오는 데 실패했습니다." << std::endl;
                } else {
                    std::cout << "상위 " << numProcesses << " CPU 사용량이 높은 프로세스:" << std::endl;
                    for (const ProcessInfo& process : topProcesses) {
                        std::cout << "PID: " << process.pid << " | CPU 사용량(%): " << process.cpuUsage << " | cmd: " << process.command << std::endl;
                    }
                }
                std::cout <<"*********************************************************************"<<std::endl;
                sleep(3);
                rclcpp::shutdown();
            }
            

            
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