#ifndef FLEXARRAY_H_
#define FLEXARRAY_H_

typedef struct flexarrayrec *flexarray;
typedef struct posting_rec posting;

extern flexarray flexarray_new();
extern void flexarray_append(flexarray f, long id);
extern void flexarray_print(flexarray f);

extern long flexarray_get_last_id(flexarray f);
extern void flexarray_updatecount(flexarray f);

#endif
