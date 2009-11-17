// cover edges(@conditionedge)
// cover union(edges(intersect(@func(foo),@conditionedge)),edges(intersect(@func(main),@conditionedge)))
// should yield the same results

int foo(int x) {
  switch (x) {
    case 5:
      return 3;
    case 6:
      return 4;
    case 7:
      return 5;
    default:
      return 6;
  }
}
  
int main(int argc, char * argv[]) {
  if (argc > 4)
    return foo(argc);
  return 0;
}
