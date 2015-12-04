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
    StageProcedure();

    /// \brief Retuns whether the execution unit has reached the last iteration.
    ///
    inline bool IsLastIteration()const          { return currentProcIter+1 == procIterCount; }

    /// \brief Returns the current iteration of the procedure.
    ///
    inline uint32_t GetCurrentIteration() const { return currentProcIter; }

    /// \brief Returns the total number of iterations that this
    /// procedure runs this stage.
    ///
    inline uint32_t GetIterationCount()const    { return procIterCount;   }

    /// \brief Returns the framebuffer
    ///
    inline Texture * GetFramebuffer() { return fb; }




    class AttributeBlock {
      public:
        void Add(DataType);
      private:
        friend class StageProcedure;
        std::stack<DataType> types;
    };

    /// \brief Returns the types and numbers of values that will be 
    /// read during each iteration
    ///
    virtual AttributeBlock InputSignature() = 0;

    /// \brief Returns the number of bytes that will be written to for each
    /// iteration that writes
    ///
    virtual AttributeBlock OuptutSignature() = 0;

    /// \brief Holds the runtime information for each iteration of the Procedure.
    ///
    /// RuntimeIO holds the iteractive input/ouput state of the StageProcedure
    /// during the running of the procedure. The IO state is governed by  
    /// the AttributeBlock's assigned in InputSignature() and OutputSignature(). 
    ///
    class RuntimeIO {
      public:

        /// \brief Reads the next DataPrimitive as a pointer to a variable to type T.
        ///
        DataPrimitive * ReadNext();

        /// \brief Reads all the remaning queued bytes for this iteration
        ///
        DataPrimitive * ReadSlot(uint32_t slot);



        /// \brief Writes data to be given to the next stage
        ///
        WriteNext(DataPrimitive *);

        /// \brief Writes the data to the slot assigned in the InputSignature() call.
        ///
        WriteSlot(uint32_t slot, DataPrimitive *);


        /// \brief Commits the written data and marks the end of the output iteration
        ///
        void Commit();
        
        /// \brief Returns the size of the data type
        ///
        uint32_t SizeOf(DataType);


      private:
        friend class StageProcedure;
        RuntimeIO(StageProcedure *, uint32_t sizeofVertex);
        uint32_t iterIn;
        uint32_t iterOut;
        uint32_t sizeIn;
        uint32_t sizeOut;
        uint8_t * DataIn;
        uint8_t * DataOut;
        StageProcedure * host;
    }




    /// \brief Iteration of the procedure.
    ///
    virtual void operator(Runtime *) = 0;

  private:
    void RuntimeSetup(
        uint32_t sizeofVertex_,
        uint8_t inputBuffer,
        uint8_t outputBuffer
    );

    uint32_t currentProcIter;
    uint32_t procIterCount;;
    uint32_t sizeofVertex;

    RuntimeIO * current;
    Texture * fb;

};

}
