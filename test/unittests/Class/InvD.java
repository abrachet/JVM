
class InvD {
  static void func() {
    Runnable r = () -> System.out.println("run");
    r.run();
  }
}
