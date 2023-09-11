gcc -O0 -DEBUG_FLAG -DVERBOSE_FLAG -Wall -Werror -Wno-unused-value -Wextra -o main main.c curve25519.c utils.c modl.c ristretto255.c && main.exe

rem FLAGS: -DBIGENDIAN_FLAG -DEBUG_FLAG -DVERBOSE_FLAG