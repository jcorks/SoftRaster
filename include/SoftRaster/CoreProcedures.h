#ifndef H_SOFTRASTER_CORE_PROCEDURE_INCLUDED
#define H_SOFTRASTER_CORE_PROCEDURE_INCLUDED

#include <SoftRaster/StageProcedure.h>

namespace SoftRaster {

/// \brief Creates pre-defined ShaderProcedures
class CoreProcedure {

    enum class ID {
    };

    enum class Polygon {
        Triangles,
        Lines,
        Points
    };

    enum class DepthBuffering {
        On,
        Off
    };




    /// \brief creates a new instance of the specified ShaderProcedure.
    ///
    static StageProcedure * Create(ID);

    static StageProcedure * CreateRasterizer(
        Polygon shape,
        DepthBuffering d= DepthBuffering::On
    );


};

}
#endif
