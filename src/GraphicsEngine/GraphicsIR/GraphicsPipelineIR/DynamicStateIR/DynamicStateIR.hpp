#pragma once

#include <unordered_set>
#include "../../GraphicsIntermediateRepresentation.hpp"

namespace pEngine::girEngine::gir::pipeline {

    /**
     * This should basically just control whether you want to enable
     * Vulkan's dynamic state features, which I think(?) is just being able
     * to use dynamic offsets into buffers as opposed to having resources be
     * statically allocated or whatever the hell the terminology is.
     */
    struct DynamicStateIR {
        /**
         * These are individual bits of functionality in the graphics pipeline
         * that can be specified to be dynamic. Note that you'll need Vulkan 1.3+ for
         * a lot of these to work. In the future I may add support for the options
         * that require Vulkan extensions, but for now we'll just stick with the 1.3 sources.
         */
        enum class DynamicStateSource {
            DYNAMIC_VIEWPORT,
            DYNAMIC_SCISSOR,
            DYNAMIC_LINE_WIDTH,
            DYNAMIC_DEPTH_BIAS,
            DYNAMIC_BLEND_CONSTANTS,
            DYNAMIC_DEPTH_BOUNDS,
            DYNAMIC_STENCIL_COMPARE_MASK,
            DYNAMIC_STENCIL_WRITE_MASK,
            DYNAMIC_STENCIL_REFERENCE,
            DYNAMIC_CULL_MODE,
            DYNAMIC_FRONT_FACE,
            DYNAMIC_PRIMITIVE_TOPOLOGY,
            DYNAMIC_VIEWPORT_WITH_COUNT,
            DYNAMIC_SCISSOR_WITH_COUNT,
            DYNAMIC_VERTEX_INPUT_BINDING_STRIDE,
            DYNAMIC_DEPTH_TEST_ENABLE,
            DYNAMIC_DEPTH_WRITE_ENABLE,
            DYNAMIC_DEPTH_COMPARE_OP,
            DYNAMIC_DEPTH_BOUNDS_TEST_ENABLE,
            DYNAMIC_STENCIL_TEST_ENABLE,
            DYNAMIC_STENCIL_OP,
            DYNAMIC_RASTERIZER_DISCARD_ENABLE,
            DYNAMIC_DEPTH_BIAS_ENABLE,
            DYNAMIC_PRIMITIVE_RESTART_ENABLE,
        };

        bool dynamicStateEnabled = false;

        std::unordered_set<DynamicStateSource> dynamicStateSources = {};
    };
}