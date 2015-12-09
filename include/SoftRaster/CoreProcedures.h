#ifndef H_SOFTRASTER_CORE_PROCEDURE_INCLUDED
#define H_SOFTRASTER_CORE_PROCEDURE_INCLUDED

#include <SoftRaster/StageProcedure.h>

namespace SoftRaster {

/// \brief Creates pre-defined ShaderProcedures
///
StageProcedure * CreateRasterizer(
    Polygon shape,
    DepthBuffering d= DepthBuffering::On
);



}
#endif
