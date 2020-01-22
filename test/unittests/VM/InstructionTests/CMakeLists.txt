
target_sources(vm_test PUBLIC
  ConstantTests.cpp
  InvokeStaticTests.cpp
)

set_source_files_properties(InvokeStaticTests.cpp PROPERTIES COMPILE_FLAGS -rdynamic)
