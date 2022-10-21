cmake_minimum_required(VERSION 2.8)  

add_llvm_library(omp-advisor MODULE 
    Loop.cpp
    Kernel.cpp
    OMPAdvisor_Visitor.cpp
    OMPAdvisor_ASTConsumer.cpp
    OMPAdvisor.cpp
    PLUGIN_TOOL clang
    )

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  target_link_libraries(omp-advisor PRIVATE
      clangAST
      clangBasic
      clangFrontend
      LLVMSupport
      )
endif()
