
class Methods {
  public int pubInt() {
    return 1;
  }

  private float privFloat() {
    return 1.0f;
  }

  protected char protChar() {
    return '1';
  }

  public Methods retMethods() { return null; }
  public Object retObject() { return retMethods(); }
}
