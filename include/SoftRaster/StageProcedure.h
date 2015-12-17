#ifndef H_SOFTRASTER_STAGE_PROCEDURE_INCLUDED
#define H_SOFTRASTER_STAGE_PROCEDURE_INCLUDED

/* SoftRaster: StageProcedure
   Johnathan Corkery, 2015 */

#include <cstring>
#include <stack>
#include <vector>
#include <SoftRaster/Pipeline.h>

namespace SoftRaster {


/// \brief A single Pipeline computation unit.
///
/// A StageProcedure is a functor that processes data
/// in multiple iterations until its inputs are exhausted.
/// It's results then become the inputs for the next StageProcedure.
/// It is functionally modeled after GLSL programs and follows 
/// many of the same principles, but allows for more flexibility.
///
/// Each iteration (StageProcedure::operator()) is given a RuntimeIO instance,
/// for which inputs may be read and outputs may be written. See RuntimeIO for
/// details on how to interact with inputs and outputs.
///
/// The order of the iterations is not guaranteed.
/// 
class StageProcedure {
  public:


    /// \brief Holds a sequence of data types
    ///
    class SignatureIO {
      public:	
        SignatureIO(const std::vector<DataType> &);
        SignatureIO(){};

        /// \brief Pushes a data type to the IO signature.
        ///
        /// THe first pushed is the first argument.
        void AddSlot(DataType);
        
        /// \brief Returns whether two SignatureIO instances match.
        ///
        bool operator==(const SignatureIO & )const;             

        /// \brief Returns a queue of DataTypes with 
        /// the first argument at the top of the stack
        ///
        std::stack<DataType> Get() const;

        /// \brief
      private:
        std::vector<DataType> types;
    };

    /// \brief Returns the order and types of values that will be passed each iteration.
    ///
    /// The RuntimeIO::ReadNext() calls will read each variable in order of pushing.
    virtual SignatureIO InputSignature() const = 0;

    /// \brief Returns the number of bytes that will be written to for each
    /// iteration that writes
    ///
    /// The RuntimeIO::WriteNext() calls will read each variable in order of pushing.
    virtual SignatureIO OutputSignature() const  = 0;


    /// \brief Iteration of the procedure.
    ///
    virtual void operator()(RuntimeIO *) = 0;


};

}

#endif
