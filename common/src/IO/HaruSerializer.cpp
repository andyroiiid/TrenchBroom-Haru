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

#define write(format, ...)                                                               \
  fmt::format_to(std::ostreambuf_iterator<char>(m_outputStream), format, __VA_ARGS__)

void HaruSerializer::doBeginFile(const std::vector<const Model::Node*>& rootNodes) {}


void HaruSerializer::doEndFile()
{
  write("{}\n", m_numEntities);
  m_outputStream << m_outputStringStream.rdbuf();
}

#undef write

#define write(format, ...)                                                               \
  fmt::format_to(                                                                        \
    std::ostreambuf_iterator<char>(m_outputStringStream), format, __VA_ARGS__)

void HaruSerializer::doBeginEntity(const Model::Node* node) {}


void HaruSerializer::doEndEntity(const Model::Node* node)
{
  write("{}\n", m_currentProperties.size());
  for (const auto& [key, value] : m_currentProperties)
  {
    write("{}\n", key);
    write("{}\n", value);
  }
  m_currentProperties.clear();

  write("{}\n", m_currentBrushes.size());
  for (const auto& brush : m_currentBrushes)
  {
    brush.serialize(m_outputStringStream);
  }
  m_currentBrushes.clear();

  m_numEntities++;
}


void HaruSerializer::doEntityProperty(const Model::EntityProperty& property)
{
  m_currentProperties[property.key()] = property.value();
}


void HaruSerializer::doBrush(const Model::BrushNode* brush)
{
  m_currentBrushes.emplace_back(brush->brush());
}

#undef write

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
    face.texture = brushFace.texture()->name();
    face.normal = Direction(brushFace.normal());
    for (const auto& vertex : brushFace.vertices())
    {
      face.vertices.emplace_back(
        vertex->position(), brushFace.textureCoords(vertex->position()));
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
    write("{} {} {} ", vertex.x, vertex.y, vertex.z);
  }
  write("\n");

  write("{}\n", faces.size());
  {
    for (const auto& face : faces)
    {
      write("{} ", face.texture);
      write("{} {} {} ", face.normal.x, face.normal.y, face.normal.z);
      write("{} ", face.vertices.size());
      for (const auto& vertex : face.vertices)
      {
        const auto& position = vertex.position;
        const auto& textureCoord = vertex.textureCoord;
        write(
          "{} {} {} {} {} ",
          position.x,
          position.y,
          position.z,
          textureCoord.u,
          textureCoord.v);
      }
      write("\n");
    }
  }

#undef write
}


} // namespace IO
} // namespace TrenchBroom