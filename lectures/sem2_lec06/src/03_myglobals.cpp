
const char* c = "foobar";

int i;
double j;

float f = 42.;
int ii = 24;

// uncomment to check compiling w/ '-O1'
// void doJob(int&);

extern "C" int foo() {
    int l = 42;
    // uncomment to check compiling w/ '-O1'
    // doJob(l);
    return l + i + ii;
}
