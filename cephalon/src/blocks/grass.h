#ifndef CEPHALON_GRASS_H_
#define CEPHALON_GRASS_H_

namespace cephalon {
    class Grass : public Block {
    public:
        virtual glm::vec2 getTexCoord() const override {
            return glm::vec2(0.25f, 0.0f);
        }
    };
}

#endif // CEPHALON_DIRT_H_