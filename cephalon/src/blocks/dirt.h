#ifndef CEPHALON_BLOCKS_DIRT_H_
#define CEPHALON_BLOCKS_DIRT_H_

namespace cephalon {
    class Dirt : public Block {
    public:
        Dirt()
            : Block("dirt")
        {

        }

        void init() {
            texture_ = loadTexture("dirt");
        }

        virtual TextureInfo getRightTexture() const override {
            return texture_;
        }

        virtual TextureInfo getLeftTexture() const override {
            return texture_;
        }

        virtual TextureInfo getTopTexture() const override {
            return texture_;
        }

        virtual TextureInfo getBottomTexture() const override {
            return texture_;
        }

        virtual TextureInfo getBackTexture() const override {
            return texture_;
        }

        virtual TextureInfo getFrontTexture() const override {
            return texture_;
        }
    private:
        TextureInfo texture_;
    };
}

#endif // CEPHALON_BLOCKS_DIRT_H_