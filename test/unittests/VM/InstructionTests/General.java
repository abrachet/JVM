
class General {
    public int i;

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

    public static synchronized int synced() {
        return 1;
    }

    public static void callSynced() {
        synced();
    }

    public static void createNewGeneral() {
        General g = new General();
        int a = g.i;
    }

    public static int returnDefault() {
        General g = new General();
        return g.i;
    }

    public static int setAndReturn() {
        General g = new General();
        g.i = 5;
        return g.i;
    }

    public static int loop() {
        int a = 0;
        for (int i = 0; i != 5; i++)
            a += i;
        return a;
    }

    public static void callLoop() {
        int a = loop();
    }

    public static int getArrayLength() {
        int[] arr = new int[10];
        return arr.length;
    }

    public static void callGetArrayLength() {
        int a = getArrayLength();
    }

    public static int getAndSetArray() {
        int[] arr = new int[5];
        arr[3] = 5;
        return arr[3];
    }

    public static void callGetAndSetArray() {
        int a = getAndSetArray();
    }
}
