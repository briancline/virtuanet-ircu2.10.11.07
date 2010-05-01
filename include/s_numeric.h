/*
 * s_numeric.h
 *
 * $Id: s_numeric.h,v 1.2 2000/03/18 05:20:28 bleep Exp $
 */
#ifndef INCLUDED_s_numeric_h
#define INCLUDED_s_numeric_h

struct Client;

/*
 * Prototypes
 */

extern int do_numeric(int numeric, int nnn, struct Client *cptr, struct Client *sptr,
    int parc, char *parv[]);

#endif /* INCLUDED_s_numeric_h */
