#ifndef CEPHALON_BLOCKS_SAND_H_
#define CEPHALON_BLOCKS_SAND_H_

namespace cephalon {
    class Sand : public Block {
    public:
        Sand()
            : Block("sand")
        {
        
        }

        void init() {
            info_ = loadTexture("sand");
        }

        virtual TextureInfo getRightTexture() const override {
            return info_;
        }

        virtual TextureInfo getLeftTexture() const override {
            return info_;
        }

        virtual TextureInfo getTopTexture() const override {
            return info_;
        }

        virtual TextureInfo getBottomTexture() const override {
            return info_;
        }

        virtual TextureInfo getBackTexture() const override {
            return info_;
        }

        virtual TextureInfo getFrontTexture() const override {
            return info_;
        }
    private:
        TextureInfo info_;
    };
}

#endif // CEPHALON_BLOCKS_SAND_H_