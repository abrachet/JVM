
class General {
    public static native int returnArg(int arg);

    public static void testIload() {
        int local0 = 5;
        int loal1 = returnArg(local0);
    }

    public static int returnArgJVM(int arg) {
        return arg;
    }

    public static void callReturnArgJVM() {
        int local = returnArgJVM(1);
    }
}
