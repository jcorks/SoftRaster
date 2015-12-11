#ifndef H_SOFTRASTER_PIPELINE_INCLUDED
#define H_SOFTRASTER_PIPELINE_INCLUDED

/* SoftRaster: Pipeline
   Johnathan Corkery, 2015 */
#include <cstdint>
#include <string>
#include <vector>
#include <SoftRaster/Texture.h>


namespace SoftRaster {
class StageProcedure;
/// \brief The Pipeline controls how the rendering process occurs. 
///
/// The pipeline is where the stages of rendering are assembled.
/// Each StageProcedure is added in the order that they should be run in the 
/// resuling program. Pipeline also does some basic error checking
/// to make sure you're not making a revious error.
///
class Pipeline {
  public:

    /// \brief An assembled pipeline that can process vertices.
    ///
    /// In this form, there is no external interface to modify the pipeline
    /// The program is the unit that actually performs the rendering. 
    /// Refer to Context for running.
    ///
    class Program {
      public:
        ~Program();



        /// \brief Returns the status string of the Program
        ///
        std::string GetStatus();

      private:
        friend class Context;
        friend class Pipeline;
        void Run(
            Texture * framebuffer,
            uint8_t * v, 
            uint32_t sizeofVertex, 
            uint32_t num
        );

        Program(const std::string s);
        std::vector<StageProcedure*> cachedProcs;
        Texture * src;    
        std::string status;
    };


    /// \brief  Adds an additional execution stage
    ///
    /// Returns empty string if no error, else returns a string elaborating on what went wrong
    /// When compiled into a program, the stages are run sequentially, passing
    /// output to the next stage's input.
    std::string PushExecutionStage (StageProcedure *);

    /// \brief Creates a rendering program.
    ///
    /// Takes all pushed execution stages and creates a
    /// Pipeline::Program instance reflecting the execution pipeline
    /// assembled. The oder in which they are assembled is the 
    /// order in which they were pushed
    Program * Compile();

    /// \brief Rids the pipeline of all pushed stages.
    ///
    void Clear();

  private:
    std::vector<StageProcedure*> procs;

};



/// \brief Holds the runtime information for each iteration of the Procedure.
///
/// RuntimeIO holds the iteractive input/ouput state of the StageProcedure
/// during the running of the pipeline stages. The IO state is governed by  
/// the SignatureIO instances assigned in InputSignature() and OutputSignature(). 
///
/// It's important to note that the Read/Write tmeplate typename pramaters are 
/// just type hints and dont actually affect how many bytes are read or written.
///
class RuntimeIO {
  public:
    RuntimeIO();

    /// \brief Reads the next DataPrimitive as a pointer to a variable to type T.
    ///
    template<typename T>
    void ReadNext(T *);

    /// \brief Reads the slot'th variable in the input. This does not affect ReadNext calls.
    ///
    template<typename T>
    void ReadSlot(uint32_t slot, T *);

    /// \brief Returns the start of the memory block containing the 
    /// input information for this iteration. 
    /// 
    /// ReadStackSize() returns the size of the input memory block in bytes. 
    ///
    inline uint8_t * GetReadPointer() { return inputCacheIter; }

    /// \brief returns the number of bytes of the memory block 
    /// pointed to by ReadStackPointer()
    ///
    inline uint32_t GetReadSize() const {return inputSize; }


    /// \brief Writes the next registered data slot to be passed
    ///
    template<typename T>
    void WriteNext(const T *);

    /// \brief Writes the data to the slot assigned in the InputSignature() call.
    ///
    template<typename T>
    void WriteSlot(uint32_t slot, const T *);


    /// \brief Returns the start of the memory block containing the 
    /// input information for this iteration. 
    /// 
    /// ReadStackSize() returns the size of the input memory block in bytes. 
    ///
    inline uint8_t * GetWritePointer() { return outputCacheIter; }

    /// \brief returns the number of bytes of the memory block 
    /// pointed to by ReadStackPointer()
    ///
    inline uint32_t GetWriteSize() const {return inputSize; }



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
    inline Texture * GetFramebuffer()const { return fb; }



    /// \brief Commits the written data and marks the end of the output iteration
    ///
    void Commit();
    
    /// \brief Returns the size of the data type
    ///
    uint32_t SizeOf(DataType);


  private:
    friend class Pipeline::Program;
    void RunSetup(uint8_t * vdata, uint32_t szVertex, uint32_t numIterations, Texture *);
    void NextProc(const StageProcedure *);
    void NextIter();
    void PrepareInputCache(uint32_t bytes);
    void PrepareOutputCache(uint32_t bytes);
    

    uint32_t iterSlotIn;    
    uint32_t iterSlotOut;
    uint32_t inputSize;
    uint32_t outputSize;

    uint32_t sizeofVertex;
    uint32_t currentProcIter;
    uint32_t procIterCount;
    uint32_t commitCount;

    std::vector<uint32_t> argInLocs;
    std::vector<uint32_t> argOutLocs;

    uint8_t * inputCache;
    uint8_t * outputCache;
    uint8_t * inputCacheIter;
    uint8_t * outputCacheIter;
    uint32_t inputCacheSize;
    uint32_t outputCacheSize;

    Texture * fb;
};


}
#include <SoftRaster/PipelineImpl.hpp>


#endif
