
// ASSERT: javac %{file}
// RUN: %{JVM} -Xintmain IntMain 1
// ASSERT: test %{status} -eq 1
// RUN: %{JVM} -Xintmain IntMain 34
// ASSERT: test %{status} -eq 34

class IntMain {
    public static void main(int arg) {
        Natives.exit(arg);
    }
}
