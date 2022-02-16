// See LICENSE for license details.
/* #define DEBUG_PRINTF */

#include "snprintf.h"
// #include "ape1210_uart.h"
// #include "ape1210.h"
#include "uart.h"

// #define USE_UART0 1
// #define USE_UART1 1

// #define PUTCHAR(x) uart_sendchar(SEEHI_UART1, x)
#define PUTCHAR(x) uart_putchar(x)

int my_vsnprintf(char *out, size_t n, const char *s, va_list vl)
{
  bool format = false;
  bool longarg = false;
  size_t pos = 0;
  for (; *s; s++)
  {
    if (format)
    {
      switch (*s)
      {
      case 'l':
        longarg = true;
        break;
      case 'p':
        longarg = true;
        if (++pos < n)
          out[pos - 1] = '0';
        if (++pos < n)
          out[pos - 1] = 'x';
      case 'x':
      {
        long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
        for (int i = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1; i >= 0; i--)
        {
          int d = (num >> (4 * i)) & 0xF;
          if (++pos < n)
            out[pos - 1] = (d < 10 ? '0' + d : 'a' + d - 10);
        }
        longarg = false;
        format = false;
        break;
      }
      case 'd':
      {
        long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
        if (num < 0)
        {
          num = -num;
          if (++pos < n)
            out[pos - 1] = '-';
        }
        long digits = 1;
        for (long nn = num; nn /= 10; digits++)
          ;
        for (int i = digits - 1; i >= 0; i--)
        {
          if (pos + i + 1 < n)
            out[pos + i] = '0' + (num % 10);
          num /= 10;
        }
        pos += digits;
        longarg = false;
        format = false;
        break;
      }
      case 's':
      {
        const char *s2 = va_arg(vl, const char *);
        while (*s2)
        {
          if (++pos < n)
            out[pos - 1] = *s2;
          s2++;
        }
        longarg = false;
        format = false;
        break;
      }
      case 'c':
      {
        if (++pos < n)
          out[pos - 1] = (char)va_arg(vl, int);
        longarg = false;
        format = false;
        break;
      }
      default:
        break;
      }
    }
    else if (*s == '%')
      format = true;
    else if (++pos < n)
      out[pos - 1] = *s;
  }
  if (pos < n)
    out[pos] = 0;
  else if (n)
    out[n - 1] = 0;
  return pos;
}

