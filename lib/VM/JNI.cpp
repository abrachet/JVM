
#include "JVM/VM/JNI.h"
#include <array>
#include <dlfcn.h>
#include <string>
#include <utility>
#include <vector>

void *getMethodHandle(const char *sym) { return dlsym(RTLD_DEFAULT, sym); }

void *loadLibrary(const char *name) {
  return dlopen(name, RTLD_LAZY | RTLD_GLOBAL);
}

template <typename T> struct type_constant { using type = T; };

template <size_t> struct FuncT : type_constant<uint64_t(uint64_t, ...)> {};
template <> struct FuncT<0> : type_constant<uint64_t(void)> {};

template <size_t... Ss>
static uint64_t callExtern(void *handle, const std::vector<uint64_t> &begin,
                           std::index_sequence<Ss...>) {
  using FunctionT = typename FuncT<sizeof...(Ss)>::type;
  return reinterpret_cast<FunctionT *>(handle)(begin[Ss]...);
}

template <size_t Size>
constexpr uint64_t callExtern(void *inv, const std::vector<uint64_t> &vec) {
  return callExtern(inv, vec, std::make_index_sequence<Size>());
}

using CallT = uint64_t(void *, const std::vector<uint64_t> &);

template <size_t... Ss>
constexpr std::array<CallT *, sizeof...(Ss)>
make_arr(std::index_sequence<Ss...> seq) {
  return {callExtern<Ss>...};
}

// Compiling 256 callExtern template instantiations takes too long. Testing will
// never call more than 10 arguments, so don't bother compiling all of them.
#ifdef TESTING
constexpr size_t maxMethodArgs = 10;
#else
constexpr size_t maxMethodArgs = 256;
#endif

constexpr auto array = make_arr(std::make_index_sequence<maxMethodArgs>());

uint64_t invoke(void *handle, const std::vector<uint64_t> &arguments) {
  return array[arguments.size()](handle, arguments);
}
