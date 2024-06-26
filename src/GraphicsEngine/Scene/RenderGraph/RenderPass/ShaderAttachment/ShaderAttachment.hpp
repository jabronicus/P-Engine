//
// Created by paull on 2023-10-02.
//

#pragma once

#include <memory>
#include "../../../ShaderModule/ShaderModule.hpp"

namespace pEngine::girEngine::scene::graph::renderPass {

    /**
     *
     */
    struct ShaderAttachment {
        std::shared_ptr<ShaderModule> shaderModule = nullptr;
    };

} // scene
