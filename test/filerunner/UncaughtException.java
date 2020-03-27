
// ASSERT: javac %{file}
// RUN: %{JVM} -Xnoinvokespecial UncaughtException
// ASSERT: test %{stderr} = 'Exception in thread "main" java.lang.Exception' && test %{status} -eq 1

class UncaughtException {
    public static void main(String[] args) throws Exception {
        throw new Exception();
    }
}
