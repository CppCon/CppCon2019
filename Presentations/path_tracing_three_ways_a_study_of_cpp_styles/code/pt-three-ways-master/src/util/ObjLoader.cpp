#include "ObjLoader.h"

#include <algorithm>
#include <fstream>
#include <string>

double impl::asDouble(std::string_view sv) {
  return std::stod(std::string(sv)); // This is dreadful
}

int impl::asInt(std::string_view sv) {
  return std::stoi(std::string(sv)); // This is dreadful
}

size_t impl::asIndex(std::string_view sv, size_t max) {
  auto res = std::stol(std::string(sv));
  return res < 0 ? res + max : res - 1;
}

std::unordered_map<std::string, MaterialSpec>
impl::loadMaterials(std::istream &in) {
  using namespace std::literals;
  if (!in)
    throw std::runtime_error("Bad input stream");
  in.exceptions(std::ios_base::badbit);
  std::unordered_map<std::string, MaterialSpec> result;

  MaterialSpec *curMat{};
  int illum = 2;
  Vec3 ambientColour;

  auto flushMat = [&] {
    if (!curMat)
      return;
    if (illum == 3) {
      curMat->reflectivity = ambientColour.length();
    }
    curMat = nullptr;
  };

  parse(in, [&](std::string_view command,
                const std::vector<std::string_view> &params) {
    if (command == "newmtl"sv) {
      flushMat();
      if (params.size() != 1)
        throw std::runtime_error("Wrong number of params for newmtl");
      curMat =
          &result.emplace(std::string(params[0]), MaterialSpec{}).first->second;
      return true;
    } else if (command == "Ke"sv) {
      if (!curMat)
        throw std::runtime_error("Unexpected Ke");
      if (params.size() != 3)
        throw std::runtime_error("Wrong number of params for Ke");
      curMat->emission =
          Vec3(asDouble(params[0]), asDouble(params[1]), asDouble(params[2]));
      return true;
    } else if (command == "Kd"sv) {
      if (!curMat)
        throw std::runtime_error("Unexpected Kd");
      if (params.size() != 3)
        throw std::runtime_error("Wrong number of params for Kd");
      curMat->diffuse =
          Vec3(asDouble(params[0]), asDouble(params[1]), asDouble(params[2]));
      return true;
    } else if (command == "Ka"sv) {
      if (!curMat)
        throw std::runtime_error("Unexpected Ka");
      if (params.size() != 3)
        throw std::runtime_error("Wrong number of params for Ka");
      ambientColour =
          Vec3(asDouble(params[0]), asDouble(params[1]), asDouble(params[2]));
      return true;
    } else if (command == "Ni"sv) {
      if (!curMat)
        throw std::runtime_error("Unexpected Ni");
      if (params.size() != 1)
        throw std::runtime_error("Wrong number of params for Ni");
      curMat->indexOfRefraction = asDouble(params[0]);
      return true;
    } else if (command == "Ns"sv) {
      if (!curMat)
        throw std::runtime_error("Unexpected Ns");
      if (params.size() != 1)
        throw std::runtime_error("Wrong number of params for Ns");
      // Values seem to be in the range [0, 1000], where higher is a tighter
      // specular highlight. This is an empirical hack.
      auto val = asDouble(params[0]) / 100;
      curMat->reflectionConeAngleRadians = M_PI * std::clamp(1 - val, 0.0, 1.0);
      return true;
    } else if (command == "illum"sv) {
      if (!curMat)
        throw std::runtime_error("Unexpected illum");
      if (params.size() != 1)
        throw std::runtime_error("Wrong number of params for illum");
      illum = asInt(params[0]);
      return true;
    } else if (command == "Ks"sv || command == "d"sv) {
      // Ignored
      return true;
    }
    return false;
  });

  flushMat();

  return result;
}
