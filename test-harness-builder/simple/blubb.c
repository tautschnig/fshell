void * malloc(unsigned);

int f(int x, int y); 
/*{
  return y;
}*/

/*int f() {
  int z;
  return z;
}*/

int bla() {
  int x;
  x = f(x, x);
  return x;
}

int main(int argc, char* argv[]) {
  int x;
  int y;
  char * a;
  a = (char*) malloc(10);
  a[0] = 'b';
  y = bla();
  if (f(x, y)) x = 4;
  if (argc > 5) x=2; else x=1;
  if (argc > 27) ++x; else --x;
  bla();
  return y;
}

