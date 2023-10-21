#ifndef _LOAD_STL_MODE_H_
#define _LOAD_STL_MODE_H_

#include <string>
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <regex>
#include <math.h>
#include <type_traits>
#include <unordered_map>
#include <map>

using x_position = double;
using y_position = double;
using z_position = double;
using Point = std::tuple<x_position, y_position, z_position>;
using triangleVertex = std::tuple<Point, Point, Point>;

#pragma pack(4)

struct Vertex {
    float xPosition;
    float yPosition;
    float zPosition;
    float homogeneous;
    float xNormal;
    float yNormal;
    float zNormal;
    bool operator == (const Vertex& vertex) const {
        if (this->xPosition == vertex.xPosition &&
            this->yPosition == vertex.yPosition &&
            this->zPosition == vertex.zPosition) {
            return true;
        }
        return false;
    }

    bool operator < (const Vertex& vertex) const {
        if (this->xPosition < vertex.xPosition) {
            return true;
        } else if (this->xPosition == vertex.xPosition) {
            if (this->yPosition < vertex.yPosition) {
                return true;
            } else if (this->yPosition == vertex.yPosition) {
                if (this->zPosition < vertex.zPosition) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    Vertex(float xPos = 0, float yPos = 0, float zPos = 0, float xNor = 0, float yNor = 0, float zNor = 0)
        : xPosition(xPos)
        , yPosition(yPos)
        , zPosition(zPos)
        , homogeneous(1)
        , xNormal(xNor)
        , yNormal(yNor)
        , zNormal(zNor) {}
};

struct VertexProperty
{
    double xMax;
    double xMin;
    double yMin;
    double yMax;
    double zMax;
    double zMin;
    Point center;
};

class LoadStlMode {
public:
    inline LoadStlMode(std::string modePath) : modePath_(modePath), haveLoadData_(false) {}

    inline bool LoadTriangleVertex() {
        std::ifstream stream;
        stream.open(modePath_.c_str(), std::ios::in);
        if (!stream.is_open()) {
            std::cout << modePath_ << " open failed" << std::endl;
            return false;
        }
        uint32_t triangleCount = 0;
        std::string lineBuffer;
        uint32_t count = 0;
        triangleVertex triangle;
        bool firstPoint = true;
        std::tuple<float, float, float, uint32_t> vertexNormal;
        while (std::getline(stream, lineBuffer)) {
            if (std::regex_match(lineBuffer.c_str(), std::regex("(\\s*facet normal(\\s([0-9]|e|\\.|\\+|-)+){3}\\s)$"))) {
                Point normal;
                if (!parserLineBuffer(lineBuffer, normal)) {
                    std::cout << "parser line buffer failed" << std::endl;
                    return false;
                } else {
                    std::get<0>(vertexNormal) = std::get<0>(normal);
                    std::get<1>(vertexNormal) = std::get<1>(normal);
                    std::get<2>(vertexNormal) = std::get<2>(normal);
                    std::get<3>(vertexNormal) = triangleCount;
                }
                continue;
            }

            if (std::regex_match(lineBuffer.c_str(), std::regex("(\\s*vertex(\\s([0-9]|e|\\.|\\+|-)+){3}\\s)$"))) {
                count++;
                Point point;
                if (!parserLineBuffer(lineBuffer, point)) {
                    std::cout << "parser line buffer failed" << std::endl;
                    return false;
                }
                Vertex vertex;
                vertex.xPosition = std::get<0>(point);
                vertex.yPosition = std::get<1>(point);
                vertex.zPosition = std::get<2>(point);
                vertexs_.push_back(vertex);
                auto iter = vertexNormals_.find(vertex);
                if (vertexNormals_.end() != iter) {
                    iter->second.push_back(vertexNormal);
                } else {
                    std::vector<decltype(vertexNormal)> normalsOfSameVertic;
                    normalsOfSameVertic.push_back(vertexNormal);
                    vertexNormals_.insert(std::pair(vertex, normalsOfSameVertic));
                }

                if (firstPoint) {
                    property_.xMin = std::get<0>(point);
                    property_.xMax = std::get<0>(point);
                    property_.yMin = std::get<1>(point);
                    property_.yMax = std::get<1>(point);
                    property_.zMin = std::get<2>(point);
                    property_.zMax = std::get<2>(point);
                    firstPoint = false;
                }
                if (std::get<0>(point) < property_.xMin)
                    property_.xMin = std::get<0>(point);
                if (std::get<0>(point) > property_.xMax)
                    property_.xMax = std::get<0>(point);
                if (std::get<1>(point) < property_.yMin)
                    property_.yMin = std::get<1>(point);
                if (std::get<1>(point) > property_.yMax)
                    property_.yMax = std::get<1>(point);
                if (std::get<2>(point) < property_.zMin)
                    property_.zMin = std::get<2>(point);
                if (std::get<2>(point) > property_.zMax)
                    property_.zMax = std::get<2>(point);               
            }

            lineBuffer.clear();

            if (count > 0 && (count%3) == 0 && (count/3) > areas_.size()) {
                triangleCount = count/3;
                float area = getTriangleArea(vertexs_[count-1], vertexs_[count-2], vertexs_[count-3]);
                areas_.push_back(area);
            }
        }
        stream.close();
        haveLoadData_ = true;
        std::get<0>(property_.center) = (property_.xMax + property_.xMin)/2;
        std::get<1>(property_.center) = (property_.yMax + property_.yMin)/2;
        std::get<2>(property_.center) = (property_.zMax + property_.zMin)/2;
        return true;
    }

    inline bool getProperty(VertexProperty& property) {
        if (!haveLoadData_) {
            std::cout << "please load model" << std::endl;
            return false;
        } else {
            property = property_;
            return true;
        }
    }

    inline std::vector<Vertex> getTriangleVertexs() {
        computeAverageVertecNormalByFaceNormal();
        return vertexs_;
    }

private:
    inline float getTriangleArea(const Vertex& point1, const Vertex& point2, const Vertex& point3) {
        float area = 0;
        std::tuple<float, float, float> p1p2((point2.xPosition - point1.xPosition), 
                                             (point2.yPosition - point1.yPosition),
                                             (point2.zPosition - point1.zPosition));
        std::tuple<float, float, float> p1p3((point3.xPosition - point1.xPosition), 
                                             (point3.yPosition - point1.yPosition),
                                             (point3.zPosition - point1.zPosition));
        area = pow(std::get<1>(p1p2)*std::get<2>(p1p3) - std::get<2>(p1p2)*std::get<1>(p1p3), 2) 
             + pow(std::get<2>(p1p2)*std::get<0>(p1p3) - std::get<0>(p1p2)*std::get<2>(p1p3), 2) 
             + pow(std::get<0>(p1p2)*std::get<1>(p1p3) - std::get<1>(p1p2)*std::get<0>(p1p3), 2);
        area = sqrt(area)/2;
        return area;
    }

    inline bool computeAverageVertecNormalByFaceNormal() {
        for (auto& vertex : vertexs_) {
            auto iter = vertexNormals_.find(vertex);
            if (iter == vertexNormals_.end()) {
                std::cout << "computeAverageVertecNormalByFaceNormal execute failed!" << std::endl;
                return false;
            }
            for (auto& normal : iter->second) {
                vertex.xNormal = vertex.xNormal + areas_[std::get<3>(normal)] * std::get<0>(normal);
                vertex.yNormal = vertex.yNormal + areas_[std::get<3>(normal)] * std::get<1>(normal);
                vertex.zNormal = vertex.zNormal + areas_[std::get<3>(normal)] * std::get<2>(normal);
            }
            std::cout << vertex.xPosition << '\t';
            std::cout << vertex.yPosition << '\t';
            std::cout << vertex.zPosition << std::endl;
            std::cout << vertex.xNormal << '\t';
            std::cout << vertex.yNormal << '\t';
            std::cout << vertex.zNormal << std::endl;
        }
        return true;
    }

    inline bool parserLineBuffer(std::string line, Point& point) {
        std::vector<std::string> XIndexYIndexZIndex;
        for (std::smatch result; std::regex_search(line, result, std::regex("(([\\+|\\-]*[0-9]*[\\.]*[0-9]+)|(e\\+00))"));) {
            XIndexYIndexZIndex.push_back(std::string(result.str()));
            line = result.suffix();
        }
        
        auto parserNum = [=](std::string numStr)->double{
            if (!strcmp(numStr.c_str(), "e+00"))
                return 0;
            int point = -1;
            bool positiveNum = true;
            double num = 0;
            for(auto i : numStr) {
                if (i == '-') {
                    positiveNum = false;
                    continue;
                }
                if (i == '.') {
                    point = 0;
                    continue;
                }
                if (point >= 0)
                    point++;
                if (i >= '0' && i <= '9')
                    num = 10 * num + (int)(i - '0');
            }
            if (point == -1) 
                point = 0;
            num = num/pow(10, point);
            if (!positiveNum)
                num = 0 - num;

            return num;
        };

        double x = parserNum(XIndexYIndexZIndex[0].c_str());
        int xIndex = parserNum(XIndexYIndexZIndex[1].c_str());
        if (xIndex >= 0) {
            x = x * pow(10, xIndex);
        } else {
            x = x/pow(10, abs(xIndex));
        }
        std::get<0>(point) = x;

        double y = parserNum(XIndexYIndexZIndex[2].c_str());
        int yIndex = parserNum(XIndexYIndexZIndex[3].c_str());
        if (yIndex >= 0) {
            y = y * pow(10, yIndex);
        } else {
            y = y/pow(10, abs(yIndex));
        }
        std::get<1>(point) = y;

        double z = parserNum(XIndexYIndexZIndex[4].c_str());
        int zIndex = parserNum(XIndexYIndexZIndex[5].c_str());
        if (zIndex >= 0) {
            z = z * pow(10, zIndex);
        } else {
            z = z/pow(10, abs(zIndex));
        }
        std::get<2>(point) = z;

        return true;
    }

private:
    const std::string modePath_;
    bool haveLoadData_;
    VertexProperty property_;
    std::vector<Vertex> vertexs_;
    std::map<Vertex, std::vector<std::tuple<float, float, float, uint32_t>>> vertexNormals_;
    std::vector<float> areas_;
};

#endif