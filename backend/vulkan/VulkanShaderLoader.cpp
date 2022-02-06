#include "VulkanShaderLoader.h"
#include "Utilities.h"
#include "api/Logger.h"
#include "VulkanContext.h"

#include <fstream>

#include <ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

namespace glslang
{
    const TBuiltInResource DefaultTBuiltInResource = {
        /* .MaxLights = */ 32,
        /* .MaxClipPlanes = */ 6,
        /* .MaxTextureUnits = */ 32,
        /* .MaxTextureCoords = */ 32,
        /* .MaxVertexAttribs = */ 64,
        /* .MaxVertexUniformComponents = */ 4096,
        /* .MaxVaryingFloats = */ 64,
        /* .MaxVertexTextureImageUnits = */ 32,
        /* .MaxCombinedTextureImageUnits = */ 80,
        /* .MaxTextureImageUnits = */ 32,
        /* .MaxFragmentUniformComponents = */ 4096,
        /* .MaxDrawBuffers = */ 32,
        /* .MaxVertexUniformVectors = */ 128,
        /* .MaxVaryingVectors = */ 8,
        /* .MaxFragmentUniformVectors = */ 16,
        /* .MaxVertexOutputVectors = */ 16,
        /* .MaxFragmentInputVectors = */ 15,
        /* .MinProgramTexelOffset = */ -8,
        /* .MaxProgramTexelOffset = */ 7,
        /* .MaxClipDistances = */ 8,
        /* .MaxComputeWorkGroupCountX = */ 65535,
        /* .MaxComputeWorkGroupCountY = */ 65535,
        /* .MaxComputeWorkGroupCountZ = */ 65535,
        /* .MaxComputeWorkGroupSizeX = */ 1024,
        /* .MaxComputeWorkGroupSizeY = */ 1024,
        /* .MaxComputeWorkGroupSizeZ = */ 64,
        /* .MaxComputeUniformComponents = */ 1024,
        /* .MaxComputeTextureImageUnits = */ 16,
        /* .MaxComputeImageUniforms = */ 8,
        /* .MaxComputeAtomicCounters = */ 8,
        /* .MaxComputeAtomicCounterBuffers = */ 1,
        /* .MaxVaryingComponents = */ 60,
        /* .MaxVertexOutputComponents = */ 64,
        /* .MaxGeometryInputComponents = */ 64,
        /* .MaxGeometryOutputComponents = */ 128,
        /* .MaxFragmentInputComponents = */ 128,
        /* .MaxImageUnits = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
        /* .MaxCombinedShaderOutputResources = */ 8,
        /* .MaxImageSamples = */ 0,
        /* .MaxVertexImageUniforms = */ 0,
        /* .MaxTessControlImageUniforms = */ 0,
        /* .MaxTessEvaluationImageUniforms = */ 0,
        /* .MaxGeometryImageUniforms = */ 0,
        /* .MaxFragmentImageUniforms = */ 8,
        /* .MaxCombinedImageUniforms = */ 8,
        /* .MaxGeometryTextureImageUnits = */ 16,
        /* .MaxGeometryOutputVertices = */ 256,
        /* .MaxGeometryTotalOutputComponents = */ 1024,
        /* .MaxGeometryUniformComponents = */ 1024,
        /* .MaxGeometryVaryingComponents = */ 64,
        /* .MaxTessControlInputComponents = */ 128,
        /* .MaxTessControlOutputComponents = */ 128,
        /* .MaxTessControlTextureImageUnits = */ 16,
        /* .MaxTessControlUniformComponents = */ 1024,
        /* .MaxTessControlTotalOutputComponents = */ 4096,
        /* .MaxTessEvaluationInputComponents = */ 128,
        /* .MaxTessEvaluationOutputComponents = */ 128,
        /* .MaxTessEvaluationTextureImageUnits = */ 16,
        /* .MaxTessEvaluationUniformComponents = */ 1024,
        /* .MaxTessPatchComponents = */ 120,
        /* .MaxPatchVertices = */ 32,
        /* .MaxTessGenLevel = */ 64,
        /* .MaxViewports = */ 16,
        /* .MaxVertexAtomicCounters = */ 0,
        /* .MaxTessControlAtomicCounters = */ 0,
        /* .MaxTessEvaluationAtomicCounters = */ 0,
        /* .MaxGeometryAtomicCounters = */ 0,
        /* .MaxFragmentAtomicCounters = */ 8,
        /* .MaxCombinedAtomicCounters = */ 8,
        /* .MaxAtomicCounterBindings = */ 1,
        /* .MaxVertexAtomicCounterBuffers = */ 0,
        /* .MaxTessControlAtomicCounterBuffers = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
        /* .MaxGeometryAtomicCounterBuffers = */ 0,
        /* .MaxFragmentAtomicCounterBuffers = */ 1,
        /* .MaxCombinedAtomicCounterBuffers = */ 1,
        /* .MaxAtomicCounterBufferSize = */ 16384,
        /* .MaxTransformFeedbackBuffers = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances = */ 8,
        /* .MaxCombinedClipAndCullDistances = */ 8,
        /* .MaxSamples = */ 4,
        /* .maxMeshOutputVerticesNV = */ 256,
        /* .maxMeshOutputPrimitivesNV = */ 512,
        /* .maxMeshWorkGroupSizeX_NV = */ 32,
        /* .maxMeshWorkGroupSizeY_NV = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV = */ 1,
        /* .maxTaskWorkGroupSizeX_NV = */ 32,
        /* .maxTaskWorkGroupSizeY_NV = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV = */ 1,
        /* .maxMeshViewCountNV = */ 4,
        /* .maxDualSourceDrawBuffersEXT = */ 1,

        /* .limits = */ {
            /* .nonInductiveForLoops = */ 1,
            /* .whileLoops = */ 1,
            /* .doWhileLoops = */ 1,
            /* .generalUniformIndexing = */ 1,
            /* .generalAttributeMatrixVectorIndexing = */ 1,
            /* .generalVaryingIndexing = */ 1,
            /* .generalSamplerIndexing = */ 1,
            /* .generalVariableIndexing = */ 1,
            /* .generalConstantMatrixVectorIndexing = */ 1,
        } };
}

