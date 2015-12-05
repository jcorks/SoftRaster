#ifndef H_SOFTRASTER_PIPELINE_INCLUDED
#define H_SOFTRASTER_PIPELINE_INCLUDED

/* SoftRaster: Pipeline
   Johnathan Corkery, 2015 */


namespace SoftRaster {

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
    ///
    class Program {
      public:

        /// \brief Returns the status string of the Program
        ///
        std::string GetStatus();

        /// \brief Runs the program stages with the given data.
        /// The first stage will run over all vertices and the last stage
        /// must write a texture pointer.
        void Run(
            Texture * framebuffer, 
            uint8_t * v, 
            uint32_t sizeofVertex, 
            uint32_t num
        );


        /// \brief Holds the runtime information for each iteration of the Procedure.
        ///
        /// RuntimeIO holds the iteractive input/ouput state of the StageProcedure
        /// during the running of the procedure. The IO state is governed by  
        /// the AttributeBlock's assigned in InputSignature() and OutputSignature(). 
        ///
        class RuntimeIO {
          public:
            RuntimeIO();

            /// \brief Reads the next DataPrimitive as a pointer to a variable to type T.
            ///
            template<typename T>
            T * ReadNext<T>();

            /// \brief Reads the slot'th variable in the input. This does not affect ReadNext calls.
            ///
            template<typename T>
            T * ReadSlot<T>(uint32_t slot);



            /// \brief Writes the next registered data slot to be passed
            ///
            template<typename T>
            WriteNext(T *);

            /// \brief Writes the data to the slot assigned in the InputSignature() call.
            ///
            template<typename T>
            WriteSlot(uint32_t slot, const T *);




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



            /// \brief Commits the written data and marks the end of the output iteration
            ///
            void Commit();
            
            /// \brief Returns the size of the data type
            ///
            uint32_t SizeOf(DataType);


          private:
            friend class Program;
            void RunSetup(uint8_t * vdata, uint32_t szVertex, uint32_t numIterations, Texture *);
            void NextProc(const StageProcedure *);
            void NextIter();
            void PrepareInputCache(uint32_t bytes);
            void PrepareOutputputCache(uint32_t bytes);
            

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
            uint32_t inputCacheSize;
            uint32_t outputCacheSize;
    
            Texture * fb;
        }


        ~Program();

      private:
        friend class Program;
        Program(const std::string s);
        std::vector<ShaderProcedure*> cachedProcs;
        Texture * src;    
        std::string status;
    };


    /// \brief  Adds an additional execution stage
    ///
    /// Returns empty string if no error, else returns a string elaborating on what went wrong
    /// When compiled into a program, the stages are run sequentially, passing
    /// output to the next stage's input.
    std::string PushExecutionStage (ShaderProcedure *);

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
    std::vector<ShaderProcedure*> procs;

};
#include "PipelineImpl.hpp"
}


#endif
