#include "SDTCommon.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double SDT_sampleRate = 0.0;
double SDT_timeStep = 0.0;

void SDT_setSampleRate(double sampleRate) {
  SDT_sampleRate = sampleRate;
  if (SDT_sampleRate > SDT_MICRO) SDT_timeStep = 1.0 / sampleRate;
}

unsigned int SDT_argMax(double *x, unsigned int n, double *maxOut) {
  double max;
  unsigned int i, argMax;

  max = x[0];
  argMax = 0;
  for (i = 1; i < n; i++) {
    if (x[i] > max) {
      max = x[i];
      argMax = i;
    }
  }
  if (maxOut) *maxOut = max;
  return argMax;
}

unsigned int SDT_argMin(double *x, unsigned int n, double *minOut) {
  double min;
  unsigned int i, argMin;

  min = x[0];
  argMin = 0;
  for (i = 1; i < n; i++) {
    if (x[i] < min) {
      min = x[i];
      argMin = i;
    }
  }
  if (minOut) *minOut = min;
  return argMin;
}

double SDT_average(double *x, unsigned int n) {
  double sum;
  unsigned int i;

  sum = 0;
  for (i = 0; i < n; i++) {
    sum += x[i];
  }
  return sum / n;
}

unsigned int SDT_bitReverse(unsigned int u, unsigned int bits) {
  u = ((u >> 0x01) & 0x55555555) | ((u & 0x55555555) << 0x01);
  u = ((u >> 0x02) & 0x33333333) | ((u & 0x33333333) << 0x02);
  u = ((u >> 0x04) & 0x0F0F0F0F) | ((u & 0x0F0F0F0F) << 0x04);
  u = ((u >> 0x08) & 0x00FF00FF) | ((u & 0x00FF00FF) << 0x08);
  u = (u >> 0x10) | (u << 0x10);
  return u >> (sizeof(u) * CHAR_BIT - bits);
}

void SDT_blackman(double *sig, int n) {
  int i, j;
  double w, scale;

  for (i = 0; i < n / 2; i++) {
    j = n - i - 1;
    w = SDT_TWOPI * i / (n - 1);
    scale = 0.42 - 0.5 * cos(w) + 0.08 * cos(2 * w);
    sig[i] *= scale;
    sig[j] *= scale;
  }
}

long SDT_clip(long x, long min, long max) {
  if (x < min)
    x = min;
  else if (x > max)
    x = max;
  return x;
}

double SDT_expRand(double lambda) { return -log(1.0 - SDT_frand()) / lambda; }

double SDT_fclip(double x, double min, double max) {
  x = fmax(min, x);
  x = fmin(x, max);
  return x;
}

double SDT_frand() { return rand() / ((double)RAND_MAX + 1); }

void SDT_gaussian1D(double *x, double sigma, int n) {
  double mid, sum, det, num;
  int i;

  mid = 0.5 * n;
  sum = 0.0;
  det = 2.0 * sigma * sigma * n;
  for (i = 0; i < n; i++) {
    num = i - mid + 0.5;
    x[i] = exp(-num * num / det);
    sum += x[i];
  }
  for (i = 0; i < n; i++) {
    x[i] /= sum;
  }
}

double SDT_gravity(double mass) { return SDT_EARTH * mass; }

void SDT_hanning(double *sig, int n) {
  int i, j;
  double scale;

  for (i = 0; i < n / 2; i++) {
    j = n - i - 1;
    scale = 0.5 - 0.5 * cos(SDT_TWOPI * i / (n - 1));
    sig[i] *= scale;
    sig[j] *= scale;
  }
}

void SDT_haar(double *sig, long n) {
  double tmp[n];
  long x, i, j, k, l;

  memcpy(tmp, sig, n * sizeof(double));
  n /= 2;
  for (x = 0; x < n; x++) {
    i = x;
    j = i + n;
    k = 2 * x;
    l = k + 1;
    sig[i] = (tmp[k] + tmp[l]) / SDT_SQRT2;
    sig[j] = (tmp[k] - tmp[l]) / SDT_SQRT2;
  }
}

