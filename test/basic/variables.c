int global;

int foo(int formal_param);

int main(int argc, char * argv[]) {
  int local;
  
  {
    int inner_local;
  }

  return local + global;
}

