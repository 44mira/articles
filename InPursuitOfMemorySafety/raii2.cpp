struct Object {
  Resource *r1;
  Resource *r2;

  // initialize upon acquisition
  Object() {
    r1 = new Resource;
    r2 = new Resource;
  }

  // deinitialize upon destroy
  ~Object() {
    delete r1;
    delete r2;
  }
}
