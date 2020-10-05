# Kalyna

## Description
- *main.c* - provides a command line user interface;
- *kalyna.h, kalyna.c* - contains the main part of Kalyna alghorithm, fully compliant to standard: https://eprint.iacr.org/2015/650 ;
- *static.h, static.c* - contains the static part of algorithm (e.g. *s_box, standardconfigurations ...*);
- *types.h* - contains typedef aliases;
- *util.h, util.c* - contains utility methods (e.g. *shift, mul_bytes ...*);

### Unit test:
- *test_kalyna_512_512.c* - contains unit test for all kalyna standard methods, and examine example encryption/decryption on 512 block and 512 key sizes;
