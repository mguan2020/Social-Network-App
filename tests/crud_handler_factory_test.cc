#include "gtest/gtest.h"
#include "factory/crud_handler_factory.h"
#include <fstream>
#include <filesystem>

class CrudHandlerFactoryTest : public :: testing::Test{ 
protected:
    NginxConfig config;
    RequestHandler * handler;
    CrudHandlerFactory factory;
    void SetUp(){
        handler = factory.create("/api", "/api/testing/1",config);
    }
    void TearDown(){
        delete handler;
    }
};

TEST_F(CrudHandlerFactoryTest, simpleTest){
    EXPECT_EQ(handler, nullptr);
}