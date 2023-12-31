cmake_minimum_required(VERSION 3.8)
project(cpu_test)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
add_executable(cpu_test src/cpu_test.cpp)
add_executable(test_node src/test_node.cpp)

ament_target_dependencies(cpu_test 
    rclcpp
    std_msgs
)

ament_target_dependencies(test_node
    
    rclcpp
    std_msgs
)
install(TARGETS cpu_test test_node
    DESTINATION lib/${PROJECT_NAME}
)

ament_export_dependencies(
    
    rclcpp
    std_msgs
)



if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
