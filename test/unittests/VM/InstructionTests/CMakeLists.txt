
target_sources(vm_test PUBLIC
  ConstantTests.cpp
  General.cpp
  InvokeStaticJVMTest.cpp
  InvokeStaticNativeTest.cpp
  LoadsTest.cpp
  MathTest.cpp
  NewTest.cpp
  StackTest.cpp
)

target_link_options(vm_test PUBLIC -rdynamic)
