#ifndef DC_H_TRANSFORM_MATRIX_INCLUDED
#define DC_H_TRANSFORM_MATRIX_INCLUDED



namespace SoftRaster {

/// \brief A transform representated as a row-major matrix. Ported from libDynacoe 
///
class TransformMatrix {
  public:
    TransformMatrix(const TransformMatrix &);
    TransformMatrix & operator=(const TransformMatrix &);
    TransformMatrix(float *);
    TransformMatrix(); // identity, row major

    /// \brief Transposes the matrix.
    ///
    void Transpose();

    /// \brief Inverts the matrix.
    ///
    void Inverse();

    /// \brief Returns the internal representation of the TransformMatrix.
    ///
    float * GetData();

    /// \brief Multiplies 2 matrices.
    ///
    TransformMatrix operator*(const TransformMatrix &) const;
    
    /// \brief Rotates the matrix about the Euler angles psi, theta, and phi.
    ///
    void RotateByAngles(float x, float y, float z);

    /// \brief Expresses a translation by x, y, and z
    ///
    void Translate(float x, float y, float z);

    /// \brief Expresses a scaling in the x, y, and z directions.
    ///
    void Scale(float x, float y, float z);


    /// \brief Sets the matrix to the identity matrix, removing all transformations.
    ///
    void SetToIdentity();
    
  private:
    float data[16];
};
}

#endif  
