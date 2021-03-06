/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

//
/*  address="192.168.1.103" port="8090"  */
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>


//

char *lsprintf(char *dest, const char *format, ...) {
    va_list arg;
    int done;
    //
    if (!dest) return NULL;
    va_start(arg, format);
    done = vsprintf(dest, format, arg);
    va_end(arg);
    if (0x00 > done) return NULL;
    //
    return dest + done;
}
char *lscpy(char * dest, const char * src) {
    if (!dest) return NULL;
    register char *d = dest;
    register const char *s = src;
    while (*d = *s++) d++;
    return d;
}

/*
stpncpy(char * __restrict dst, const char * __restrict src, size_t n) {
    for (; n--; dst++, src++) {
        if (!(*dst = *src)) {
            char *ret = dst;
            while (n--)
                *++dst = '\0';
            return (ret);
        }
    }
    return (dst);
}
 * 
 char *
(__stpncpy) (char *dest, const char *src, size_t n)
{
  char c;
  char *s = dest;

  if (n >= 4)
    {
      size_t n4 = n >> 2;

      for (;;)
        {
          c = *src++;
          *dest++ = c;
          if (c == '\0')
            break;
          c = *src++;
          *dest++ = c;
          if (c == '\0')
            break;
          c = *src++;
          *dest++ = c;
          if (c == '\0')
            break;
          c = *src++;
          *dest++ = c;
          if (c == '\0')
            break;
          if (--n4 == 0)
            goto last_chars;
        }
      n -= dest - s;
      goto zero_fill;
    }

 last_chars:
  n &= 3;
  if (n == 0)
    return dest;

  for (;;)
    {
      c = *src++;
      --n;
      *dest++ = c;
      if (c == '\0')
        break;
      if (n == 0)
        return dest;
    }

 zero_fill:
  while (n-- > 0)
    dest[n] = '\0';

  return dest - 1;
}
 * 
char *
STRNCPY (char *s1, const char *s2, size_t n)
{
  char c;
  char *s = s1;
 
  --s1;
 
  if (n >= 4)
    {
      size_t n4 = n >> 2;
 
      for (;;)
    {
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        break;
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        break;
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        break;
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        break;
      if (--n4 == 0)
        goto last_chars;
    }
      n = n - (s1 - s) - 1;
      if (n == 0)
    return s;
      goto zero_fill;
    }
 
 last_chars:
  n &= 3;
  if (n == 0)
    return s;
 
  do
    {
      c = *s2++;
      *++s1 = c;
      if (--n == 0)
    return s;
    }
  while (c != '\0');
 
 zero_fill:
  do
    *++s1 = '\0';
  while (--n > 0);
 
  return s;
}
*/

char *lsncpy(char *dest, const char *src, size_t n) {
    char c;
    char *s = dest;
    //
    if (!dest) return NULL;
    if (n >= 4) {
        size_t n4 = n >> 2;
        for (;;) {
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            if (--n4 == 0) goto last_chars;
        }
        return dest;
    }
last_chars:
    n &= 3;
    if (n == 0) {
        *dest = '\0';
        return dest;
    }
    for (;;) {
        c = *src++;
        *dest = c;
        if (c == '\0') break;
        dest++;
        if (--n == 0) {
            *dest = '\0';
            return dest;
        }
    }
    //
    return dest;
}

/*
 char *strcat (char *dest, const char *src) {
  strcpy (dest + strlen (dest), src);
  return dest;
}
 */
char *lscat(char * dest, const char * src) {
    if (!dest) return NULL;
    return lscpy(dest + strlen(dest), src);
}

/*
 char *strncat (char *dest, const char *src, size_t n) {
  char *destptr = dest + strlen (dest);
  for (; n > 0 && (*destptr = *src) != '\0'; src++, destptr++, n--);
  if (n == 0) *destptr = '\0';
  return dest;
}
 */
char *lsncat(char *dest, const char *src, size_t n) {
    if (!dest) return NULL;
    char *destptr = dest + strlen(dest);
    for (; n > 0 && (*destptr = *src) != '\0'; src++, destptr++, n--);
    if (n == 0) *destptr = '\0';
    return destptr;
}



//

int main(int argc, char** argv) {
    char *last;
    char xxx[1024];

    xxx[0] = 0x00;


    last = lsncp(xxx, "", 0);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "1", 1);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "12", 3);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "123", 2);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "1234", 4);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "12345", 5);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "123456", 6);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "1234567", 5);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "12345678", 4);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    last = lsncp(xxx, "123456789", 3);
    printf("last|%s|\n", last);
    printf("|%s|\n", xxx);
    //
}



