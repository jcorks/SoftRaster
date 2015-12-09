#include <SoftRaster/Pipeline.h>
#include <SoftRaster/StageProcedure.h>
#include <SoftRaster/Primitives.h>
#include <cassert>

using namespace SoftRaster;

const char * p_error_c__begin_signature_mismatch = "ERROR: The first StageProcedure must accept just one UserVertex as its input.";
const char * p_error_c__end_signature_mismatch   = "ERROR: The last StageProcedure must not return anything";
const char * p_error_c__io_mismatch              = "ERROR: The current end of the pipeline's return signature does not match the incoming StageProcedure's input signature.";

const uint32_t pipeline_program_init_cache_size     = 1024 * 512; // 512 KB should be fine to start
const float    pipeline_program_cache_resize_factor = 1.1f;

static uint32_t SignatureSize(const StageProcedure::SignatureIO &, uint32_t sizeofVertex);

std::string Pipeline::PushExecutionStage(StageProcedure * proc) {
    const StageProcedure::SignatureIO pipelineHead (
        {DataType::UserVertex}
    );
    if (procs.empty()) {
        // the first stage only accepts a user vertex
        if (!(proc->InputSignature() == pipelineHead)) {
            return p_error_c__begin_signature_mismatch;
        }
    } else {

        // check that io connects properly.
        // (would be better if it was completely type aware, but...)
        if (!(procs[procs.size()-1]->OutputSignature() ==
                                proc->InputSignature())) {
            return p_error_c__io_mismatch;
        }   
    }

    procs.push_back(proc);
    return "";

}


Pipeline::Program * Pipeline::Compile() {
    if (!procs.size()) return nullptr;
    if (!(procs[procs.size()-1]->OutputSignature() == StageProcedure::SignatureIO())) {
        return nullptr;
    }
    Program * out = new Program("");
    for(int i = 0; i < procs.size(); ++i) {
        out->cachedProcs.push_back(procs[i]);
    }

    return out;
}

void Pipeline::Clear() {
    procs.clear();
}






Pipeline::Program::Program(const std::string s) {
    status = s;
    src = nullptr;
}


std::string Pipeline::Program::GetStatus() {
    return status;
}

void Pipeline::Program::Run(
        Texture * framebuffer, 
        uint8_t * v, 
        uint32_t sizeofVertex,
        uint32_t num) {
    uint32_t currentSize;


    std::stack<DataType> input;
    static RuntimeIO runtimeIO;
    




    runtimeIO.RunSetup(v, sizeofVertex, num, framebuffer);

    for(uint32_t i = 0; i < cachedProcs.size(); ++i) {
        runtimeIO.NextProc(cachedProcs[i]);


        for(uint32_t n = 0; n < num; ++n) {
            (*cachedProcs[i])(&runtimeIO);
            runtimeIO.NextIter();
        }

    }
    

}


/////// RuntimeIO
RuntimeIO::RuntimeIO() {
    inputCacheSize = pipeline_program_init_cache_size;
    outputCacheSize = pipeline_program_init_cache_size;
    inputCache = new uint8_t[inputCacheSize];
    outputCache = new uint8_t[outputCacheSize];
}


void RuntimeIO::RunSetup(
    uint8_t * vData,
    uint32_t szVertex,
    uint32_t numIterations,
    Texture * framebuffer
) {
    sizeofVertex = szVertex;
    commitCount = 0;

    PrepareOutputCache(szVertex*numIterations);
    memcpy(outputCache, vData, szVertex*numIterations);
    fb = framebuffer;
}


void RuntimeIO::NextProc(const StageProcedure * p) {
    iterSlotIn = 0;
    iterSlotOut = 0;
    


    // since we are dealing with a new proc,
    // we need to correct our io signatures
    argInLocs.clear();
    argOutLocs.clear();

    auto input  = p->InputSignature();
    auto output = p->OutputSignature();
   
    inputSize  = SignatureSize(p->InputSignature(), sizeofVertex);
    outputSize = SignatureSize(p->OutputSignature(), sizeofVertex);

    auto iStack = input.Get();
    
    while(!iStack.empty()) {
        argInLocs.push_back(
            (argInLocs.empty() ? 
                0
            :                   
                argInLocs[argInLocs.size() - 1]
            )
            + SizeOf(iStack.top())
        ); 
        iStack.pop();
    }
    auto oStack = output.Get();
    while(!oStack.empty()) {
        argOutLocs.push_back(
            (argOutLocs.empty() ? 
                0
            :                   
                argOutLocs[argOutLocs.size() - 1]
            )
            + SizeOf(oStack.top())
        ); 
        oStack.pop();
    }



    // Reset the proc

    procIterCount   = commitCount;
    currentProcIter = 0;
    commitCount     = 0;

    PrepareInputCache(procIterCount * (inputSize));
    PrepareOutputCache(procIterCount * (inputSize));

    // Our output from the last run is always our input to the next run.
    std::swap(outputCache, inputCache);
    inputCacheIter = inputCache;
    outputCacheIter = outputCache;
}



void RuntimeIO::NextIter() {
    currentProcIter++;
    // want to reset the read iter in case user didnt
    // actually read in some args
    inputCacheIter += inputSize;
    iterSlotIn = 0;
    
}



void RuntimeIO::PrepareInputCache(uint32_t b) {
    if (b < inputCacheSize) return;
    inputCacheSize*=pipeline_program_cache_resize_factor;
    inputCache = (uint8_t*)realloc(inputCache, inputCacheSize);
}

void RuntimeIO::PrepareOutputCache(uint32_t b) {
    if (b < outputCacheSize) return;
    outputCacheSize*=pipeline_program_cache_resize_factor;
    outputCache = (uint8_t*)realloc(outputCache, outputCacheSize);
}

void RuntimeIO::Commit() {
    commitCount++;
    PrepareOutputCache((commitCount+1)*outputSize);
    outputCacheIter += outputSize;
    iterSlotOut = 0;
}

uint32_t RuntimeIO::SizeOf(DataType type) {
    switch(type) {
        case DataType::Null:    return 0;
        case DataType::Float:   return sizeof(float);
        case DataType::Int:     return sizeof(int);     
        case DataType::Vector2: return sizeof(Vector2); 
        case DataType::Vector3: return sizeof(Vector3); 
        case DataType::Vector4: return sizeof(Vector4); 
        case DataType::Mat4:    return sizeof(Mat4); 
        case DataType::UserVertex:
            return sizeofVertex;
        default: assert(!"Could not determine size of variable..");
        break;
    }
    return 0;
}






///// Statics//////
uint32_t SignatureSize(const StageProcedure::SignatureIO & sig, uint32_t sizeofVertex) {

    std::stack<DataType> stk = sig.Get();
    uint32_t size = 0;
    while(!stk.empty()) {
        switch(stk.top()) {
            case DataType::Null: break; 
            case DataType::Float:   size += sizeof(float);   break;
            case DataType::Int:     size += sizeof(int);     break;
            case DataType::Vector2: size += sizeof(Vector2); break;
            case DataType::Vector3: size += sizeof(Vector3); break;
            case DataType::Vector4: size += sizeof(Vector4); break;
            case DataType::Mat4:    size += sizeof(Mat4);    break;
            case DataType::UserVertex:
                size += sizeofVertex; break;
            default: assert(!"Could not determine size of variable..");
            break;
        }
        stk.pop();
    }
    return size;
}





