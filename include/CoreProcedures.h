#ifndef H_SOFTRASTER_CORE_PROCEDURE_INCLUDED
#define H_SOFTRASTER_CORE_PROCEDURE_INCLUDED


namespace SoftRaster {
class CoreProcedure {

    enum class ID {
        RasterizeTriangles, // previous stage must write out sizeof vertex bytes
        RasterizeLines,     // previous stage must write out sizeof vertex bytes
        
    };



    static ShaderProcedure * Get(Stages);


};

}
