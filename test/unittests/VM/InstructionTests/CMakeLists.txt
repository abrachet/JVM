
target_sources(vm_test PUBLIC
  ConstantTests.cpp
  General.cpp
  InvokeStaticTests.cpp
  LoadsTest.cpp
)

target_link_options(vm_test PUBLIC -rdynamic)
