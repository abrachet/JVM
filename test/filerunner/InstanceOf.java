
// ASSERT: javac %{file}
// RUN: %{JVM} -Xintmain InstanceOf 0
// ASSERT: test %{status} -eq 1
// RUN: %{JVM} -Xintmain InstanceOf 1
// ASSERT: test %{status} -eq 1 # %{stderr}
// ASSERT: %{JVM} -Xintmain InstanceOf 2

class InstanceOf {
    // Casts not implemented yet.
    public static int fromBool(boolean b) {
        return b ? 1 : 0;
    }

    public static void main(int arg) {
        if (arg == 0) {
            int[] a = new int[1];
            Natives.exit(fromBool(a instanceof Object));
        }
        if (arg == 1) {
            Object o = new InstanceOf();
            Natives.exit(fromBool(o instanceof InstanceOf));
        }
        if (arg == 2) {
            Object o = new Object();
            Natives.exit(fromBool(o instanceof InstanceOf));
        }
    }
}
