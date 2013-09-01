//
//  alder_kseq.c
//  alder-adapter
//
//  Created by Sang Chul Choi on 8/30/13.
//  Copyright (c) 2013 Sang Chul Choi. All rights reserved.
//

#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef kroundup32
#define kroundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

typedef struct __kstring_t
{
    size_t l, m;
    char *s;
} kstring_t;


typedef struct __kstream_t
{
    char *buf;
    int begin, end, is_eof;
    gzFile f;
} kstream_t;

static inline kstream_t *
ks_init (gzFile f)
{
    kstream_t *ks = (kstream_t *) calloc (1, sizeof (kstream_t));
    ks->f = f;
    ks->buf = (char *) malloc (4096);
    return ks;
}

static inline void
ks_destroy (kstream_t * ks)
{
    if (ks)
    {
        free (ks->buf);
        free (ks);
    }
}

static inline int
ks_getc (kstream_t * ks)
{
    if (ks->is_eof && ks->begin >= ks->end)
        return -1;
    if (ks->begin >= ks->end)
    {
        ks->begin = 0;
        ks->end = gzread (ks->f, ks->buf, 4096);
        if (ks->end < 4096)
            ks->is_eof = 1;
        if (ks->end == 0)
            return -1;
    }
    return (int) ks->buf[ks->begin++];
}

static int
ks_getuntil (kstream_t * ks, int delimiter, kstring_t * str, int *dret)
{
    if (dret)
        *dret = 0;
    str->l = 0;
    if (ks->begin >= ks->end && ks->is_eof)
        return -1;
    for (;;)
    {
        int i;
        if (ks->begin >= ks->end)
        {
            if (!ks->is_eof)
            {
                ks->begin = 0;
                ks->end = gzread (ks->f, ks->buf, 4096);
                if (ks->end < 4096)
                    ks->is_eof = 1;
                if (ks->end == 0)
                    break;
            }
            else
                break;
        }
        if (delimiter > 1)
        {
            for (i = ks->begin; i < ks->end; ++i)
                if (ks->buf[i] == delimiter)
                    break;
        }
        else if (delimiter == 0)
        {
            for (i = ks->begin; i < ks->end; ++i)
                if (isspace (ks->buf[i]))
                    break;
        }
        else if (delimiter == 1)
        {
            for (i = ks->begin; i < ks->end; ++i)
                if (isspace (ks->buf[i]) && ks->buf[i] != ' ')
                    break;
        }
        else
            i = 0;
        if (str->m - str->l < i - ks->begin + 1)
        {
            str->m = str->l + (i - ks->begin) + 1;
            kroundup32(str->m);
            str->s = (char *) realloc (str->s, str->m);
        }
        memcpy(str->s + str->l, ks->buf + ks->begin, i - ks->begin);
        str->l = str->l + (i - ks->begin);
        ks->begin = i + 1;
        if (i < ks->end)
        {
            if (dret)
                *dret = ks->buf[i];
            break;
        }
    }
    if (str->l == 0)
    {
        str->m = 1;
        str->s = (char *) calloc (1, 1);
    }
    str->s[str->l] = '\0';
    return (int)str->l;
}

typedef struct
{
    kstring_t name, comment, seq, qual;
    int last_char;
    kstream_t *f;
} kseq_t;

static inline kseq_t *
kseq_init (gzFile fd)
{
    kseq_t *s = (kseq_t *) calloc (1, sizeof (kseq_t));
    s->f = ks_init (fd);
    return s;
}

static inline void
kseq_rewind (kseq_t * ks)
{
    ks->last_char = 0;
    ks->f->is_eof = ks->f->begin = ks->f->end = 0;
} static inline void

kseq_destroy (kseq_t * ks)
{
    if (!ks)
        return;
    free (ks->name.s);
    free (ks->comment.s);
    free (ks->seq.s);
    free (ks->qual.s);
    ks_destroy (ks->f);
    free (ks);
}

static int
kseq_read (kseq_t * seq)
{
    int c;
    kstream_t *ks = seq->f;
    if (seq->last_char == 0)
    {
        while ((c = ks_getc (ks)) != -1 && c != '>' && c != '@');
        if (c == -1)
            return -1;
        seq->last_char = c;
    }
    seq->comment.l = seq->seq.l = seq->qual.l = 0;
    if (ks_getuntil (ks, 0, &seq->name, &c) < 0)
        return -1;
    if (c != '\n')
        ks_getuntil (ks, '\n', &seq->comment, 0);
    while ((c = ks_getc (ks)) != -1 && c != '>' && c != '+' && c != '@')
    {
        if (isgraph (c))
        {
            if (seq->seq.l + 1 >= seq->seq.m)
            {
                seq->seq.m = seq->seq.l + 2;
                (--(seq->seq.m), (seq->seq.m) |=
                 (seq->seq.m) >> 1, (seq->seq.m) |=
                 (seq->seq.m) >> 2, (seq->seq.m) |=
                 (seq->seq.m) >> 4, (seq->seq.m) |=
                 (seq->seq.m) >> 8, (seq->seq.m) |=
                 (seq->seq.m) >> 16, ++(seq->seq.m));
                seq->seq.s = (char *) realloc (seq->seq.s, seq->seq.m);
            }
            seq->seq.s[seq->seq.l++] = (char) c;
        }
    } if (c == '>' || c == '@')
        seq->last_char = c;
    seq->seq.s[seq->seq.l] = 0;
    if (c != '+')
        return seq->seq.l;
    if (seq->qual.m < seq->seq.m)
    {
        seq->qual.m = seq->seq.m;
        seq->qual.s = (char *) realloc (seq->qual.s, seq->qual.m);
    }
    while ((c = ks_getc (ks)) != -1 && c != '\n');
    if (c == -1)
        return -2;
    while ((c = ks_getc (ks)) != -1 && seq->qual.l < seq->seq.l)
        if (c >= 33 && c <= 127)
            seq->qual.s[seq->qual.l++] = (unsigned char) c;
    seq->qual.s[seq->qual.l] = 0;
    seq->last_char = 0;
    if (seq->seq.l != seq->qual.l)
        return -2;
    return seq->seq.l;
}
