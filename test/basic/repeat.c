// use --unwind 6 --no-unwinding-assertions
// cover edges(@basicblockentry) passing @entry(main).(@4*.@5)>=4.@exit(main) or
// >=1

int main(int argc, char * argv[]) {
  int x;
  __CPROVER_assume(x>3);
  while (x > 0)
    --x;
  return 0;
}
