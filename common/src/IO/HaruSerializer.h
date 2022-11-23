
#pragma once

#include "IO/NodeSerializer.h"

#include <FloatType.h>
#include <fmt/format.h>
#include <vecmath/vec.h>

namespace TrenchBroom
{
namespace Assets
{
class Texture;
}

namespace Model
{
class Brush;
class BrushNode;
class BrushFace;
class EntityProperty;
class Node;
} // namespace Model

namespace IO
{

class HaruSerializer : public NodeSerializer
{
public:
  explicit HaruSerializer(std::ostream& outputStream);

private:
  struct Vec3
  {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;

    explicit Vec3(const vm::vec3& v)
      : x(v.x())
      , y(v.z())
      , z(-v.y())
    {
    }
  };

  struct Position : Vec3
  {
    Position() = default;

    explicit Position(const vm::vec3& v)
      : Vec3(v)
    {
      x /= 32.0f;
      y /= 32.0f;
      z /= 32.0f;
    }
  };

  struct Direction : Vec3
  {
    Direction() = default;

    explicit Direction(const vm::vec3& v)
      : Vec3(v)
    {
    }
  };

  struct Face
  {
    Direction normal;
    std::vector<Position> vertices;
  };

  struct BrushSerializer
  {
    std::vector<Position> vertices;
    std::vector<Face> faces;

    explicit BrushSerializer(const Model::Brush& brush);

    void serialize(std::ostream& outputStream) const;
  };

  void doBeginFile(const std::vector<const Model::Node*>& rootNodes) override;

  void doEndFile() override;

  void doBeginEntity(const Model::Node* node) override;

  void doEndEntity(const Model::Node* node) override;

  void doEntityProperty(const Model::EntityProperty& property) override;

  void doBrush(const Model::BrushNode* brush) override;

  void doBrushFace(const Model::BrushFace& face) override;

  void doPatch(const Model::PatchNode* patchNode) override;

  std::ostream& m_outputStream;

  std::vector<BrushSerializer> m_brushes;
};
} // namespace IO
} // namespace TrenchBroom