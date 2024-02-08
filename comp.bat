gcc -Os -Wall -Wno-unused-function -Werror -Wextra -o main main.c gf25519.c utils.c modl.c ristretto255.c prng.c xxhash.c && main.exe

rem NOTE: You can set various flags in config.h