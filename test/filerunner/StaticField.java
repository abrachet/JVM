
// ASSERT: javac %{file}
// ASSERT: %{JVM} -Xintmain StaticField 0
// RUN: %{JVM} -Xintmain StaticField 1
// ASSERT: test %{status} -eq 1
// RUN: %{JVM} -Xintmain StaticField 2
// ASSERT: test %{status} -eq 5

class StaticField {
    static int a;

    public static void main(int arg) {
        if (arg == 0)
            Natives.exit(a);
        if (arg == 1) {
            a = 1;
            Natives.exit(a);
        }
        if (arg == 2) {
            HasStatic.a = 5;
            Natives.exit(HasStatic.a);
        }
    }
}

class HasStatic {
    static int a;

}