#ifndef H_SOFTRASTER_CORE_PROCEDURE_INCLUDED
#define H_SOFTRASTER_CORE_PROCEDURE_INCLUDED

#include <SoftRaster/StageProcedure.h>

namespace SoftRaster {

/// \brief Holds built-in ShaderProcedures
class CoreProcedure {

    enum class ID {
        RasterizeTriangles, // previous stage must write out sizeof vertex bytes
        RasterizeLines,     // previous stage must write out sizeof vertex bytes
        
    };



    /// \brief creates a new instance of the specified ShaderProcedure.
    ///
    static StageProcedure * Create(ID);


};

}
