#pragma once

#include <memory>

#include "model.hpp"

namespace lve {

struct Transform2dComponent {
  glm::vec2 translation;
  glm::vec2 scale{ 1.f, 1.f };
  float rotation;

  glm::mat2 mat2() {
    const float s = glm::sin( rotation );
    const float c = glm::cos( rotation );
    glm::mat2 scaleMat{ { scale.x, 0.f }, { 0.f, scale.y } };
    glm::mat2 rotMat{ { c, s }, { -s, c } };
    return rotMat * scaleMat;
  }
};

class GameObject {
 private:
  using id_t = unsigned int;
  id_t id;

  GameObject( id_t _id ) : id{ _id } {}

 public:
  GameObject( const GameObject& ) = delete;
  GameObject& operator=( const GameObject& ) = delete;
  GameObject( GameObject&& ) = default;
  GameObject& operator=( GameObject&& ) = default;

  std::shared_ptr< Model > model{};
  glm::vec3 color{};
  Transform2dComponent transform2d;

  static GameObject createGameObject() {
    static id_t currentId = 0;
    return GameObject( currentId++ );
  }

  id_t getId() { return id; }
};

}  // namespace lve
