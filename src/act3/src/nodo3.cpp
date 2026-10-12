#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "interfaces/srv/check_system.hpp"

using namespace std::chrono_literals;

class OperadorNode : public rclcpp::Node
{
public:
  OperadorNode()
  : Node("operador")
  {
    // Crear el cliente de servicio para 'check_system'
    client_ = this->create_client<interfaces::srv::CheckSystem>("check_system");

    // Timer para consultar el servicio periódicamente
    timer_ = this->create_wall_timer(
      2s, std::bind(&OperadorNode::consultar_monitor, this));

    RCLCPP_INFO(this->get_logger(), "Nodo Operador iniciado.");
  }

private:
  void consultar_monitor()
  {
    // Esperar a que el servicio esté disponible
    if (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrumpido mientras se esperaba el servicio. Saliendo.");
        return;
      }
      RCLCPP_INFO(this->get_logger(), "Servicio /check_system no disponible, esperando...");
      return;
    }

    auto request = std::make_shared<interfaces::srv::CheckSystem::Request>();

    // Enviar petición de forma asíncrona
    auto result_future = client_->async_send_request(
      request,
      std::bind(&OperadorNode::respuesta_callback, this, std::placeholders::_1));
  }

  void respuesta_callback(rclcpp::Client<interfaces::srv::CheckSystem>::SharedFuture future)
  {
    auto response = future.get();

    if (response->estado) {
      RCLCPP_INFO(this->get_logger(), "Respuesta: Autorización de movimiento concedida");
    } else {
      RCLCPP_WARN(this->get_logger(), "Respuesta: Bloqueo por temperatura alta o motores no listos");
    }
  }

  rclcpp::Client<interfaces::srv::CheckSystem>::SharedPtr client_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<OperadorNode>());
  rclcpp::shutdown();
  return 0;
}