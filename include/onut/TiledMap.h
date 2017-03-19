#ifndef TILEDMAP_H_INCLUDED
#define TILEDMAP_H_INCLUDED

// Onut
#include <onut/Point.h>
#include <onut/Maths.h>
#include <onut/Resource.h>
#include <onut/SampleMode.h>

// STL
#include <unordered_map>

// Forward
#include <onut/ForwardDeclaration.h>
OForwardDeclare(ContentManager);
OForwardDeclare(IndexBuffer);
OForwardDeclare(Texture);
OForwardDeclare(TiledMap);
OForwardDeclare(VertexBuffer);

namespace onut
{
    class TiledMap final : public Resource
    {
    public:
        struct Layer
        {
            virtual ~Layer();
            std::string name;
            bool isVisible = true;
        };

        struct TileLayer : public Layer
        {
            virtual ~TileLayer();
            int width;
            int height;
            float opacity;
            uint32_t *tileIds = nullptr;
        };

        struct Object
        {
            Vector2 position;
            Vector2 size;
            uint32_t id;
            std::string name;
            std::string type;
            std::unordered_map<std::string, std::string> properties;
        };

        struct TileSet
        {
            int firstId;
            int tileWidth;
            int tileHeight;
            OTextureRef pTexture;
            std::string name;
        };

        struct ObjectLayer : public Layer
        {
            virtual ~ObjectLayer();
            uint32_t objectCount = 0;
            Object *pObjects = nullptr;
        };

        static OTiledMapRef create(int width, int height, int tileSize);
        static OTiledMapRef createFromFile(const std::string &filename, const OContentManagerRef& pContentManager = nullptr);

        ~TiledMap();

        const Matrix &getTransform() const { return m_transform; }
        void setTransform(const Matrix &transform) { m_transform = transform; }

        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
        int getTileSize() const { return m_tileSize; }

        void render();
        void renderLayer(int index);
        void renderLayer(const std::string &name);
        void renderLayer(Layer *pLayer);
        void render(const iRect &rect);
        void renderLayer(const iRect &rect, int index);
        void renderLayer(const iRect &rect, const std::string &name);
        void renderLayer(const iRect &rect, Layer *pLayer);

        TileSet* getTileSet(int index) const;
        TileSet* getTileSet(const std::string& name) const;
        TileSet* addTileSet(const OTextureRef& pTexture, const std::string& name);

        int getLayerCount() const { return m_layerCount; }
        Layer* getLayer(int index) const { return m_layers[index]; }
        Layer* getLayer(const std::string &name) const;
        int getLayerIndex(const std::string &name) const;
        bool* generateCollisions(const std::string &collisionLayerName);
        bool* getCollisionTiles() const { return m_pCollisionTiles; }
        Layer* addLayer(const std::string &name);

        const OTextureRef& getMinimap();

        void setFiltering(onut::sample::Filtering filtering);

        uint32_t getTileAt(TileLayer *pLayer, int x, int y) const;
        void setTileAt(TileLayer *pLayer, int x, int y, uint32_t tileId);

    private:
        struct Tile
        {
            TileSet *pTileset = nullptr;
            Rect rect;
            Vector4 UVs;
        };

        struct Chunk
        {
            OVertexBufferRef pVertexBuffer;
            OIndexBufferRef pIndexBuffer;
            TileSet *pTileset = nullptr;
            int tileCount = 0;
            bool isDirty = true;
            bool isSizeDirty = true;
            int x, y;
        };

        struct TileLayerInternal : public TileLayer
        {
            virtual ~TileLayerInternal();
            Tile *tiles = nullptr;
            Chunk *chunks = nullptr;
            int chunkPitch = 0;
            int chunkRows = 0;
        };

        void refreshChunk(Chunk* pChunk, TileLayerInternal* pLayer);

        int m_width = 0;
        int m_height = 0;
        int m_tileSize = 1;
        int m_layerCount = 0;
        int m_tilesetCount = 0;
        Layer **m_layers = nullptr;
        TileSet *m_tileSets = nullptr;
        Matrix m_transform = Matrix::Identity;
        onut::sample::Filtering m_filtering = OFilterNearest;
        OTextureRef m_pMinimap;
        bool* m_pCollisionTiles = nullptr;
    };
};

OTiledMapRef OGetTiledMap(const std::string& name);

#endif
