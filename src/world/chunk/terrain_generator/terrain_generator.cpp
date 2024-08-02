#include "terrain_generator.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

const std::array<BiomeParams, 5> TerrainGenerator::biomeParameters = {{
    {64, 10, -0.5f, 0.0f},  // SNOWY_TUNDRA
    {64, 32, 0.0f, 0.3f},  // MOUNTAINS
    {64, 8, 0.3f, 0.3f},    // PLAINS
    {64, 15, 0.3f, 0.6f},   // FOREST
    {64, 6, 0.7f, 0.0f}     // DESERT
}};

TerrainGenerator::TerrainGenerator(int width, int height, int depth, glm::vec2 index)
    : width(width), height(height), depth(depth), index_(index), perlinNoise() {
    std::srand(static_cast<unsigned>(std::time(0)));
}

void TerrainGenerator::generateTerrain(std::vector<Voxel*>& voxels) {
    voxels.resize(width * height * depth, nullptr);

    const int waterLevel = 62;
    const float biomeScale = 0.002f;

    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            float temperature = perlinNoise.noise((index_.x * width + x) * biomeScale, (index_.y * depth + z) * biomeScale);
            float humidity = perlinNoise.noise((index_.x * width + x) * biomeScale + 1000, (index_.y * depth + z) * biomeScale + 1000);

            auto [primaryBiome, secondaryBiome, blendFactor] = determineBiome(temperature, humidity);
            int maxY = generateHeight(x, z, primaryBiome, secondaryBiome, blendFactor);

            for (int y = 0; y < height; ++y) {
                glm::vec3 voxelPosition(x, y, z);
                unsigned int voxelIndex = coordsToIndex(voxelPosition);

                if (y < maxY) {
                    VoxelType blockType = determineBlockType(y, maxY, primaryBiome, waterLevel);

                    // Generate caves
                    float caveDensity = generateCaveDensity(x, y, z);
                    if (caveDensity > 0.55f && y < maxY - 5) {
                        if (y < waterLevel) {
                            voxels[voxelIndex] = new Voxel(voxelPosition, WATER);
                        }
                        continue;
                    }

                    voxels[voxelIndex] = new Voxel(voxelPosition, blockType);

                    // Generate ores
                    generateOres(x, y, z, voxelIndex, blockType, voxels);
                } else if (y < waterLevel) {
                    voxels[voxelIndex] = new Voxel(voxelPosition, WATER);
                }
            }

            // Add surface features
            addSurfaceFeatures(x, z, maxY, primaryBiome, waterLevel, voxels);
        }
    }
}

std::tuple<BiomeType, BiomeType, float> TerrainGenerator::determineBiome(float temperature, float humidity) {
    BiomeType primaryBiome = PLAINS;
    BiomeType secondaryBiome = PLAINS;
    float minDistance = std::numeric_limits<float>::max();
    float secondMinDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i < biomeParameters.size(); ++i) {
        float distance = std::hypot(temperature - biomeParameters[i].temperatureThreshold, humidity - biomeParameters[i].humidityThreshold);
        if (distance < minDistance) {
            secondaryBiome = primaryBiome;
            secondMinDistance = minDistance;
            primaryBiome = static_cast<BiomeType>(i);
            minDistance = distance;
        } else if (distance < secondMinDistance) {
            secondaryBiome = static_cast<BiomeType>(i);
            secondMinDistance = distance;
        }
    }

    float blendFactor = minDistance / (minDistance + secondMinDistance);
    return std::make_tuple(primaryBiome, secondaryBiome, blendFactor);
}

int TerrainGenerator::generateHeight(int x, int z, BiomeType primaryBiome, BiomeType secondaryBiome, float blendFactor) const {
    float scale1 = 0.01f;
    float scale2 = 0.05f;
    float globalX = (x + index_.x * width);
    float globalZ = (z + index_.y * depth);

    const BiomeParams& primaryParams = biomeParameters[primaryBiome];
    const BiomeParams& secondaryParams = biomeParameters[secondaryBiome];

    float noise1 = perlinNoise.noise(globalX * scale1, globalZ * scale1);
    float noise2 = perlinNoise.noise(globalX * scale2, globalZ * scale2);

    float primaryHeight = (noise1 * primaryParams.heightVariation + primaryParams.baseHeight) +
                          (std::pow(std::max(0.0f, noise2), 2) * primaryParams.heightVariation);
    float secondaryHeight = (noise1 * secondaryParams.heightVariation + secondaryParams.baseHeight) +
                            (std::pow(std::max(0.0f, noise2), 2) * secondaryParams.heightVariation);

    return static_cast<int>(primaryHeight * (1 - blendFactor) + secondaryHeight * blendFactor);
}

