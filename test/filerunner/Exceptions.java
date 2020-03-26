
// ASSERT: javac %{file}
// RUN: %{JVM} -Xnoinvokespecial Exceptions
// ASSERT: test %{status} -eq 2 # %{stderr}

class Exceptions {

    public static int thrw() throws Exception {
        throw new Exception();
    }

    public static int callThrw() throws Exception {
        int a = 1;
        int b = thrw();
        return a + b;
    }

    public static void main(String[] args) {
        try {
            int exit = callThrw();
            Natives.exit(exit);
        } catch (NullPointerException e) {
            Natives.exit(3);
        } catch (Exception e) {
            Natives.exit(2);
        } catch (Throwable t) {
            Natives.exit(1);
        }
    }

}