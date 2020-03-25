
class ExceptionTable {
    public static native void maybeThrows() throws Throwable;

    public static int a() {
        try {
            maybeThrows();
        } catch (Exception e) {
            return 1;
        } catch (Throwable e) {
            return 2;
        }
        return 3;
    }
}
