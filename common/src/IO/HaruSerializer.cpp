//

#include "HaruSerializer.h"
#include "Assets/Texture.h"
#include "Ensure.h"
#include "Model/BrushFace.h"
#include "Model/BrushNode.h"
#include "Model/EntityProperties.h"
#include "Model/PatchNode.h"
#include "Model/Polyhedron.h"

namespace TrenchBroom
{
namespace IO
{

static void write(std::ostream& stream, uint8_t data)
{
  const char* pData = reinterpret_cast<const char*>(&data);
  stream.write(pData, sizeof(uint8_t));
}

static void write(std::ostream& stream, size_t originalData)
{
  ensure(originalData <= UINT16_MAX, "size_t too large for serialization (max 65535)");
  uint16_t data = originalData;
  const char* pData = reinterpret_cast<const char*>(&data);
  stream.write(pData, sizeof(uint16_t));
}

static void write(std::ostream& stream, float data)
{
  const char* pData = reinterpret_cast<const char*>(&data);
  stream.write(pData, sizeof(float));
}

static void write(std::ostream& stream, const std::string& data)
{
  ensure(data.size() < 256, "string too long for serialization");
  write(stream, static_cast<uint8_t>(data.size()));
  stream.write(data.data(), data.size());
}


HaruSerializer::HaruSerializer(std::ostream& outputStream)
  : m_outputStream(outputStream)
{
  ensure(m_outputStream.good(), "output stream is bad");
}

void HaruSerializer::doBeginFile(const std::vector<const Model::Node*>& rootNodes) {}


void HaruSerializer::doEndFile()
{
  write(m_outputStream, m_numEntities);
  m_outputStream << m_outputStringStream.rdbuf();
}


void HaruSerializer::doBeginEntity(const Model::Node* node) {}


void HaruSerializer::doEndEntity(const Model::Node* node)
{
  write(m_outputStringStream, m_currentProperties.size());
  for (const auto& [key, value] : m_currentProperties)
  {
    write(m_outputStringStream, key);
    write(m_outputStringStream, value);
  }
  m_currentProperties.clear();

  write(m_outputStringStream, m_currentBrushes.size());
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
  write(outputStream, vertices.size());
  for (const auto& vertex : vertices)
  {
    write(outputStream, vertex.x);
    write(outputStream, vertex.y);
    write(outputStream, vertex.z);
  }

  write(outputStream, faces.size());
  {
    for (const auto& face : faces)
    {
      write(outputStream, face.texture);
      write(outputStream, face.normal.x);
      write(outputStream, face.normal.y);
      write(outputStream, face.normal.z);
      write(outputStream, face.vertices.size());
      for (const auto& vertex : face.vertices)
      {
        const auto& position = vertex.position;
        const auto& textureCoord = vertex.textureCoord;
        write(outputStream, position.x);
        write(outputStream, position.y);
        write(outputStream, position.z);
        write(outputStream, textureCoord.u);
        write(outputStream, textureCoord.v);
      }
    }
  }
}


} // namespace IO
} // namespace TrenchBroom