
target_sources(vm_test PUBLIC
  ConstantTests.cpp
  InvokeStaticTests.cpp
)

target_link_options(vm_test PUBLIC -rdynamic)
