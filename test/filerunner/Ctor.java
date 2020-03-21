
// ASSERT: javac %{file}
// RUN: %{JVM} -Xintmain Ctor 0
// ASSERT: test %{status} -eq 13
// RUN: %{JVM} -Xintmain Ctor 1
// ASSERT: test %{status} -eq 14

class Ctor {
    int a;
    int b;

    Ctor() {
        a = 6;
        b = 7;
    }

    Ctor(int add) {
        this();
        a += add;
    }

    int add() {
        return a + b;
    }

    public static void main(int arg) {
        if (arg == 0) {
            Ctor c = new Ctor();
            Natives.exit(c.add());
        } else {
            Ctor c = new Ctor(arg);
            Natives.exit(c.add());
        }
    }
}
