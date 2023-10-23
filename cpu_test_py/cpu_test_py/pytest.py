import rclpy
from rclpy.node import Node
import psutil



class CPU(Node):

    def __init__(self):
        super().__init__('pycputest')
        num=0
        trig=False
        while (True):
            num+=1
            # 모든 프로세스 정보 가져오기
            process_list = [proc.info for proc in psutil.process_iter(attrs=['pid', 'name', 'cpu_percent'])]

            # CPU 사용량을 기준으로 프로세스 정렬
            sorted_process_list = sorted(process_list, key=lambda x: x['cpu_percent'], reverse=True)

            # 상위 몇 개의 프로세스 정보 표시 (예: 상위 10개)
            num_top_processes = 10
            if(num>50):

                for i, process_info in enumerate(sorted_process_list[:num_top_processes]):
                    pid = process_info['pid']
                    cmd = process_info['name']
                    cpu_percent = process_info['cpu_percent']
                    if(cpu_percent>99):
                        print(f"PID: {pid} | 명령어: {cmd} | CPU 사용량(%): {cpu_percent} \n")
                        trig=True
                if(trig==True):
                    print("***************************************************************") 
                    trig=False   
                num=0
            

def main(args=None):
    rclpy.init(args=args)

    cpu = CPU()

    rclpy.spin(cpu)
    cpu.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()