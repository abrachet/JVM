
class General {
    public static native int returnArg(int arg);

    public static void testIload() {
        int local0 = 5;
        int loal1 = returnArg(local0);
    }
}
