#include <memory>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "interfaces/msg/hardware_status.hpp"
#include "interfaces/srv/check_system.hpp" // El include usa snake_case (check_system.hpp)

using std::placeholders::_1;
using std::placeholders::_2;

class MonitorNode : public rclcpp::Node
{
public:
  MonitorNode()
  : Node("monitor"), umbral_temperatura_(50), ultima_temperatura_(0), motores_listos_(false)
  {
    // Obtener el parámetro 'umbral_temperatura'
    this->declare_parameter<int64_t>("umbral_temperatura", 50);
    this->get_parameter("umbral_temperatura", umbral_temperatura_);
    
    RCLCPP_INFO(this->get_logger(), "Nodo Monitor iniciado con umbral_temperatura: %ld", umbral_temperatura_);

    // Suscriptor
    subscription_ = this->create_subscription<interfaces::msg::HardwareStatus>(
      "hardware_status", 10,
      std::bind(&MonitorNode::topic_callback, this, _1));

    // Servidor de Servicio 
    service_ = this->create_service<interfaces::srv::CheckSystem>(
      "check_system",
      std::bind(&MonitorNode::handle_check_system, this, _1, _2));
  }

private:
  void topic_callback(const interfaces::msg::HardwareStatus & msg)
  {
    ultima_temperatura_ = msg.temperatura;
    motores_listos_ = msg.motores_listos;

    RCLCPP_INFO_STREAM(this->get_logger(), 
      "Estado recibido -> Temp: " << msg.temperatura << " | Motores: " << msg.motores_listos << " | Msg: " << msg.debug_msg);

    // Formato con %ld porque 'temperatura' es int64
    if (msg.temperatura >= umbral_temperatura_) {
      RCLCPP_WARN(this->get_logger(), "¡ADVERTENCIA! La temperatura (%ld°C) supera el umbral (%ld°C)", 
                  msg.temperatura, umbral_temperatura_);
    }
  }

  void handle_check_system(
    const std::shared_ptr<interfaces::srv::CheckSystem::Request> request,
    std::shared_ptr<interfaces::srv::CheckSystem::Response> response)
  {
    (void)request;
    
    RCLCPP_INFO(this->get_logger(), "Petición recibida en /check_system");

    // Revisa si los campos de CheckSystem.srv usan 'es_seguro' o 'estado'
    if (ultima_temperatura_ < umbral_temperatura_ && motores_listos_) {
      response->estado = true;
    } else {
      response->estado = false;
    }
  }

  int64_t umbral_temperatura_;
  int64_t ultima_temperatura_;
  bool motores_listos_;

  rclcpp::Subscription<interfaces::msg::HardwareStatus>::SharedPtr subscription_;
  rclcpp::Service<interfaces::srv::CheckSystem>::SharedPtr service_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MonitorNode>());
  rclcpp::shutdown();
  return 0;
}