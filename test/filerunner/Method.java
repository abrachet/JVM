
// ASSERT: javac %{file}
// RUN: %{JVM} Method
// ASSERT: test %{status} -eq 13 # %{stderr}

class Method {
    int a;
    int b;

    int add() {
        return a + b;
    }

    public static void main(String[] args) {
        Method m = new Method();
        m.a = 6;
        m.b = 7;
        Natives.exit(m.add());
    }
}
