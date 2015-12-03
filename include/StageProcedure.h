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
    
    /// \brief Returns the size of the vertex set that was run with
    ///
    inline uint32_t GetVertexSize()const        { return sizeofVertex;    }


    /// \brief Returns the framebuffer
    ///
    inline Texture * GetFramebuffer() { return fb; }



    /// \brief Returns the number of bytes that will be read upon each execution
    ///
    virtual uint32_t InputBytesToRead() const = 0;

    /// \brief Returns the number of bytes that will be written to for each
    /// iteration that writes
    ///
    virtual uint32_t OutputBytesToWrite() const  = 0;

    /// \brief Holds the runtime information for each iteration of the Procedure.
    ///
    /// RuntimeIO holds the iteractive input/ouput state of the StageProcedure
    /// during the running of the procedure
    ///
    class RuntimeIO {
      public:

        /// \brief Reads the next sizeof(T) bytes as a pointer to a variable to type T.
        ///
        template<typename T>
        T * Read<T>() { iterIn+=sizeof(T); return static_cast<T*>(DataIn+(iter-sizeof(T))); }   

        /// \brief Writes data to be given to the next stage
        ///
        template<typename T>
        void Write(T * data) { iterOut+=sizeof(T); memcpy(DataOut+(iter-sizeof(T)), data, sizeof(T)); } 

        /// \brief Commits the written data and marks the end of the output iteration
        ///
        void Commit();
        


      private:
        friend class StageProcedure;
        RuntimeIO(StageProcedure *);
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
    void RuntimeSetup(uint32_t sizeofVertex_);

    uint32_t currentProcIter;
    uint32_t procIterCount;;
    uint32_t sizeofVertex;
    RuntimeIO * current;
    Texture * fb;

};

}
