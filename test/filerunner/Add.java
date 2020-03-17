
// ASSERT: javac %{file}
// RUN: %{JVM} Add
// ASSERT: test %{status} -eq 34

class Add {
    public static void main(String[] args) {
        int a = 17;
        Natives.exit(a + a);
    }
}
