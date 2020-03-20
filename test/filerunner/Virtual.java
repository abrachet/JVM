
// ASSERT: javac %{file}
// ASSERT: %{JVM} Derived
// RUN: %{JVM} DerivedNoImpl
// ASSERT: test %{status} -eq 1 # %{stderr}

class Virtual {
    public int virtual() {
        return 1;
    }
}

class Derived extends Virtual {
    public int virtual() {
        return 0;
    }

    public static void main(String[] args) {
        Virtual b = new Derived();
        Natives.exit(b.virtual());
    }
}

class DerivedNoImpl extends Virtual {
    public static void main(String[] args) {
        Virtual b = new DerivedNoImpl();
        Natives.exit(b.virtual());
    }
}
