
// ASSERT: javac %{file}
// RUN: %{JVM} ClassFields
// ASSERT: test %{status} -eq 63

class ClassFields {
    int a;
    int b;

    public static int multiply(ClassFields cf) {
        return cf.a * cf.b;
    }

    public static void main(String[] args) {
        ClassFields cf = new ClassFields();
        cf.a = 7;
        cf.b = 9;
        int result = multiply(cf);
        Natives.exit(result);
    }
}
