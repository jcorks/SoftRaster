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
/// Rendering of vertices is done by following transformations of data over a series of stages.
/// The Pipeline is an assembling of these stages. 
/// Each stage is added via PushExecutionStage where the push 
/// order is the order in which each stage will process vertices.
/// When the Pipeline is ready to be used, it must be compiled into a
/// Pipeline::Program, where it can then be passed to a Context instance for rendering.
///
class Pipeline {
  public:

    /// \brief An assembled pipeline that can process vertices.
    ///
    /// In this form, there is no external interface to modify the pipeline.
    /// Though, the program is the unit that actually performs the rendering. 
    /// Refer to Context for usage.
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
    /// Returns empty string if no error, else returns a string elaborating on what went wrong.
    /// When compiled into a program, the stages are run sequentially, passing
    /// output to the next stage's input.
    ///
    /// When pushing stages, it is important to note that
    /// the first stage expects there to be only one UserVertex.
    std::string PushExecutionStage (StageProcedure *);

    /// \brief Creates a rendering program.
    ///
    /// Takes all pushed execution stages and creates a
    /// Pipeline::Program instance reflecting the execution pipeline
    /// assembled. The order in which they are assembled is the 
    /// order in which they will process vertices.
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
/// during the rendering. The IO state is governed by  
/// the SignatureIO instances assigned in InputSignature() and OutputSignature(). 
///
/// It's important to note that the Read/Write template typename paramaters are 
/// just type hints and dont actually affect how many bytes are read or written.
///
class RuntimeIO {
  public:
    RuntimeIO();

    /// \brief Reads the next DataPrimitive into the given pointer. 
    ///
    /// The number of bytes read is equal to the SizeOf the slot currently being read
    ///
    template<typename T>
    void ReadNext(T *);

    /// \brief Reads the slot'th variable in the input. 
    ///
    /// This does not affect ReadNext calls.
    ///
    template<typename T>
    void ReadSlot(uint32_t slot, T *);

    /// \brief Returns the start of the memory block containing the 
    /// input information for this iteration. 
    /// 
    /// GetReadSize() returns the size of the input memory block in bytes. 
    ///
    inline uint8_t * GetReadPointer() { return inputCacheIter; }

    /// \brief returns the number of bytes of the memory block 
    /// pointed to by GetReadPointer()
    ///
    inline uint32_t GetReadSize() const {return inputSize; }


    /// \brief Writes the next registered data slot to be passed in.
    ///
    /// The number of bytes written is equal to the SizeOf() the slot currently being read
    ///
    template<typename T>
    void WriteNext(const T *);

    /// \brief Writes the data to the slot assigned in the StageProcedure::OutputSignature() call.
    ///
    template<typename T>
    void WriteSlot(uint32_t slot, const T *);


    /// \brief Returns the start of the memory block containing the 
    /// output information for this iteration. 
    /// 
    /// GetWriteSize() returns the size of the input memory block in bytes. 
    ///
    inline uint8_t * GetWritePointer() { return outputCacheIter; }

    /// \brief returns the number of bytes of the memory block 
    /// pointed to by GetWritePointer()
    ///
    inline uint32_t GetWriteSize() const {return inputSize; }



    /// \brief Retuns whether the execution unit has reached the last iteration.
    ///
    inline bool IsLastIteration()const          { return currentProcIter+1 == procIterCount; }

    /// \brief Returns the current iteration of the procedure starting at 0.
    ///
    inline uint32_t GetCurrentIteration() const { return currentProcIter; }

    /// \brief Returns the total number of iterations that this
    /// procedure runs this stage.
    ///
    inline uint32_t GetIterationCount()const    { return procIterCount;   }

    /// \brief Returns the target framebuffer for this render.
    ///
    inline Texture * GetFramebuffer()const { return fb; }



    /// \brief Commits the written data and marks the end of the output iteration
    ///
    /// For each commit made by the running StateProcedure, an additional iteration of the
    /// next StageProcedure is queued with the written outputs as its input.
    /// If a stage does not Commit before exhausting all of its own iterations,
    /// no additional stages are run. The order in which data is committed is not guaranteed 
    /// to be the same order it is run. 
    void Commit();
    
    /// \brief Returns the size of the given data type. Note that UserVertex is based
    /// on the what vertex type is being rendered with. (See Context).
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
