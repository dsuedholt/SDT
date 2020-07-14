#include "SDTJSON.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

//-------------------------------------------------------------------------------------//

json_value *SDTResonator_toJSON(SDTResonator *x) {
  json_value *obj = json_object_new(0);
  json_value *f = json_array_new(0);
  json_value *d = json_array_new(0);
  json_value *w = json_array_new(0);
  json_value *g = json_array_new(0);

  for (int n = 0; n < SDTResonator_getNModes(x) ; ++n) {
    json_array_push(f, json_double_new(SDTResonator_getFrequency(x, n)));
    json_array_push(d, json_double_new(SDTResonator_getDecay(x, n)));
    json_array_push(w, json_double_new(SDTResonator_getWeight(x, n)));
  }

  for (int p = 0 ; p < SDTResonator_getNPickups(x); ++p) {
    json_value *gp = json_array_new(0);
    for (int n = 0 ; n < SDTResonator_getNModes(x); ++n)
      json_array_push(gp, json_double_new(SDTResonator_getGain(x, p, n)));
    json_array_push(g, gp);
  }

  json_object_push(obj, "nModes", json_integer_new(SDTResonator_getNModes(x)));
  json_object_push(obj, "nPickups", json_integer_new(SDTResonator_getNPickups(x)));
  json_object_push(obj, "activeModes", json_integer_new(SDTResonator_getActiveModes(x)));
  json_object_push(obj, "fragmentSize", json_double_new(SDTResonator_getFragmentSize(x)));
  json_object_push(obj, "freqs", f);
  json_object_push(obj, "decays", d);
  json_object_push(obj, "weights", w);
  json_object_push(obj, "gains", g);

  return obj;
}

SDTResonator *SDTResonator_fromJSON(const json_value *x) {
  if (!x || x->type != json_object)
    return 0;
  
  const json_value *v;
  unsigned int nModes, nPickups, activeModes;

  v = json_object_get_by_key(x, "nModes");
  nModes = (unsigned int) (v && (v->type == json_integer))? v->u.integer : 0;

  v = json_object_get_by_key(x, "nPickups");
  nPickups = (unsigned int) (v && (v->type == json_integer))? v->u.integer : 0;

  v = json_object_get_by_key(x, "activeModes");
  activeModes = (unsigned int) (v && (v->type == json_integer))? v->u.integer : 0;

  v = json_object_get_by_key(x, "fragmentSize");
  double fragmentSize = (v && (v->type == json_double))? v->u.dbl : 1;

  SDTResonator *res = SDTResonator_new(nModes, nPickups);
  SDTResonator_setActiveModes(res, activeModes);
  SDTResonator_setFragmentSize(res, fragmentSize);
  
  const json_value *f = json_object_get_by_key(x, "freqs");
  const json_value *d = json_object_get_by_key(x, "decays");
  const json_value *w = json_object_get_by_key(x, "weights");
  for (unsigned int m = 0 ; m < SDTResonator_getNModes(res) ; ++m) {
    SDTResonator_setFrequency(res, m, json_array_get_number(f, m, 0));
    SDTResonator_setDecay(res, m, json_array_get_number(d, m, 0));
    SDTResonator_setWeight(res, m, json_array_get_number(w, m, 0));
  }

  const json_value *g = json_object_get_by_key(x, "gains");
  if (g && g->type == json_array)
    for (unsigned int p = 0 ; (p < g->u.array.length) && (p < SDTResonator_getNPickups(res)) ; ++p) {
      const json_value *gp = g->u.array.values[p];
      if (gp && gp->type == json_array)
        for (unsigned int m = 0 ; m < SDTResonator_getNModes(res) ; ++m)
          SDTResonator_setGain(res, p, m, json_array_get_number(gp, m, 0));
    }


  return res;
}

//-------------------------------------------------------------------------------------//

int can_write_file(const char *fpath) {
  char *s = malloc(sizeof(char) * (strlen(fpath) + 1));
  strcpy(s, fpath);
  struct stat buf;

  return strlen(s) && (                                  // file name must be non-empty and either
    ((stat(s, &buf) == -1) && !access(dirname(s), W_OK)) // - the file does not exist and it is in a writable directory
    ||
    (S_ISREG(buf.st_mode) && !access(s, W_OK))           // - the file is a regular file and it is writable
  );
}

int can_read_file(const char *fpath) {
  return !access(fpath, R_OK);
}

long long file_size(const char *fpath) {
  char *s = malloc(sizeof(char) * (strlen(fpath) + 1));
  strcpy(s, fpath);
  struct stat buf;
  if (stat(s, &buf) == -1)
    return -1;

  return (long long) buf.st_size;
}

json_value *json_read(const char *fpath) {
  json_value *v = 0;
  int N = 0;
  FILE *fp;

  fp = fopen(fpath, "r");
  if(fp) {
    // Get size
    fseek(fp, 0, SEEK_END);
    N = ftell(fp);
    rewind(fp);

    char *s = (char *) malloc(sizeof(char) * N);
    N = fread(s, 1, N, fp);
    fclose(fp);

    v = json_parse((json_char *) s, strlen(s));
    free(s);
  }
  return v;
}

int json_dump(json_value *x, const char *fpath) {
  char *s = malloc(json_measure(x));
  json_serialize(s, x);

  if (!can_write_file(fpath))
    return 1;

  FILE *f = fopen(fpath, "w");
  fprintf(f, "%s", s);
  fclose(f);

  free(s);
  return 0;
}

const json_value *json_object_get_by_key(const json_value *x, const char *key) {
  if (!x || (x->type != json_object))
    return 0;
  for (unsigned int i = 0 ; i < x->u.object.length ; ++i)
    if (!strcmp(key, x->u.object.values[i].name))
      return x->u.object.values[i].value;
  return 0;
}

double json_array_get_number(const json_value *x, unsigned int idx, double dflt) {
  if (x && (x->type == json_array) && (idx < x->u.array.length)) {
    if (x->u.array.values[idx]->type == json_integer)
      return (double) x->u.array.values[idx]->u.integer;
    else if (x->u.array.values[idx]->type == json_double)
      return x->u.array.values[idx]->u.dbl;
  }
  return dflt;
}