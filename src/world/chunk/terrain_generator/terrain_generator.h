#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <array>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include "../../voxel/voxel.h"
#include "../../voxel/types.h"
#include "../../../utils/perlin.h"


enum BiomeType {
    SNOWY_TUNDRA,
    MOUNTAINS,
    PLAINS,
    FOREST,
    DESERT
};

struct BiomeParams {
    int baseHeight;
    int heightVariation;
    float temperatureThreshold;
    float humidityThreshold;
};

class TerrainGenerator {
public:
    TerrainGenerator(int width, int height, int depth, glm::vec2 index);
    void generateTerrain(std::vector<Voxel*>& voxels);

private:
    std::tuple<BiomeType, BiomeType, float> determineBiome(float temperature, float humidity);
    int generateHeight(int x, int z, BiomeType primaryBiome, BiomeType secondaryBiome, float blendFactor) const;
    VoxelType determineBlockType(int y, int maxY, BiomeType biome, int waterLevel);
    float generateCaveDensity(int x, int y, int z);
    void generateOres(int x, int y, int z, unsigned int voxelIndex, VoxelType blockType, std::vector<Voxel*>& voxels);
    void addSurfaceFeatures(int x, int z, int maxY, BiomeType biome, int waterLevel, std::vector<Voxel*>& voxels);
    void generateTree(int x, int z, int y, std::vector<Voxel*>& voxels);
    void generateCactus(int x, int z, int y, std::vector<Voxel*>& voxels);
    void generateIceSpike(int x, int z, int y, std::vector<Voxel*>& voxels);
    unsigned int coordsToIndex(glm::vec3 coords) const;

    int width, height, depth;
    glm::vec2 index_;
    Perlin perlinNoise;

    static const std::array<BiomeParams, 5> biomeParameters;
};

#endif // TERRAIN_GENERATOR_H

