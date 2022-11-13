add_llvm_library(InstructionCount MODULE InstructionCount.cpp PLUGIN_TOOL clang)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  target_link_libraries(InstructionCount PRIVATE
      clangAST
      clangBasic
      clangFrontend
      LLVMSupport
      )
endif()
