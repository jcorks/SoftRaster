#ifndef H_SOFTRASTER_STAGE_PROCEDURE_INCLUDED
#define H_SOFTRASTER_STAGE_PROCEDURE_INCLUDED

/* SoftRaster: StageProcedure
   Johnathan Corkery, 2015 */


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


    inline bool IsLastIteration() { return currentProcIter+1 == procIterCount; }
    inline uint32_t GetCurrentIteration() { return currentProcIter; }
    inline uint32_t GetIterationCount()   { return procIterCount;   }
    

    // Returns the number of bytes that will be read upon each execution
    virtual inline uint32_t InputBytesToRead() const = 0;

    // Returns the number of bytes that will be written to for each
    // iteration that writes
    virtual inline uint32_t OutputBytesToWrite() const  = 0;

    // Holds the runtime information to Read/Write from/ to
    class RuntimeIO {
      public:

        // Reads next argument. 
        template<typename T>
        T * Read<T>() { iterIn+=sizeof(T); return static_cast<T*>(DataIn+(iter-sizeof(T))); }   

        // Confirms data for next stage.
        template<typename T>
        void Write<T>(T * data) { iterOut+=sizeof(T); memcpy(DataOut+(iter-sizeof(T)), data, sizeof(T)); } 

        


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





    virtual void operator(Runtime *) = 0;

  private:
    

    uint32_t currentProcIter;
    uint32_t procIterCount;;
    Runtime * current;

};

}