void SDT_ihaar(double *sig, long n) {
  double tmp[n];
  long x, i, j, k, l;

  memcpy(tmp, sig, n * sizeof(double));
  n /= 2;
  for (x = 0; x < n; x++) {
    i = 2 * x;
    j = i + 1;
    k = x;
    l = k + n;
    sig[i] = (tmp[k] + tmp[l]) / SDT_SQRT2;
    sig[j] = (tmp[k] - tmp[l]) / SDT_SQRT2;
  }
}

int SDT_isHole(double *x, unsigned int index, unsigned int radius) {
  int i;

  for (i = 1; i <= radius; i++) {
    if (x[index - i] <= x[index]) return 0;
    if (x[index + i] < x[index]) return 0;
  }
  return 1;
}

int SDT_isPeak(double *x, unsigned int index, unsigned int radius) {
  int i;

  for (i = 1; i <= radius; i++) {
    if (x[index - i] >= x[index] || x[index + i] > x[index]) return 0;
  }
  return 1;
}

double SDT_kinetic(double mass, double velocity) {
  return 0.5 * mass * velocity * velocity;
}

double SDT_max(double *x, unsigned int n) {
  double max;
  unsigned int i;

  max = x[0];
  for (i = 1; i < n; i++) {
    if (x[i] > max) {
      max = x[i];
    }
  }
  return max;
}

double SDT_min(double *x, unsigned int n) {
  double min;
  unsigned int i;

  min = x[0];
  for (i = 1; i < n; i++) {
    if (x[i] < min) {
      min = x[i];
    }
  }
  return min;
}

unsigned int SDT_nextPow2(unsigned int u) {
  return 1 << (unsigned int)ceil(log2(u));
}

double SDT_normalize(double x, double min, double max) {
  return (x - min) / (max - min);
}

void SDT_normalizeWindow(double *sig, int n) {
  double sum;
  int i;

  sum = 0.0;
  for (i = 0; i < n; i++) {
    sum += sig[i];
  }
  for (i = 0; i < n; i++) {
    sig[i] /= sum;
  }
}

void SDT_ones(double *sig, int n) {
  int i;

  for (i = 0; i < n; i++) {
    sig[i] = 1.0;
  }
}

double SDT_rank(double *x, int n, int k) {
  int i, j, l, r;
  double a[n], pivot, swap;

  for (i = 0; i < n; i++) {
    a[i] = x[i];
  }

  l = 0;
  r = n - 1;
  while (l < r) {
    pivot = a[k];
    i = l;
    j = r;
    while (1) {
      while (a[i] < pivot) i++;
      while (pivot < a[j]) j--;
      if (i <= j) {
        swap = a[i];
        a[i] = a[j];
        a[j] = swap;
        i++;
        j--;
      }
      if (i > j) break;
    }
    if (j < k) l = i;
    if (k < i) r = j;
  }
  return a[k];
}

void SDT_removeDC(double *sig, int n) {
  double avg;
  int i;

  avg = 0.0;
  for (i = 0; i < n; i++) {
    avg += sig[i];
  }
  avg /= n;
  for (i = 0; i < n; i++) {
    sig[i] -= avg;
  }
}

int SDT_roi(double *sig, int *peaks, int *bounds, int d, int n) {
  int i, j, isPeak, nPeaks;
  double min;

  nPeaks = 0;
  for (i = 0; i < n; i++) {
    isPeak = 1;
    for (j = 1; j <= d; j++) {
      if ((i - j >= 0 && sig[i - j] >= sig[i]) ||
          (i + j < n && sig[i + j] >= sig[i])) {
        isPeak = 0;
        break;
      }
    }
    if (isPeak) {
      peaks[nPeaks] = i;
      nPeaks += 1;
    }
  }
  bounds[0] = 0;
  for (i = 1; i < nPeaks; i++) {
    min = sig[peaks[i - 1]];
    bounds[i] = peaks[i - 1];
    for (j = peaks[i - 1] + 1; j < peaks[i]; j++) {
      if (sig[j] < min) {
        min = sig[j];
        bounds[i] = j;
      }
    }
  }
  bounds[nPeaks] = n;
  return nPeaks;
}

