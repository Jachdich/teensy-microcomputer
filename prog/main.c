int some_other_func(int a) {
    return a * 3;
}
asm(".global _start");
int _start() {
    return 69;//some_other_func(3);
}
