
// ASSERT: javac %{file}
// ASSERT: %{JVM} DoNothing
// RUN: rm DoNothing.class

class DoNothing {
    public static void main(String[] args) {}
}
