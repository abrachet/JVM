import java.PackagedClass;

class CallNative {

    public static native int add(int arg1, int arg2);

    public static void callAdd() {
        int got = add(1, 2);
    }

    public static void callRet1() {
        int got = PackagedClass.ret1();
    }

    public static void callTestName() {
        int got = PackagedClass.testClassName();
    }
}
