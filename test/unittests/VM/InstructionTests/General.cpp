
int returnArgGot;
extern "C" int Java_General_returnArg(void *, int arg) {
  return returnArgGot = arg;
}