double SDT_samplesInAir(double length) {
  return fmax(0.0, length) / SDT_MACH1 * SDT_sampleRate;
}

double SDT_samplesInAir_inv(double samples) {
  return samples * SDT_MACH1 / SDT_sampleRate;
}

double SDT_scale(double x, double srcMin, double srcMax, double dstMin,
                 double dstMax, double gamma) {
  return pow((x - srcMin) / (srcMax - srcMin), gamma) * (dstMax - dstMin) +
         dstMin;
}

int SDT_signum(double x) {
  int result;

  if (x < 0)
    result = -1;
  else if (x == 0)
    result = 0;
  else
    result = 1;
  return result;
}

void SDT_sinc(double *sig, double f, int n) {
  // Avoid division by zero
  if (fabs(f) < SDT_MICRO) return;
  /* Multiply by PI, but t increases by 2. So, the effective
  ** angular velocity id 2 PI f radians per sample */
  const double w = SDT_PI * f;
  int t, i, j;
  double x;

  t = (n + 1) % 2;  // Even?
  j = n / 2;        // Right midpoint
  i = j - t;        // Left midpoint
  // If odd, skip middle point (t = 0), because x = 1
  t = (t) ? 1 : 2;
  for (; j < n; t += 2, --i, ++j) {
    x = t * w;
    x = sin(x) / x;
    sig[i] *= x;
    sig[j] *= x;
  }
}

double SDT_truePeakPos(double *sig, int peak) {
  double a, b, c;

  a = sig[peak - 1];
  b = sig[peak];
  c = sig[peak + 1];
  return peak + 0.5 * (a - c) / (a - 2.0 * b + c);
}

double SDT_truePeakValue(double *sig, int peak) {
  double a, b, c;

  a = sig[peak - 1];
  b = sig[peak];
  c = sig[peak + 1];
  return b + 0.5 * (0.5 * ((c - a) * (c - a))) / (2 * b - a - c);
}

double SDT_weightedAverage(double *values, double *weights, unsigned int n) {
  double sumValues, sumWeights;
  unsigned int i;

  sumValues = 0;
  sumWeights = 0;
  for (i = 0; i < n; i++) {
    sumValues += values[i] * weights[i];
    sumWeights += weights[i];
  }
  return (sumWeights > SDT_MICRO) ? sumValues / sumWeights : 0.0;
}

double SDT_wrap(double x) {
  x += SDT_PI;
  x = fmod(x, SDT_TWOPI);
  if (x < 0.0) x += SDT_TWOPI;
  return x - SDT_PI;
}

void SDT_zeros(double *sig, int n) {
  memset((void *)sig, 0, sizeof(double) * n);
}

static int _SDT_sprintTime(char *s, size_t n) {
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  return strftime(s, n, "[%Y-%m-%d %H:%M:%S]", tm);
}

int SDT_eprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int r = vfprintf(stderr, fmt, args);
  va_end(args);
  return r;
}

#define N_LOGGERS 5
static int (*_SDT_log_inner[N_LOGGERS])(const char *, ...) = {0, 0, 0, 0, 0};
static int _SDT_log_newlines[N_LOGGERS] = {0, 0, 0, 0, 0};

void SDT_setLogger(int level, int (*print_func)(const char *, ...),
                   int newline) {
  if (level >= 0 && level < N_LOGGERS) {
    _SDT_log_inner[level] = print_func;
    _SDT_log_newlines[level] = newline;
  }
}

int (*SDT_getLogger(int level, int *newline))(const char *, ...) {
  int (*p)(const char *, ...) = 0;
  if (level >= 0 && level < N_LOGGERS) {
    p = _SDT_log_inner[level];
    if (newline) *newline = _SDT_log_newlines[level];
  } else if (newline)
    *newline = 0;
  return (p) ? p : &SDT_eprintf;
}

