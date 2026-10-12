#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "interfaces/msg/hardware_status.hpp"                                            // CHANGE

using namespace std::chrono_literals;

class MinimalPublisher : public rclcpp::Node
{
public:
  MinimalPublisher()
  : Node("emulador_hardware"), count_(0)
  {
    publisher_ = this->create_publisher<interfaces::msg::HardwareStatus>("hardware_status", 10);  // CHANGE
    timer_ = this->create_wall_timer(
      1000ms, std::bind(&MinimalPublisher::timer_callback, this));
  }

private:
  void timer_callback()
  {
    auto message = interfaces::msg::HardwareStatus();                                   // CHANGE
    message.temperatura = this->count_++;      
    if(message.temperatura>10){
        message.motores_listos = 1;
        
    }                  
    else message.motores_listos=0;  
    if(message.temperatura>=10 && message.temperatura<75 ){
        message.debug_msg = "Operacion normal";

    }   
    else if (message.temperatura < 10){
         message.debug_msg = "Calentando";
    }   
    else  message.debug_msg ="Falla de sensor"  ;                   
    // CHANGE
    RCLCPP_INFO_STREAM(this->get_logger(), "Publishing: Temperatura:'" << message.temperatura << "'Motores:'"<< message.motores_listos <<"'Estado: '"<< message.debug_msg <<"'");    // CHANGE
    publisher_->publish(message);
  }
  
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<interfaces::msg::HardwareStatus>::SharedPtr publisher_;             // CHANGE
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}