namespace VALX
{
    static EShLanguage ConvertShaderStageGlslang(ShaderStage stage)
    {
        switch (stage)
        {
        case VALX::ShaderStage::VERTEX:
            return EShLangVertex;
        case VALX::ShaderStage::TESSELLATION_CONTROL:
            return EShLangTessControl;
        case VALX::ShaderStage::TESSELLATION_EVALUATION:
            return EShLangTessEvaluation;
        case VALX::ShaderStage::GEOMETRY:
            return EShLangGeometry;
        case VALX::ShaderStage::FRAGMENT:
            return EShLangFragment;
        case VALX::ShaderStage::COMPUTE:
            return EShLangCompute;
        default:
            VALX_ASSERT(false && "invalid shader stage");
            return EShLangVertex;
        }
    }

    static glslang::EShSource ConvertShaderLanguage(ShaderLanguage language)
    {
        switch (language)
        {
        case VALX::ShaderLanguage::GLSL:
            return glslang::EShSourceGlsl;
        case VALX::ShaderLanguage::HLSL:
            return glslang::EShSourceHlsl;
        default:
            VALX_ASSERT(false && "invalid shader language");
            return glslang::EShSourceGlsl;
        }
    }

    ShaderStageInfo VulkanShaderLoader::LoadFromSourceFile(const std::string& filepath, ShaderStage stage, ShaderLanguage language)
    {
        std::ifstream file(filepath, std::ifstream::binary);
        return this->LoadFromSourceString(std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()), stage, language);
    }

    ShaderStageInfo VulkanShaderLoader::LoadFromBinaryFile(const std::string& filepath, ShaderStage stage, ShaderLanguage language)
    {
        std::ifstream file(filepath, std::ifstream::binary);
        ShaderStageInfo result;
        result.Bytecode = std::vector<char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        result.Stage = stage;
        return result;
    }

    ShaderStageInfo VulkanShaderLoader::LoadFromSourceString(const std::string& source, ShaderStage stage, ShaderLanguage language)
    {
        const char* sourcePtr = source.c_str();
        const int shaderVersion = 460;
        EShLanguage shaderStage = ConvertShaderStageGlslang(stage);
        glslang::TShader shader(shaderStage);
        shader.setStrings(&sourcePtr, 1);
        shader.setEnvInput(ConvertShaderLanguage(language), shaderStage, glslang::EShClient::EShClientVulkan, shaderVersion);
        shader.setEnvClient(glslang::EShClient::EShClientVulkan, (glslang::EShTargetClientVersion)GetVulkanContext()->GetAPIVersion());
        shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_5);
        bool isParsed = shader.parse(&glslang::DefaultTBuiltInResource, shaderVersion, false, EShMessages::EShMsgDefault);
        if (!isParsed)
        {
            const char* log = shader.getInfoLog();
            GetCurrentLogger()->LogError("VulkanShaderLoader", std::string(log));
            return ShaderStageInfo{ };
        }

        glslang::TProgram program;
        program.addShader(&shader);
        bool isLinked = program.link(EShMessages::EShMsgDefault);
        if (!isLinked)
        {
            const char* log = shader.getInfoLog();
            GetCurrentLogger()->LogError("VulkanShaderLoader", std::string(log));
            return ShaderStageInfo{ };

        }

        auto intermediate = program.getIntermediate(shaderStage);
        std::vector<uint32_t> bytecode;
        glslang::GlslangToSpv(*intermediate, bytecode);

        ShaderStageInfo result;
        result.Bytecode = std::vector<char>(reinterpret_cast<const char*>(bytecode.data()), reinterpret_cast<const char*>(bytecode.data() + bytecode.size()));
        result.Stage = stage;
        return result;
    }
}