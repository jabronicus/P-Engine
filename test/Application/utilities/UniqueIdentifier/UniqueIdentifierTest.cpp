//
// Created by paull on 2023-04-02.
//

#include <gtest/gtest.h>
#include "../../../../src/utilities/UniqueIdentifier/UniqueIdentifier.hpp"

using namespace pEngine::util;

class UniqueIdentifierTest : public ::testing::Test {
protected:
    void SetUp() override {
        uniqueIdentifier1 = std::make_shared<UniqueIdentifier>();
        uniqueIdentifier2 = std::make_shared<UniqueIdentifier>();
    }

    std::shared_ptr<UniqueIdentifier> uniqueIdentifier1;
    std::shared_ptr<UniqueIdentifier> uniqueIdentifier2;
};

TEST_F(UniqueIdentifierTest, CreateUniqueIdentifier) {
}

TEST_F(UniqueIdentifierTest, EnsureTwoCreatedUniqueIdentifiersAreNotIdentical) {
    ASSERT_TRUE(*uniqueIdentifier1 != *uniqueIdentifier2);
}

TEST_F(UniqueIdentifierTest, TestCopiedUniqueIdentifierIsEqualToOriginal) {
    std::shared_ptr<UniqueIdentifier> uniqueIdentifier = std::make_shared<UniqueIdentifier>();
    ASSERT_TRUE(*uniqueIdentifier != *uniqueIdentifier1 && *uniqueIdentifier != *uniqueIdentifier2);
    uniqueIdentifier = uniqueIdentifier2;
    ASSERT_TRUE(*uniqueIdentifier1 != *uniqueIdentifier);
    ASSERT_TRUE(*uniqueIdentifier == *uniqueIdentifier2);
}
