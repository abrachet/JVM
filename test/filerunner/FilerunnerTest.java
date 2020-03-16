// This file will not be run by the jvm but rather by java(1)
// It is merely to test the filerunner utility

// ASSERT: javac %{file}
// ASSERT: java FilerunnerTest
// ASSERT: test %{stdout} = "Hello world"
// RUN: rm -f FilerunnerTest.class

class FilerunnerTest {
    public static void main(String[] args) {
        System.out.println("Hello world");
    }
}
