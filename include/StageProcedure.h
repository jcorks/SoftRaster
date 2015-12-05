#ifndef H_SOFTRASTER_STAGE_PROCEDURE_INCLUDED
#define H_SOFTRASTER_STAGE_PROCEDURE_INCLUDED

/* SoftRaster: StageProcedure
   Johnathan Corkery, 2015 */

#include <cstring>

namespace SoftRaster {


/// \brief A single Pipeline computation unit.
///
/// A StageProcedure is a functor that processes data
/// in multiple iterations until its inputs are exhausted.
/// It's results then because the inputs for the next StageProcedure.
/// It is functionally modeled after Shader programs and follows 
/// many of the same principles, but allows for more flexibility
///
/// 
/// Each iteration, the StageProcedure interacts with the RuntimeIO object
/// to process inputs and outputs. On each iteration, the instance is 
/// passed an input byte reserve and output byte reserve, accessed via
/// RuntimeIO::Read / Write respectively.
/// 
class StageProcedure {
  public:


    /// \brief Holds a sequence of data types
    ///
    class SignatureIO {
      public:
        SignatureIO(const std::vector<DataType> &);
        SignatureIO();

        /// \brief pushes a data type to the IO signature;
        ///
        void AddSlot(DataType);
        
        /// \brief Returns whether two SignatureIO instances match.
        ///
        bool operator==(const SignatrueIO & )const;             

        /// \brief Returns a queue of DataTypes with 
        /// the first argument at the top of the stack;
        std::stack<DataType> Get() const;

        /// \brief
      private:
        std::vector<DataType> types;
    };

    /// \brief Returns the types and numbers of values that will be 
    /// read during each iteration
    ///
    virtual SignatureIO InputSignature() = 0;

    /// \brief Returns the number of bytes that will be written to for each
    /// iteration that writes
    ///
    virtual SignatureIO OuptutSignature() = 0;


    /// \brief Iteration of the procedure.
    ///
    virtual void operator(Runtime *) = 0;


};

}
