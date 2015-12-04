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

        /// \brief Returns whether the Program is in a valid state
        ///
        /// If the program is in an invlaid state, it cannot be run
        bool IsValid() {return cachedProcs.size();}

        /// \brief Runs the program stages with the given data.
        /// The first stage will run over all vertices and the last stage
        /// must write a texture pointer.
        Run(Texture * texture, uint8_t * v, uint32_t sizeofVertex, uint32_t num);

        ~Program();

      private:
        Program(const std::string s) : status(s); 
        std::vector<ShaderProcedure*> cachedProcs;
        Texture src;    
        std::string status;
        uint8_t * inputCache;
        uint8_t * outputCache;
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
    void ClearPipeline();

  private:
    std::vector<ShaderProcedure*> procs;

};

}


#endif
