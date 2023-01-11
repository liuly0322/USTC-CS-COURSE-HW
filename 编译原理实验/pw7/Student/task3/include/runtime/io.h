#ifndef M_DRIVE_IO_H
#define M_DRIVE_IO_H

#ifdef __cplusplus
extern "C" {
#endif

int getint();
float getfloat();
int getch();
int getarray(int a[]);
int getfarray(float a[]);
void putint(int a);
void putfloat(float a);
void putch(int a);
void putarray(int n, int a[]);
void putfarray(int n, float a[]);
int current_time();

#ifdef __cplusplus
}
#endif

#endif