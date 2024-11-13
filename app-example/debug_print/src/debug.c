#include "debug.h"

#define Buffer 16
static bool SendingisPending = 0;
static uint8_t data[Buffer];
int len = 0;
static const char digit[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
                             'A', 'B', 'C', 'D', 'E', 'F'};

static int getIntLen (long int value)
{
  int l = 1;
  while(value > 9)
  {
    l++;
    value /= 10;
  }
  return l;
}

int power(int a, int b)
{
  int i;
  int x = a;

  for (i = 1; i < b; i++)
  {
    x *= a;
  }

  return x;
}

static int itoa10Unsigned(putc_t putcf, unsigned long long int num)
{
  int len = 0;

  if (num == 0)
  {
    putcf('0');
    return 1;
  }

  unsigned long long int i = 1;

  while ((num / i) > 9)
  {
    i *= 10L;
  }

  do
  {
    putcf(digit[(num / i) % 10L]);
    len++;
  }
  while (i /= 10L);

  return len;
}

static int itoa10(putc_t putcf, long long int num, int precision)
{
  int len = 0;

  if (num == 0)
  {
    putcf('0');
    return 1;
  }

  long long unsigned int n = num;
  if (num < 0)
  {
    n = -num;
    putcf('-');
    len++;
  }

  int numLenght = getIntLen(num);
  if (numLenght < precision)
  {
    int fillWithZero = precision - numLenght;
    while (fillWithZero > 0)
    {
      putcf('0');
      len++;
      fillWithZero--;
    }
  }

  return itoa10Unsigned(putcf, n) + len;
}

int evprintf(putc_t putcf, const char * fmt, va_list ap)
{
  int len=0;
  float num;
  char* str;
  int precision;
  int width;
  char padChar;

  while (*fmt)
  {
    if (*fmt == '%')
    {
      precision = 6;
      padChar = ' ';
      width = 0;

      fmt++;
      if (*fmt == '%') {
        putcf(*fmt++);
        len++;
        continue;
      }

      while ('0' == *fmt)
      {
        padChar = '0';
        fmt++;
      }

			while(isdigit((unsigned)*fmt))
			{
				width *= 10;
				width += *fmt - '0';
				fmt++;
			}

      while (!isalpha((unsigned) *fmt))
      {
        if (*fmt == '.')
        {
          fmt++;
          if (isdigit((unsigned)*fmt))
          {
            precision = *fmt - '0';
            fmt++;
          }
        }
      }
      switch (*fmt++)
      {
        case 's':
          str = va_arg(ap, char* );
          while(*str)
          {
            putcf(*str++);
            len++;
          }
          break;
        default:
          break;
      }
    }
    else
    {
      putcf(*fmt++);
      len++;
    }
  }
  
  return len;
}

int eprintf(putc_t putcf, const char * fmt, ...)
{
  va_list ap;
  int len;

  va_start(ap, fmt);
  len = evprintf(putcf, fmt, ap);
  va_end(ap);

  return len;
}

int uartPutchar(int ch)
{
  if(SendingisPending)
  {
	  UART_DMA_Transmit_1(data, len);
    len = 0;
  }
  if(!SendingisPending)
  {
    if(len < Buffer)
    {
      data[len] = (uint8_t)ch;
      len++;
    }

    if(ch == '\n' || len >= Buffer)
    {
      SendingisPending = 1;
      UART_DMA_Transmit_1(data, len);
      len = 0;
    }
  }
  return ch;
}
