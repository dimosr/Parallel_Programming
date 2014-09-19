/*
 * tiled.h
 *
 * Header file for tiled matrix multiplication functions.
 */


typedef union _matrix {
    double **d;
    union _matrix ***p;
} *matrix;

void TiledMult(int, matrix, matrix, matrix);
void SerialMult(int, matrix, matrix, matrix);


