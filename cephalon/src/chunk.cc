#include "chunk.h"
using namespace cephalon;

#include <iterator>

#include "utils.h"
#include "world.h"

bgfx::VertexLayout Chunk::layout_;
bgfx::ProgramHandle Chunk::program_;

void Chunk::init() {
    layout_.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 1, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color1, 4, bgfx::AttribType::Uint8, true)
    .end();
    program_ = LoadProgram("vs_chunks", "fs_chunks");
}

void Chunk::cleanup() {
    bgfx::destroy(program_);
}

Chunk::Chunk()
{
    dirty_ = false;
    blocks_.resize(kVolume.x * kVolume.y * kVolume.z, &blocks::kAir);
    vertex_buffer_ = bgfx::createDynamicVertexBuffer(0u, layout_, BGFX_BUFFER_ALLOW_RESIZE);
    index_buffer_ = bgfx::createDynamicIndexBuffer(0u, BGFX_BUFFER_ALLOW_RESIZE);
}

Chunk::~Chunk() noexcept {
    bgfx::destroy(vertex_buffer_);
    bgfx::destroy(index_buffer_);
}

void Chunk::setBlock(glm::ivec3 pos, const Block& block) {
    assert(pos.x >= 0 && pos.x < kVolume.x);
    assert(pos.y >= 0 && pos.y < kVolume.y);
    assert(pos.z >= 0 && pos.z < kVolume.z);
    auto index = pos.z * kVolume.x * kVolume.y + pos.y * kVolume.x + pos.x;
    if (blocks_[index] != &block) {
        dirty_ = true;
        blocks_[index] = &block;
    }
}

const Block& Chunk::getBlock(glm::ivec3 pos) const {
    assert(pos.x >= 0 && pos.x < kVolume.x);
    assert(pos.y >= 0 && pos.y < kVolume.y);
    assert(pos.z >= 0 && pos.z < kVolume.z);
    return *blocks_[pos.z * kVolume.x * kVolume.y + pos.y * kVolume.x + pos.x];
}

