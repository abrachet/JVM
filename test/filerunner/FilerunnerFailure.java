// Like FilerunnerTest.java, this tests filerunner not the jvm

// ASSERT: javac %{file}
// RUN: java FilerunnerFailure
// ASSERT: test %{status} -eq 1 && test %{stderr} = stderr

class FilerunnerFailure {
    public static void main(String[] args) {
        System.err.println("stderr");
        System.exit(1);
    }
}