static int SDT_vsnlog(char *s, size_t n, int newline, int level,
                      const char *file, unsigned int line, const char *func,
                      const char *fmt, va_list arg) {
  int n_chars = 0, i;
  n /= sizeof(char);

  i = _SDT_sprintTime(s + n_chars, sizeof(char) * (n - n_chars));

  if (i >= 0 && (n_chars += i) < n) {
    char *log_level_prefix;
    switch (level) {
      case SDT_LOG_LEVEL_VERBOSE:
        log_level_prefix = "::VERBOSE";
        break;
      case SDT_LOG_LEVEL_DEBUG:
        log_level_prefix = "::DEBUG  ";
        break;
      case SDT_LOG_LEVEL_INFO:
        log_level_prefix = "::INFO   ";
        break;
      case SDT_LOG_LEVEL_WARN:
        log_level_prefix = "::WARN   ";
        break;
      case SDT_LOG_LEVEL_ERROR:
        log_level_prefix = "::ERROR  ";
        break;
      default:
        log_level_prefix = "         ";
        break;
    }
    i = snprintf(s + n_chars, sizeof(char) * (n - n_chars), "%s",
                 log_level_prefix);
  }

  if (i >= 0 && (n_chars += i) < n)
    i = (file && func) ? snprintf(s + n_chars, sizeof(char) * (n - n_chars),
                                  " %s:%d %s() ", file, line, func)
                       : 0;

  if (i >= 0 && (n_chars += i) < n)
    i = vsnprintf(s + n_chars, sizeof(char) * (n - n_chars), fmt, arg);

  // Error
  if (i < 0) return i;
  // Overflow: make sure terminating character is there (and a newline,
  // for good measure)
  if ((n_chars += i) >= n) {
    s[n - 2] = '\n';
    s[n - 1] = 0;
    n_chars = n;
  }
  if (newline) {
    // If logger automatically puts a newline at the end,
    // then remove newline if it is the last character
    int s_len = strlen(s);
    if (s[s_len - 1] == '\n') s[s_len - 1] = 0;
  }
  return n_chars;
}

int SDT_log(int level, const char *file, unsigned int line, const char *func,
            const char *fmt, ...) {
  if (level > SDT_getLogLevelFromEnv()) return 0;
  int newline = 0;
  static char _SDT_logBuffer[MAXSDTSTRING];
  int (*log)(const char *, ...) = SDT_getLogger(level, &newline);
  va_list args;
  va_start(args, fmt);
  SDT_vsnlog(_SDT_logBuffer, sizeof(char) * MAXSDTSTRING, newline, level, file,
             line, func, fmt, args);
  va_end(args);
  return log("%s", _SDT_logBuffer);
}

int SDT_isVerbose() { return SDT_VERBOSE_IF_ELSE(1, 0); }

#define _SDT_LOG_LEVEL_ENV_CASE(LEVEL) \
  if (!strcmp(level_name, #LEVEL)) level = SDT_LOG_LEVEL_##LEVEL;

int SDT_getLogLevelFromEnv() {
  static int level = SDT_LOG_LEVEL_QUIET - 1;
  // Only read the first time
  if (level >= SDT_LOG_LEVEL_QUIET) return level;

  const char *level_name = getenv("SDT_LOG_LEVEL");
  if (level_name) {
    _SDT_LOG_LEVEL_ENV_CASE(QUIET)             //
    else _SDT_LOG_LEVEL_ENV_CASE(ERROR)        //
        else _SDT_LOG_LEVEL_ENV_CASE(WARN)     //
        else _SDT_LOG_LEVEL_ENV_CASE(INFO)     //
        else _SDT_LOG_LEVEL_ENV_CASE(DEBUG)    //
        else _SDT_LOG_LEVEL_ENV_CASE(VERBOSE)  //
  }
  if (level < SDT_LOG_LEVEL_QUIET) {
    level = SDT_LOG_LEVEL_INFO;
    if (level_name) {
      SDT_LOGA(WARN,
               "Unsupported log level name from environment variable: "
               "SDT_LOG_LEVEL=%s\n",
               level_name);
    }
  }
  SDT_LOGA(DEBUG, "SDT_LOG_LEVEL=%d\n", level);
  return level;
}
