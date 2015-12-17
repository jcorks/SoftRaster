#include "TransformMatrix.h"
#include <cmath>
#include <cstring>


static float IdentityMatrix[] = {
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f

};

#define DEGREES_TO_RADS(S__) (S__ * (3.1459 / 180.0))

static TransformMatrix TranslationMatrix;
static TransformMatrix RotateXMatrix;
static TransformMatrix RotateYMatrix;
static TransformMatrix RotateZMatrix;
static TransformMatrix RotationScratch;
static TransformMatrix ScaleScratch;

TransformMatrix::TransformMatrix(const TransformMatrix & matr) {
    memcpy(data, matr.data, sizeof(float)*16);
}

TransformMatrix & TransformMatrix::operator=(const TransformMatrix & matr) {
    memcpy(data, matr.data, sizeof(float)*16);
}

TransformMatrix::TransformMatrix(float * d) {
    memcpy(data, d, sizeof(float)*16);
}

TransformMatrix::TransformMatrix() {
    SetToIdentity();
}

void TransformMatrix::Transpose() {
    float temp;
    temp = data[0]; data[0] = data[15]; data[15] = temp;
    temp = data[1]; data[1] = data[11]; data[11] = temp;
    temp = data[2]; data[2] = data[7];  data[7]  = temp;
    temp = data[4]; data[4] = data[14]; data[14] = temp;
    temp = data[5]; data[5] = data[10]; data[10] = temp;
    temp = data[8]; data[8] = data[13]; data[13] = temp;
}



void TransformMatrix::SetToIdentity() {
    memcpy(data, IdentityMatrix, sizeof(float)*16);
}

TransformMatrix TransformMatrix::operator*(const TransformMatrix & other) const{
    TransformMatrix out;
    for(int y = 0; y < 4; ++y) {
        for(int x = 0; x < 4; ++x) {

            out.data[y*4 + x] = data[y*4]     * other.data[x] +
                                data[1 + y*4] * other.data[x + 4] +
                                data[2 + y*4] * other.data[x + 8] +
                                data[3 + y*4] * other.data[x + 12];
        }
    }

    return out;
}


void TransformMatrix::RotateByAngles(float x, float y, float z) {
    
    float xRads = DEGREES_TO_RADS(x);
    float yRads = DEGREES_TO_RADS(y);
    float zRads = DEGREES_TO_RADS(z);

    RotateXMatrix.data[0] = 1;
    RotateXMatrix.data[5] = cos(xRads);
    RotateXMatrix.data[9] = sin(xRads);
    RotateXMatrix.data[6] = -sin(xRads);
    RotateXMatrix.data[10] = cos(xRads);
    RotateXMatrix.data[15] = 1;


    RotateYMatrix.data[5] = 1;
    RotateYMatrix.data[0] = cos(yRads);
    RotateYMatrix.data[8] = sin(yRads);
    RotateYMatrix.data[2] = -sin(yRads);
    RotateYMatrix.data[10] = cos(yRads);
    RotateYMatrix.data[15] = 1;

    RotateZMatrix.data[10] = 1;
    RotateZMatrix.data[5] = cos(zRads);
    RotateZMatrix.data[1] = sin(zRads);
    RotateZMatrix.data[4] = -sin(zRads);
    RotateZMatrix.data[0] = cos(zRads);
    RotateZMatrix.data[15] = 1;



    //model = model * (RotateXMatrix.data *
    //                 RotateYMatrix.data *
    //                 RotateZMatrix.data);

    (*this) = (*this) * RotateXMatrix * RotateYMatrix * RotateZMatrix;
}




void TransformMatrix::Translate(float x, float y, float z) {
    data[3]  += data[0] *x + data[1] *y + data[2] *z;
    data[7]  += data[4] *x + data[5] *y + data[6] *z;
    data[11] += data[8] *x + data[9] *y + data[10]*z;
    data[15] += data[12]*x + data[13]*y + data[14]*z;
}


void TransformMatrix::Scale(float x, float y, float z) {
    ScaleScratch.data[0] = x;
    ScaleScratch.data[5] = y;
    ScaleScratch.data[10] = z;
    (*this) = (*this) * ScaleScratch;
}

