#ifndef __MVP_TRANSFORM_H__
#define __MVP_TRANSFORM_H__

#include <vector>
#include <string.h>
#include <math.h>

#include "../3rd-party-lib/glm/glm/glm.hpp"
#include "../3rd-party-lib/glm/glm/ext.hpp"
#include "loadStlMode.hpp"

#define MATRIX_SIZE 16
#define PI 3.1415926

#pragma pack(4)

struct Position
{
    float x;
    float y;
    float z;
    float rotationX;
    float rotationY;
    float rotationZ;
};

class TransformMatrix {
public:
    TransformMatrix() {
        memset(mMatrix4, 0, MATRIX_SIZE*sizeof(float));
    }

    TransformMatrix(float matrix4[]) {
        memcpy(mMatrix4, matrix4, MATRIX_SIZE*sizeof(float));
    }

    TransformMatrix(TransformMatrix& matrix) {
        for (unsigned int row = 0; row < 4; row++) {
            for (unsigned int col = 0; col < 4; col++) {
                (*this)(row, col) = matrix(row, col);
            }
        }
    }

    TransformMatrix(TransformMatrix&& matrix) {
        for (unsigned int row = 0; row < 4; row++) {
            for (unsigned int col = 0; col < 4; col++) {
                (*this)(row, col) = matrix(row, col);
            }
        }
    }

    TransformMatrix operator = (TransformMatrix& matrix) {
        for (unsigned int row = 0; row < 4; row++) {
            for (unsigned int col = 0; col < 4; col++) {
                (*this)(row, col) = matrix(row, col);
            }
        }
        return (*this);
    }

    TransformMatrix operator = (TransformMatrix&& matrix) {
        for (unsigned int row = 0; row < 4; row++) {
            for (unsigned int col = 0; col < 4; col++) {
                (*this)(row, col) = matrix(row, col);
            }
        }
        return (*this);
    }

    TransformMatrix operator + (TransformMatrix& transformMatrix) {
        for (unsigned int row = 0; row < 4; row++) {
            for (unsigned int col = 0; col < 4; col++) {
                (*this)(row, col) = (*this)(row, col) + transformMatrix(row, col);
            }
        }
        return (*this);
    }

    TransformMatrix operator - (TransformMatrix& transformMatrix) {
        for (unsigned int row = 0; row < 4; row++) {
            for (unsigned int col = 0; col < 4; col++) {
                (*this)(row, col) = (*this)(row, col) - transformMatrix(row, col);
            }
        }
        return (*this);
    }

    TransformMatrix operator * (TransformMatrix& transformMatrix) {
        TransformMatrix result;
        for (unsigned int row = 0; row < 4; row++) {
            for (unsigned int col = 0; col < 4; col++) {
                result(row, col) = result(row, col) + ((*this)(row, 0))*(transformMatrix(0, col));
                result(row, col) = result(row, col) + ((*this)(row, 1))*(transformMatrix(1, col));
                result(row, col) = result(row, col) + ((*this)(row, 2))*(transformMatrix(2, col));
                result(row, col) = result(row, col) + ((*this)(row, 3))*(transformMatrix(3, col));
            }
        }
        return result;
    }

    Vertex operator * (Vertex& vertex) {
        Vertex result;
        result.xPosition = ((*this)(0, 0))*vertex.xPosition + ((*this)(0, 1))*vertex.yPosition + ((*this)(0, 2))*vertex.zPosition + ((*this)(0, 3))*vertex.homogeneous;
        result.yPosition = ((*this)(1, 0))*vertex.xPosition + ((*this)(1, 1))*vertex.yPosition + ((*this)(1, 2))*vertex.zPosition + ((*this)(1, 3))*vertex.homogeneous;
        result.zPosition = ((*this)(2, 0))*vertex.xPosition + ((*this)(2, 1))*vertex.yPosition + ((*this)(2, 2))*vertex.zPosition + ((*this)(2, 3))*vertex.homogeneous;
        result.homogeneous = ((*this)(3, 0))*vertex.xPosition + ((*this)(3, 1))*vertex.yPosition + ((*this)(3, 2))*vertex.zPosition + ((*this)(3, 3))*vertex.homogeneous;
        return result;
    }

    float& operator () (unsigned int row, unsigned int col) {
        return mMatrix4[4 * col + row];
    }

    float* getMatrixData() {
        return mMatrix4;
    }

private:
    float mMatrix4[MATRIX_SIZE];
};