void Chunk::rebuild(World& world) {
    std::vector<Vertex> vertices;
    std::vector<std::uint16_t> indices;

    for (int x = 0; x < kVolume.x; ++x) {
        for (int y = 0; y < kVolume.y; ++y) {
            for (int z = 0; z < kVolume.z; ++z) {
                auto& block = getBlock(glm::ivec3(x, y, z));
                if (!block.isAir()) {
                    auto color = block.getColor();

                    // right
                    if (x == kVolume.x - 1 || getBlock(glm::ivec3(x + 1, y, z)).isAir()) {
                        float block_ao[] = {
                            vertexAO(world.getBlock(glm::ivec3(x + 1, y, z - 1)), world.getBlock(glm::ivec3(x + 1, y - 1, z)), world.getBlock(glm::ivec3(x + 1, y - 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x + 1, y, z + 1)), world.getBlock(glm::ivec3(x + 1, y - 1, z)), world.getBlock(glm::ivec3(x + 1, y - 1, z + 1))),
                            vertexAO(world.getBlock(glm::ivec3(x + 1, y, z - 1)), world.getBlock(glm::ivec3(x + 1, y + 1, z)), world.getBlock(glm::ivec3(x + 1, y + 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x + 1, y, z + 1)), world.getBlock(glm::ivec3(x + 1, y + 1, z)), world.getBlock(glm::ivec3(x + 1, y + 1, z + 1))),
                        };

                        Vertex block_vertices[] = {
                            { glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), block_ao[0], color },
                            { glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), block_ao[1], color },
                            { glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), block_ao[2], color },
                            { glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), block_ao[3], color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 1, index_base + 3, index_base + 2,
                            index_base + 1, index_base + 2, index_base + 0
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // left
                    if (x == 0 || getBlock(glm::ivec3(x - 1, y, z)).isAir()) {
                        float block_ao[] = {
                            vertexAO(world.getBlock(glm::ivec3(x - 1, y, z - 1)), world.getBlock(glm::ivec3(x - 1, y - 1, z)), world.getBlock(glm::ivec3(x - 1, y - 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x - 1, y, z + 1)), world.getBlock(glm::ivec3(x - 1, y - 1, z)), world.getBlock(glm::ivec3(x - 1, y - 1, z + 1))),
                            vertexAO(world.getBlock(glm::ivec3(x - 1, y, z - 1)), world.getBlock(glm::ivec3(x - 1, y + 1, z)), world.getBlock(glm::ivec3(x - 1, y + 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x - 1, y, z + 1)), world.getBlock(glm::ivec3(x - 1, y + 1, z)), world.getBlock(glm::ivec3(x - 1, y + 1, z + 1))),
                        };
                        
                        Vertex block_vertices[] = {
                            { glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), block_ao[0], color },
                            { glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), block_ao[1], color },
                            { glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), block_ao[2], color },
                            { glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), block_ao[3], color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 0, index_base + 2, index_base + 3,
                            index_base + 0, index_base + 3, index_base + 1,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // top
                    if (y == kVolume.y - 1 || getBlock(glm::ivec3(x, y + 1, z)).isAir()) {
                        float block_ao[] = {
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z - 1)), world.getBlock(glm::ivec3(x - 1, y + 1, z)), world.getBlock(glm::ivec3(x - 1, y + 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z + 1)), world.getBlock(glm::ivec3(x - 1, y + 1, z)), world.getBlock(glm::ivec3(x - 1, y + 1, z + 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z - 1)), world.getBlock(glm::ivec3(x + 1, y + 1, z)), world.getBlock(glm::ivec3(x + 1, y + 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z + 1)), world.getBlock(glm::ivec3(x + 1, y + 1, z)), world.getBlock(glm::ivec3(x + 1, y + 1, z + 1))),
                        };

                        Vertex block_vertices[] = {
                            { glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), block_ao[0], color },
                            { glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), block_ao[1], color },
                            { glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), block_ao[2], color },
                            { glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), block_ao[3], color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 2, index_base + 3, index_base + 1,
                            index_base + 2, index_base + 1, index_base + 0,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // bottom
                    if (y == 0 || getBlock(glm::ivec3(x, y - 1, z)).isAir()) {
                        float block_ao[] = {
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z - 1)), world.getBlock(glm::ivec3(x - 1, y - 1, z)), world.getBlock(glm::ivec3(x - 1, y - 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z + 1)), world.getBlock(glm::ivec3(x - 1, y - 1, z)), world.getBlock(glm::ivec3(x - 1, y - 1, z + 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z - 1)), world.getBlock(glm::ivec3(x + 1, y - 1, z)), world.getBlock(glm::ivec3(x + 1, y - 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z + 1)), world.getBlock(glm::ivec3(x + 1, y - 1, z)), world.getBlock(glm::ivec3(x + 1, y - 1, z + 1))),
                        };

                        Vertex block_vertices[] = {
                            { glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), block_ao[0], color },
                            { glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), block_ao[1], color },
                            { glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), block_ao[2], color },
                            { glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), block_ao[3], color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 3, index_base + 2, index_base + 0,
                            index_base + 3, index_base + 0, index_base + 1,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // back
                    if (z == kVolume.z - 1 || getBlock(glm::ivec3(x, y, z + 1)).isAir()) {
                        float block_ao[] = {
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z + 1)), world.getBlock(glm::ivec3(x - 1, y, z + 1)), world.getBlock(glm::ivec3(x - 1, y - 1, z + 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z + 1)), world.getBlock(glm::ivec3(x - 1, y, z + 1)), world.getBlock(glm::ivec3(x - 1, y + 1, z + 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z + 1)), world.getBlock(glm::ivec3(x + 1, y, z + 1)), world.getBlock(glm::ivec3(x + 1, y - 1, z + 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z + 1)), world.getBlock(glm::ivec3(x + 1, y, z + 1)), world.getBlock(glm::ivec3(x + 1, y + 1, z + 1))),
                        };

                        Vertex block_vertices[] = {
                            { glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), block_ao[0], color },
                            { glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), block_ao[1], color },
                            { glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), block_ao[2], color },
                            { glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), block_ao[3], color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 1, index_base + 3, index_base + 2,
                            index_base + 0, index_base + 1, index_base + 2,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // front
                    if (z == 0 || getBlock(glm::ivec3(x, y, z - 1)).isAir()) {
                        float block_ao[] = {
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z - 1)), world.getBlock(glm::ivec3(x - 1, y, z - 1)), world.getBlock(glm::ivec3(x - 1, y - 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z - 1)), world.getBlock(glm::ivec3(x - 1, y, z - 1)), world.getBlock(glm::ivec3(x - 1, y + 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y - 1, z - 1)), world.getBlock(glm::ivec3(x + 1, y, z - 1)), world.getBlock(glm::ivec3(x + 1, y - 1, z - 1))),
                            vertexAO(world.getBlock(glm::ivec3(x, y + 1, z - 1)), world.getBlock(glm::ivec3(x + 1, y, z - 1)), world.getBlock(glm::ivec3(x + 1, y + 1, z - 1))),
                        };

                        Vertex block_vertices[] = {
                            { glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), block_ao[0], color },
                            { glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), block_ao[1], color },
                            { glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), block_ao[2], color },
                            { glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), block_ao[3], color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 2, index_base + 3, index_base + 1,
                            index_base + 2, index_base + 1, index_base + 0
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }
                }
            }
        }
    }

    if (!vertices.empty()) {
        bgfx::update(vertex_buffer_, 0, bgfx::copy(vertices.data(), vertices.size() * sizeof(Vertex)));
        bgfx::update(index_buffer_, 0, bgfx::copy(indices.data(), indices.size() * sizeof(std::uint16_t)));
    }
    dirty_ = false;
}

void Chunk::render() {
    bgfx::setVertexBuffer(0, vertex_buffer_);
    bgfx::setIndexBuffer(index_buffer_);
    bgfx::submit(0, program_);
}

float Chunk::vertexAO(const Block& side1, const Block& side2, const Block& corner) {
    if (!side1.isAir() && !side2.isAir())
        return 0.0f;
    return (3 - !side1.isAir() - !side2.isAir() - !corner.isAir()) / 3.0f;
}