void TransformMatrix::Inverse() {
    float inv[16], det;
    int i;

    inv[0] = data[5]  * data[10] * data[15] - 
             data[5]  * data[11] * data[14] - 
             data[9]  * data[6]  * data[15] + 
             data[9]  * data[7]  * data[14] +
             data[13] * data[6]  * data[11] - 
             data[13] * data[7]  * data[10];

    inv[4] = -data[4]  * data[10] * data[15] + 
              data[4]  * data[11] * data[14] + 
              data[8]  * data[6]  * data[15] - 
              data[8]  * data[7]  * data[14] - 
              data[12] * data[6]  * data[11] + 
              data[12] * data[7]  * data[10];

    inv[8] = data[4]  * data[9] * data[15] - 
             data[4]  * data[11] * data[13] - 
             data[8]  * data[5] * data[15] + 
             data[8]  * data[7] * data[13] + 
             data[12] * data[5] * data[11] - 
             data[12] * data[7] * data[9];

    inv[12] = -data[4]  * data[9] * data[14] + 
               data[4]  * data[10] * data[13] +
               data[8]  * data[5] * data[14] - 
               data[8]  * data[6] * data[13] - 
               data[12] * data[5] * data[10] + 
               data[12] * data[6] * data[9];

    inv[1] = -data[1]  * data[10] * data[15] + 
              data[1]  * data[11] * data[14] + 
              data[9]  * data[2] * data[15] - 
              data[9]  * data[3] * data[14] - 
              data[13] * data[2] * data[11] + 
              data[13] * data[3] * data[10];

    inv[5] = data[0]  * data[10] * data[15] - 
             data[0]  * data[11] * data[14] - 
             data[8]  * data[2] * data[15] + 
             data[8]  * data[3] * data[14] + 
             data[12] * data[2] * data[11] - 
             data[12] * data[3] * data[10];

    inv[9] = -data[0]  * data[9] * data[15] + 
              data[0]  * data[11] * data[13] + 
              data[8]  * data[1] * data[15] - 
              data[8]  * data[3] * data[13] - 
              data[12] * data[1] * data[11] + 
              data[12] * data[3] * data[9];

    inv[13] = data[0]  * data[9] * data[14] - 
              data[0]  * data[10] * data[13] - 
              data[8]  * data[1] * data[14] + 
              data[8]  * data[2] * data[13] + 
              data[12] * data[1] * data[10] - 
              data[12] * data[2] * data[9];

    inv[2] = data[1]  * data[6] * data[15] - 
             data[1]  * data[7] * data[14] - 
             data[5]  * data[2] * data[15] + 
             data[5]  * data[3] * data[14] + 
             data[13] * data[2] * data[7] - 
             data[13] * data[3] * data[6];

    inv[6] = -data[0]  * data[6] * data[15] + 
              data[0]  * data[7] * data[14] + 
              data[4]  * data[2] * data[15] - 
              data[4]  * data[3] * data[14] - 
              data[12] * data[2] * data[7] + 
              data[12] * data[3] * data[6];

    inv[10] = data[0]  * data[5] * data[15] - 
              data[0]  * data[7] * data[13] - 
              data[4]  * data[1] * data[15] + 
              data[4]  * data[3] * data[13] + 
              data[12] * data[1] * data[7] - 
              data[12] * data[3] * data[5];

    inv[14] = -data[0]  * data[5] * data[14] + 
               data[0]  * data[6] * data[13] + 
               data[4]  * data[1] * data[14] - 
               data[4]  * data[2] * data[13] - 
               data[12] * data[1] * data[6] + 
               data[12] * data[2] * data[5];

    inv[3] = -data[1] * data[6] * data[11] + 
              data[1] * data[7] * data[10] + 
              data[5] * data[2] * data[11] - 
              data[5] * data[3] * data[10] - 
              data[9] * data[2] * data[7] + 
              data[9] * data[3] * data[6];

    inv[7] = data[0] * data[6] * data[11] - 
             data[0] * data[7] * data[10] - 
             data[4] * data[2] * data[11] + 
             data[4] * data[3] * data[10] + 
             data[8] * data[2] * data[7] - 
             data[8] * data[3] * data[6];

    inv[11] = -data[0] * data[5] * data[11] + 
               data[0] * data[7] * data[9] + 
               data[4] * data[1] * data[11] - 
               data[4] * data[3] * data[9] - 
               data[8] * data[1] * data[7] + 
               data[8] * data[3] * data[5];

    inv[15] = data[0] * data[5] * data[10] - 
              data[0] * data[6] * data[9] - 
              data[4] * data[1] * data[10] + 
              data[4] * data[2] * data[9] + 
              data[8] * data[1] * data[6] - 
              data[8] * data[2] * data[5];

    det = data[0] * inv[0] + data[1] * inv[4] + data[2] * inv[8] + data[3] * inv[12];

    if (det == 0)
        return;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        data[i] = inv[i] * det;

}


void TransformMatrix::Transform(float * dest, float * src) {
    float * op1 = (float*)data;
    float * op2 = (float*)src;
    float * opTemp = op2;
    for(int i = 0; i < 4; ++i) {
        dest[i] = 0;
        op2 = opTemp;
        for(int n = 0; n < 4; ++n) {
            dest[i] += (*op1++) * (*op2++);
        }
    }
}



/// TRANSFORMATIONS ///



float * TransformMatrix::GetData() {
    return data;
}
