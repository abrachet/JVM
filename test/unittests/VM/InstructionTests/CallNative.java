
class CallNative {

    public static native int add(int arg1, int arg2);

    public static void callAdd() {
        int got = add(1, 2);
    }
}
