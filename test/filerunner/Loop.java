
// ASSERT: javac %{file}
// RUN: %{JVM} Loop
// ASSERT: test %{status} -eq 15

class Loop {
    public static int loop() {
        int a = 0;
        for (int i = 0; i != 6; i++)
            a += i;
        return a;
    }

    public static void main(String[] args) {
        Natives.exit(loop());
    }
}