class MvpTransform {
public:
    void initModelTransformMatrix(TransformMatrix matrix) {
        mModelTransformMatrix = matrix;
    }

    void initViewTransformMatrix(Position cameraPosition) {
        TransformMatrix translationMatrix;
        translationMatrix(0, 0) = 1;
        translationMatrix(1, 1) = 1;
        translationMatrix(2, 2) = 1;
        translationMatrix(3, 3) = 1;
        translationMatrix(0, 3) = -cameraPosition.x;
        translationMatrix(1, 3) = -cameraPosition.y;
        translationMatrix(2, 3) = -cameraPosition.z;
        TransformMatrix XrotationMatrix;
        XrotationMatrix(0, 0) = 1;
        XrotationMatrix(1, 1) = cos(cameraPosition.rotationX*PI/180);
        XrotationMatrix(1, 2) = -sin(cameraPosition.rotationX*PI/180);
        XrotationMatrix(2, 1) = sin(cameraPosition.rotationX*PI/180);
        XrotationMatrix(2, 2) = cos(cameraPosition.rotationX*PI/180);
        XrotationMatrix(3, 3) = 1;
        TransformMatrix YrotationMatrix;
        YrotationMatrix(1, 1) = 1;
        YrotationMatrix(0, 0) = cos(cameraPosition.rotationY*PI/180);
        YrotationMatrix(0, 2) = sin(cameraPosition.rotationY*PI/180);
        YrotationMatrix(2, 0) = -sin(cameraPosition.rotationY*PI/180);
        YrotationMatrix(2, 2) = cos(cameraPosition.rotationY*PI/180);
        YrotationMatrix(3, 3) = 1;
        TransformMatrix ZrotationMatrix;
        ZrotationMatrix(2, 2) = 1;
        ZrotationMatrix(0, 0) = cos(cameraPosition.rotationZ*PI/180);
        ZrotationMatrix(0, 1) = -sin(cameraPosition.rotationZ*PI/180);
        ZrotationMatrix(1, 0) = sin(cameraPosition.rotationZ*PI/180);
        ZrotationMatrix(1, 1) = cos(cameraPosition.rotationZ*PI/180);
        ZrotationMatrix(3, 3) = 1;
        mViewTransformMatrix = translationMatrix*XrotationMatrix*YrotationMatrix*ZrotationMatrix;
    }

    void initProjectionTransformMatrix(float near, float far) {
        mProjectionTransformMatrix(0, 0) = 1080*near/1960;
        mProjectionTransformMatrix(1, 1) = near;
        mProjectionTransformMatrix(2, 2) = (near + far)/(2*far);
        mProjectionTransformMatrix(2, 3) = -near*far/(2*far);
        mProjectionTransformMatrix(3, 2) = 1;
    }

    TransformMatrix getMvpMatrix() {
        TransformMatrix mvpMatrix;
        mvpMatrix = mProjectionTransformMatrix*mViewTransformMatrix*mModelTransformMatrix;
        return mvpMatrix;
    }

    TransformMatrix getNormalMatrix() {
        TransformMatrix modelViewTranslate = mModelTransformMatrix*mViewTransformMatrix;
        glm::mat4 glmModelViewTranslate(
            modelViewTranslate(0,0),modelViewTranslate(0,1),modelViewTranslate(0,2),modelViewTranslate(0,3),
            modelViewTranslate(1,0),modelViewTranslate(1,1),modelViewTranslate(1,2),modelViewTranslate(1,3),
            modelViewTranslate(2,0),modelViewTranslate(2,1),modelViewTranslate(2,2),modelViewTranslate(2,3),
            modelViewTranslate(3,0),modelViewTranslate(3,1),modelViewTranslate(3,2),modelViewTranslate(3,3)
        );
        glm::mat4 glmResult = glm::transpose(glm::inverse(glmModelViewTranslate));
        TransformMatrix result = glm::value_ptr(glmResult);
        return result;
    }

    TransformMatrix getModelTransformMatrix() {
        return mModelTransformMatrix;
    }

    TransformMatrix getViewTransformMatrix() {
        return mViewTransformMatrix;
    }

    TransformMatrix getProjectionTransformMatrix() {
        return mProjectionTransformMatrix;
    }

private:
    TransformMatrix mModelTransformMatrix;
    TransformMatrix mViewTransformMatrix;
    TransformMatrix mProjectionTransformMatrix;
};

#endif