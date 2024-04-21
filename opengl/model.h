#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <gl3.h>

#include "shader.h"

namespace CG {

struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class mesh {
public:
    void mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures);
    void drawMesh(std::shared_ptr<shader> drawShader);
    ~mesh();

private:
    void setupMesh();

private:
    std::vector<vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<texture> textures_;
    unsigned int VAO_;
    unsigned int VBO_;
    unsigned int EBO_;
};

class model {
public:
    model(std::string path);
    ~model();
    void drawModel(std::shared_ptr<shader> drawShader);

private:
    void loadModel(string path);
    void processNode(aiNode *node, const aiScene *scene);
    mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    unsigned int textureFromFile(const char *path, const string &directory);

private:
    std::vector<mesh> meshes_;
    std::string directory_;
};

}

#endif