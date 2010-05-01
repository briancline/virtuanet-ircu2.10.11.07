/*
 * random.h
 *
 * $Id: random.h,v 1.3 2001/01/02 21:47:30 kev Exp $
 */
#ifndef INCLUDED_random_h
#define INCLUDED_random_h

struct Client;

/*
 * Proto types
 */

extern int random_seed_set(struct Client* from, const char* const* fields,
			   int count);

extern unsigned int ircrandom(void);

#endif /* INCLUDED_random_h */
