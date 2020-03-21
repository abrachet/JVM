
// ASSERT: javac %{file}
// ASSERT: %{JVM} Sub

class InvokeSpecial {
    public int getInt() {
        return 0;
    }
}

class Sub extends InvokeSpecial {
    public int getInt() {
        return 1;
    }

    public int callSuper() {
        return super.getInt();
    }

    public static void main(String[] args) {
        Sub s = new Sub();
        Natives.exit(s.callSuper());
    }
}
