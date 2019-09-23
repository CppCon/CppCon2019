#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <ctre.hpp>

namespace impl {

double asDouble(std::string_view sv);
int asInt(std::string_view sv);
size_t asIndex(std::string_view sv, size_t max);

// Visible for tests
[[nodiscard]] std::unordered_map<std::string, MaterialSpec>
loadMaterials(std::istream &in);

static constexpr auto tokenRe =
    ctll::fixed_string{R"(\s*((#.*)|[^ \t\n\r#]+))"};

template <typename F>
void parse(std::istream &in, F &&handler) {
  std::string lineAsString;
  int lineNumber = 0;
  while (std::getline(in, lineAsString)) {
    lineNumber++;
    std::string_view line(lineAsString);

    std::vector<std::string_view> fields;
    for (auto match : ctre::range<tokenRe>(line)) {
      if (!match)
        break;
      auto view = match.get<1>().to_view();
      if (view[0] != '#')
        fields.emplace_back(view);
    }

    if (fields.empty())
      continue;

    auto command = fields.front();
    fields.erase(fields.begin());
    if (!handler(command, fields)) {
      throw std::runtime_error("Unknown directive '" + std::string(command)
                               + "' on line " + std::to_string(lineNumber));
    }
  }
}

}

// Thanks to https://en.wikipedia.org/wiki/Wavefront_.obj_file
template <typename SceneBuilder>
void loadObjFile(std::istream &in, ObjLoaderOpener &opener, SceneBuilder &sb) {
  using namespace std::literals;
  using namespace impl;
  if (!in)
    throw std::runtime_error("Bad input stream");
  in.exceptions(std::ios_base::badbit);

  std::vector<Vec3> vertices;
  std::unordered_map<std::string, MaterialSpec> materials;
  MaterialSpec curMat;

  parse(in, [&](std::string_view command,
                const std::vector<std::string_view> &params) {
    if (command == "v"sv) {
      if (params.size() != 3)
        throw std::runtime_error("Wrong number of params for v");
      vertices.emplace_back(asDouble(params[0]), asDouble(params[1]),
                            asDouble(params[2]));
      return true;
    } else if (command == "f"sv) {
      // Decimate the face as a fan.
      std::vector<size_t> indices;
      indices.reserve(params.size());
      for (auto f : params)
        indices.emplace_back(asIndex(f, vertices.size()));
      for (size_t index = 1; index < params.size() - 1; ++index) {
        sb.addTriangle(vertices.at(indices[0]), vertices.at(indices[index]),
                       vertices.at(indices[index + 1]), curMat);
      }
      return true;
    } else if (command == "g"sv || command == "o"sv || command == "s"sv) {
      // Ignore groups, object names and smooth shading
      return true;
    } else if (command == "usemtl"sv) {
      auto matName = std::string(params.at(0));
      auto findIt = materials.find(matName);
      if (findIt == materials.end())
        throw std::runtime_error("Can't find material " + matName);
      curMat = findIt->second;
      return true;
    } else if (command == "mtllib"sv) {
      auto matFile = opener.open(std::string(params.at(0)));
      materials = loadMaterials(*matFile);
      return true;
    }
    return false;
  });
}
