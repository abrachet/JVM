
// ASSERT: javac %{file}
// Array of size 1 will have exit code 0
// ASSERT: %{JVM} -Xintmain Arrays 1
// RUN: %{JVM} -Xintmain Arrays 5
// ASSERT: test %{status} -eq 10
// RUN: %{JVM} -Xintmain Arrays 7
// ASSERT: test %{status} -eq 21

class Arrays {

    public static int[] getArray(int size) {
        int[] a = new int[size];
        for (int i = 0; i != size; i++)
            a[i] = i;
        return a;
    }

    public static int sumArray(int[] arr) {
        int sum = 0;
        for (int i = 0; i != arr.length; i++)
            sum += arr[i];
        return sum;
    }

    public static void main(int length) {
        int[] arr = getArray(length);
        Natives.exit(sumArray(arr));
    }
}