int snprintf(char *out, size_t n, const char *s, ...)
{
  va_list vl;
  va_start(vl, s);
  int res = my_vsnprintf(out, n, s, vl);
  va_end(vl);
  return res;
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 32

static unsigned divu10(unsigned n)
{
  unsigned q, r;

  q = (n >> 1) + (n >> 2);
  q = q + (q >> 4);
  q = q + (q >> 8);
  q = q + (q >> 16);
  q = q >> 3;
  r = n - q * 10;

  return q + ((r + 6) >> 4);
}

char remu10_table[16] = {
    0, 1, 2, 2, 3, 3, 4, 5,
    5, 6, 7, 7, 8, 8, 9, 0};

static unsigned remu10(unsigned n)
{
  n = (0x19999999 * n + (n >> 1) + (n >> 3)) >> 28;
  return remu10_table[n];
}

#define DEBUG_REG (0xFED11000 + 0x180)

#if 0
static void cfg_debug(char c){
// #ifdef CFG_DEBUG
    *(unsigned int volatile *)DEBUG_REG = c;
// #endif
}
#endif

// int putchar(int s)
// {
//   uart_sendchar(s);
//   return s;
// }

static void qprintchar(char **str, int c)
{
  if (str) {
    **str = c;
    ++(*str);
  } else {
    if (c == '\r') {
      // putchar('\n');
      // uart_sendchar(SEEHI_UART0, '\n');
      PUTCHAR('\n');
    }

    if (c == '\n') {
      // putchar('\r');
      // uart_sendchar(SEEHI_UART0, '\r');
      PUTCHAR('\r');
    }

    // putchar(c);
    // uart_sendchar(SEEHI_UART0, c);
    PUTCHAR(c);
  }
    // putchar((char)c);

  /* cfg_debug((char) c); */
}

static int qprints_string(char **out, const char *string, int width, int pad)
{
  register int pc = 0, padchar = ' ';

  if (width > 0)
  {
    register int len = 0;
    register const char *ptr;
    for (ptr = string; *ptr; ++ptr)
      ++len;
    if (len >= width)
      width = 0;
    else
      width -= len;
    if (pad & PAD_ZERO)
      padchar = '0';
  }
  if (!(pad & PAD_RIGHT))
  {
    for (; width > 0; --width)
    {
      qprintchar(out, padchar);
      ++pc;
    }
  }
  for (; *string; ++string)
  {
    qprintchar(out, *string);
    ++pc;
  }
  for (; width > 0; --width)
  {
    qprintchar(out, padchar);
    ++pc;
  }

  return pc;
}

// static int qprints(char **out, const char *string, int width, int pad)
static int qprints(char **out, const int *string, int width, int pad)
{
  register int pc = 0, padchar = ' ';

  if (width > 0)
  {
    register int len = 0;
    register const int *ptr;
    for (ptr = string; *ptr; ++ptr)
      ++len;
    if (len >= width)
      width = 0;
    else
      width -= len;
    if (pad & PAD_ZERO)
      padchar = '0';
  }
  if (!(pad & PAD_RIGHT))
  {
    for (; width > 0; --width)
    {
      qprintchar(out, padchar);
      ++pc;
    }
  }
  for (; *string; ++string)
  {
    qprintchar(out, *string);
    ++pc;
  }
  for (; width > 0; --width)
  {
    qprintchar(out, padchar);
    ++pc;
  }

  return pc;
}

static int qprinti(char **out, int i, int b, int sg, int width, int pad, char letbase)
{
  // char print_buf[PRINT_BUF_LEN];
  int print_buf[PRINT_BUF_LEN];
  // register char *s;
  register int *s = NULL;
  register int neg = 0, pc = 0;
  unsigned int u = i;

  if (i == 0)
  {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return qprints(out, print_buf, width, pad);
  }

  if (sg && b == 10 && i < 0)
  {
    neg = 1;
    u = -i;
  }

  s = print_buf + PRINT_BUF_LEN - 1;
  *s = '\0';

  // treat HEX and decimal differently
  if (b == 16)
  {
    // HEX
    while (u)
    {
      int t = u & 0xF;

      if (t >= 10)
        t += letbase - '0' - 10;

      *--s = t + '0';
      u >>= 4;
    }
  }
  else
  {
    // decimal
    while (u)
    {
      *--s = remu10(u) + '0';
      u = divu10(u);
    }
  }

  if (neg)
  {
    if (width && (pad & PAD_ZERO))
    {
      qprintchar(out, '-');
      ++pc;
      --width;
    }
    else
    {
      *--s = '-';
    }
  }
  return pc + qprints(out, s, width, pad);
}

static int qprint(char **out, const char *format, va_list va)
{
  register int width = 0, pad = 0;
  register int pc = 0;
  // char scr[2];
  int scr[2];

  for (; *format != 0; ++format)
  {
    if (*format == '%')
    {
      ++format;
      width = pad = 0;
      if (*format == '\0')
        break;
      if (*format == '%')
        goto out;
      if (*format == '-')
      {
        ++format;
        pad = PAD_RIGHT;
      }
      while (*format == '0')
      {
        ++format;
        pad |= PAD_ZERO;
      }
      for (; *format >= '0' && *format <= '9'; ++format)
      {
        width *= 10;
        width += *format - '0';
      }
      if (*format == 's')
      {
        register char *s = va_arg(va, char *);
        // pc += qprints (out, s?s:"(null)", width, pad);
        pc += qprints_string(out, s ? s : "(null)", width, pad);
        continue;
      }
      if (*format == 'd')
      {
        pc += qprinti(out, va_arg(va, int), 10, 1, width, pad, 'a');
        continue;
      }
      if (*format == 'u')
      {
        pc += qprinti(out, va_arg(va, unsigned int), 10, 0, width, pad, 'a');
        continue;
      }
      if (*format == 'x')
      {
        pc += qprinti(out, va_arg(va, uint32_t), 16, 0, width, pad, 'a');
        continue;
      }
      if (*format == 'X')
      {
        pc += qprinti(out, va_arg(va, uint32_t), 16, 0, width, pad, 'A');
        continue;
      }
      if (*format == 'c')
      {
        scr[0] = va_arg(va, int);
        scr[1] = '\0';
        pc += qprints(out, scr, width, pad);
        continue;
      }
    }
    else
    {
    out:
      qprintchar(out, *format);
      ++pc;
    }
  }

  if (out)
    **out = '\0';
  return pc;
}

int mprintf(const char *format, ...)
{
  /* #ifdef DEBUG_PRINTF */
  int pc = 0;
  va_list va;
  va_start(va, format);

  pc = qprint(0, format, va);

  va_end(va);
  return pc;
  /* #endif */
}

int mputs(const char *s)
{
  int i = 0;

  while (s[i] != '\0')
    // putchar(s[i++]);
    // uart_sendchar(SEEHI_UART0, s[i++]);
    PUTCHAR(s[i++]);

  // putchar('\r');
  // putchar('\n');
  // uart_sendchar(SEEHI_UART0, '\r');
  // uart_sendchar(SEEHI_UART0, '\n');
  PUTCHAR('\r');
  PUTCHAR('\n');

  return i;
}
