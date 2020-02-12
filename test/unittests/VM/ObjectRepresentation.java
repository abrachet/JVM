
// Expected size 16
class ObjectRepresentationIJ {
    int I;  // Expected offset 0
    long J; // Expected offset 8
}

// Expected size 12
class ObjectRepresentationJI {
    long J; // Expected offset 0
    int I;  // Expected offset 8
}

// Expected size 8
class ObjectRepresentationII {
    int I1; // Expected offset 0
    int I2; // Expected offset 4
}

// Expected size 0
class ObjectRepresentationEmpty {}

// Expected size 8
class ObjectRepresentationL {
    Object L; // Expected offset 0
}