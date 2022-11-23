//

#include "HaruSerializer.h"
#include "Assets/Texture.h"
#include "Ensure.h"
#include "Model/BrushFace.h"
#include "Model/BrushNode.h"
#include "Model/EntityProperties.h"
#include "Model/PatchNode.h"
#include "Model/Polyhedron.h"

#include <fmt/format.h>

namespace TrenchBroom
{
namespace IO
{


HaruSerializer::HaruSerializer(std::ostream& outputStream)
  : m_outputStream(outputStream)
{
  ensure(m_outputStream.good(), "output stream is good");
}


void HaruSerializer::doBeginFile(const std::vector<const Model::Node*>& rootNodes)
{
  //  WRITE("begin file\n");
}


void HaruSerializer::doEndFile()
{
#define write(format, ...)                                                               \
  fmt::format_to(std::ostreambuf_iterator<char>(m_outputStream), format, __VA_ARGS__)

  write("{}\n", m_brushes.size());

  for (const auto& brush : m_brushes)
  {
    brush.serialize(m_outputStream);
  }

#undef write
}


void HaruSerializer::doBeginEntity(const Model::Node* node)
{
  //  WRITE("begin entity {}\n", node->name());
}


void HaruSerializer::doEndEntity(const Model::Node* node)
{
  //  WRITE("end entity {}\n", node->name());
}


void HaruSerializer::doEntityProperty(const Model::EntityProperty& property)
{
  //  WRITE("{}: {}\n", property.key(), property.value());
}


void HaruSerializer::doBrush(const Model::BrushNode* brush)
{
  m_brushes.emplace_back(brush->brush());
}


void HaruSerializer::doBrushFace(const Model::BrushFace& face) {}


void HaruSerializer::doPatch(const Model::PatchNode* patchNode) {}


HaruSerializer::BrushSerializer::BrushSerializer(const Model::Brush& brush)
{
  // vertices for collision
  for (const auto& vertex : brush.vertices())
  {
    vertices.emplace_back(vertex->position());
  }

  // vertices & normals for each face
  for (const auto& brushFace : brush.faces())
  {
    Face& face = faces.emplace_back();
    face.normal = Direction(brushFace.normal());
    for (const auto& vertex : brushFace.vertices())
    {
      face.vertices.emplace_back(vertex->position());
    }
  }
}


void HaruSerializer::BrushSerializer::serialize(std::ostream& outputStream) const
{
#define write(format, ...)                                                               \
  fmt::format_to(std::ostreambuf_iterator<char>(outputStream), format, __VA_ARGS__)

  write("{}\n", vertices.size());
  for (const auto& vertex : vertices)
  {
    write("{} {} {}\n", vertex.x, vertex.y, vertex.z);
  }

  write("{}\n", faces.size());
  {
    for (const auto& face : faces)
    {
      write("{} {} {}\n", face.normal.x, face.normal.y, face.normal.z);
      write("{}\n", face.vertices.size());
      for (const auto& vertex : face.vertices)
      {
        write("{} {} {}\n", vertex.x, vertex.y, vertex.z);
      }
    }
  }

#undef write
}


} // namespace IO
} // namespace TrenchBroom