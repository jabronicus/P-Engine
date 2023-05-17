//
// Created by paull on 2022-06-22.
//

#include "../../../../../../src/GraphicsEngine/Backend/ApplicationContext/PApplicationContext/VulkanInstance/VulkanInstance.hpp"

#include <gtest/gtest.h>

using namespace PGraphics;

class VulkanInstanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        instance = std::make_shared<VulkanInstance>(VulkanInstance::CreationInput{
                "test"
        });
    }

    std::shared_ptr<VulkanInstance> instance;
};

TEST_F(VulkanInstanceTest, CreateInstance) {

}

// TODO - expand this as VulkanInstance becomes more configurable in the future