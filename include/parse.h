/*
 * parse.h
 *
 * $Id: parse.h,v 1.2 2000/03/18 05:20:28 bleep Exp $
 */
#ifndef INCLUDED_parse_h
#define INCLUDED_parse_h

struct Client;
struct s_map;

/*
 * Prototypes
 */

extern int parse_client(struct Client *cptr, char *buffer, char *bufend);
extern int parse_server(struct Client *cptr, char *buffer, char *bufend);
extern void initmsgtree(void);

extern int register_mapping(struct s_map *map);
extern int unregister_mapping(struct s_map *map);

#endif /* INCLUDED_parse_h */
