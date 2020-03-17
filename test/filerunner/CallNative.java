
// ASSERT: javac %{file}
// RUN: %{JVM} CallNative
// ASSERT: test %{status} -eq 6

class CallNative {
    public static void main(String[] args) {
        Natives.exit(6);
    }
}