VoxelType TerrainGenerator::determineBlockType(int y, int maxY, BiomeType biome, int waterLevel) {
    if (y <= waterLevel - 3) {
        return DIRT;
    }
    if (y <= waterLevel) {
        return SAND;
    }

    if (y == maxY - 1) {
        switch (biome) {
            case SNOWY_TUNDRA: return SNOW;
            case MOUNTAINS: return (y > 130) ? STONE : (y > 110) ? DIRT : GRASS;
            case PLAINS: return GRASS;
            case FOREST: return GRASS;
            case DESERT: return SAND;
        }
    } else if (y > maxY - 4) {
        switch (biome) {
            case SNOWY_TUNDRA: return DIRT;
            case MOUNTAINS: return (y > 110) ? DIRT : STONE;
            case PLAINS: return DIRT;
            case FOREST: return DIRT;
            case DESERT: return SAND;
        }
    } else if (y > maxY - 6) {
        return (biome == DESERT) ? SANDSTONE : STONE;
    } else {
        return STONE;
    }
    return STONE;  // Default
}

float TerrainGenerator::generateCaveDensity(int x, int y, int z) {
    float caveNoise1 = perlinNoise.noise((x + index_.x * width) * 0.03f, y * 0.03f, (z + index_.y * depth) * 0.03f);
    float caveNoise2 = perlinNoise.noise((x + index_.x * width) * 0.05f, y * 0.05f, (z + index_.y * depth) * 0.05f);
    return (caveNoise1 + 0.5f * caveNoise2) / 1.5f;
}

void TerrainGenerator::generateOres(int x, int y, int z, unsigned int voxelIndex, VoxelType blockType, std::vector<Voxel*>& voxels) {
    if (blockType == STONE) {
        float oreNoise = perlinNoise.noise((x + index_.x * width) * 0.2f, y * 0.2f, (z + index_.y * depth) * 0.2f);
        if (oreNoise > 0.8f) {
            voxels[voxelIndex] = new Voxel({x, y, z}, IRONORE);
        }
    }
}

void TerrainGenerator::addSurfaceFeatures(int x, int z, int maxY, BiomeType biome, int waterLevel, std::vector<Voxel*>& voxels) {
    if (maxY < height - 1 && maxY > waterLevel) {
        // Use random number for feature generation
        float random = static_cast<float>(rand()) / RAND_MAX;

        switch (biome) {
            case SNOWY_TUNDRA:
                if (random < 0.001f) {  
                    generateIceSpike(x, z, maxY, voxels);
                }
                break;
            case MOUNTAINS:
                if (maxY < 100 && random < 0.001f) { 
                    generateTree(x, z, maxY, voxels);
                }
                break;

            case PLAINS:
                break;

            case FOREST:
                if (random < 0.01f) { 
                    generateTree(x, z, maxY, voxels);
                } else if (random < 0.2f) { 
                    voxels[coordsToIndex({x, maxY, z})] = new Voxel({x, maxY, z}, TALLGRASS);
                }
                break;
            case DESERT:
                if (random < 0.003f) {  
                    generateCactus(x, z, maxY, voxels);
                }
                break;
        }
    }
}

void TerrainGenerator::generateTree(int x, int z, int y, std::vector<Voxel*>& voxels) {
    int treeHeight = 4 + rand() % 3;
    for (int i = 0; i < treeHeight; ++i) {
        if (y + i < height) {
            voxels[coordsToIndex({x, y + i, z})] = new Voxel({x, y + i, z}, LOG);
        }
    }
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            for (int dy = -1; dy <= 2; ++dy) {
                if (abs(dx) + abs(dy) + abs(dz) <= 3 && y + treeHeight + dy < height) {
                    int leafX = x + dx;
                    int leafY = y + treeHeight + dy;
                    int leafZ = z + dz;
                    if (leafX >= 0 && leafX < width && leafZ >= 0 && leafZ < depth) {
                        voxels[coordsToIndex({leafX, leafY, leafZ})] = new Voxel({leafX, leafY, leafZ}, LEAVES);
                    }
                }
            }
        }
    }
}

void TerrainGenerator::generateCactus(int x, int z, int y, std::vector<Voxel*>& voxels) {
    int cactusHeight = 2 + rand() % 3;
    for (int i = 0; i < cactusHeight; ++i) {
        if (y + i < height) {
            voxels[coordsToIndex({x, y + i, z})] = new Voxel({x, y + i, z}, CACTUS);
        }
    }
}

void TerrainGenerator::generateIceSpike(int x, int z, int y, std::vector<Voxel*>& voxels) {
    int spikeHeight = 3 + rand() % 5;
    for (int i = 0; i < spikeHeight; ++i) {
        if (y + i < height) {
            voxels[coordsToIndex({x, y + i, z})] = new Voxel({x, y + i, z}, ICE);
        }
    }
}

unsigned int TerrainGenerator::coordsToIndex(glm::vec3 coords) const {
    return static_cast<unsigned int>(
        coords.x + 
        coords.y * width + 
        coords.z * width * height
    );
}
