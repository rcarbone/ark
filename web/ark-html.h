/*
 * ark-html.h - Definitions to add html support to ark-aware plugins
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *                                 _
 *                       __ _ _ __| | __
 *                      / _` | '__| |/ /
 *                     | (_| | |  |   <
 *                      \__,_|_|  |_|\_\
 *
 * 'ark', is the home for asynchronous libevent-based plugins
 *
 *
 *                  (C) Copyright 2007-2008
 * Valeria Lorenzetti <valeria.lorenzetti /at/ gmail /dot/ com>
 *         Rocco Carbone <rocco /at/ tecsiel /dot/ it>
 *
 * Released under the terms of GNU General Public License
 * at version 3;  see included COPYING file for details
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *
 */


#if !defined(ARK_HTML_H)
# define ARK_HTML_H

/* libevent header file to add basic support for HTTP serving */
#include <evhttp.h>

/* Private header file(s) */
#include "ark.h"
#include "hash.h"
#include "gnode.h"
#include "list.h"


/* Page title */
#define ARKTITLE          "'ark', a safe home for libevent-based plugins"

/* Registered urls */
#define URL_PLUGINS       "/plugins"               /* back to the plugins page                           */
#define URL_PARSE         "/parse"                 /* form parsing page                                  */
#define URL_CSS           "/ark.css"

/* Definitions to embed icons without files in ark's plugins */
#define IMAGE             "/image"
#define URL_PROJECT_NAME  IMAGE"?png=project"      /* An image with the name of the project              */
#define URL_PROJECT_LOGO  IMAGE"?png=logo"         /* Project small logo image                           */
#define URL_FAVICON       IMAGE"?png=favicon"      /* 16x16 favorite icon                                */
#define URL_MAIN_BANNER   IMAGE"?png=banner"       /* Large image (used as banner only for the homepage) */
#define URL_BACKGROUND    IMAGE"?png=background"   /* Background image                                   */
#define URL_BANNER        IMAGE"?png=header"       /* Header image                                       */
#define URL_FOOTER        IMAGE"?png=footer"       /* Footer image                                       */
#define URL_BUTTON        IMAGE"?png=button"       /* Links image                                        */
#define URL_PLG_HOME      IMAGE"?png=plghome"      /* A small icon to link plugin home page              */

#define URL_HOME          "/"PLGNAME
#define URL_HOME1         "/"PLGNAME"/"            /* The same as URL_HOME with a trailing /             */
#define URL_IMAGE         "/"PLGNAME"/image"
#define URL_PLGNAME       URL_IMAGE"?png=plgname"  /* Embedded mascotte image                            */
#define URL_MASCOTTE      URL_IMAGE"?png=mascotte" /* A small animal icon unique for each plugin         */
#define URL_ABOUT         "/"PLGNAME"/about"

/* Main clickable links */
#define LINK_HOME        "Home"
#define LINK_PLUGINS     "Plugins"
#define LINK_ABOUT       "About"

/* Default background color to render html tables */
#define HDCOLOR        "#F4A460"
#define BGCOLOR_0      "#FFFFCC"

/* How to name options without arguments */
#define NONE          "no-argument"

#define KEY_SESSIONID "sid"


/* This is a Virtual File System Item */
typedef struct hash_table vfs;

/* This is an embedded image */
typedef struct
{
  char * name;
  unsigned char * content;
  unsigned size;
} image_t;

/* This is a ChangeLog entry */
typedef struct
{
  char * when;
  char * what;
  char * desc;
} changelog_t;







/* Public function in file html.c */
void set_header (struct evhttp_request * req, char * con_type);
void begin_page (struct evbuffer * buf, char * title, char * imgtitle, char * mascotte);
void begin_mainpage (struct evbuffer * buf, char * title);
void add_link (struct evbuffer * buf, char * href, char * linkname, int active);
void add_login (struct evbuffer * buf, char * href, char * linkname, char * subtitle);
void add_rlink (struct evbuffer * buf, char * href, char * linkname);
void begin_content (struct evbuffer * buf);
void end_page (struct evbuffer * buf, const char * plugname, const char * plugversion, char * appname, time_t now);

void begin_onecol (struct evbuffer * buf);
void end_onecol (struct evbuffer * buf);

void begin_left (struct evbuffer * buf, char * title);
void left (struct evbuffer * buf, char * text);
void end_left (struct evbuffer * buf);

void begin_leftmenu (struct evbuffer * buf, char * title);
void left_item (struct evbuffer * buf, char * href, char * item_name, int active);
void end_leftmenu (struct evbuffer * buf);
void begin_itempage (struct evbuffer * buf, char * title);
void end_itempage (struct evbuffer * buf);

void begin_right (struct evbuffer * buf);
void pretty_version (struct evbuffer * buf, char * mascotte, char * version, char * release, char * url_onclick);
void add_changelog (struct evbuffer * buf, char * title);
void new_log (struct evbuffer * buf, char * title, char * date, char * text);
void end_right (struct evbuffer * buf);

void begin_plginfo (struct evbuffer * buf, char * title);
void string_info (struct evbuffer * buf, char * name, char * value);
void int_info (struct evbuffer * buf, char * name, int value);
void end_plginfo (struct evbuffer * buf);

void arkimage (struct evhttp_request * req, void * embedded);

char * percentage (counter_t partial, counter_t total);
char * counter (counter_t c);
char * htmluptime (struct timeval * started);

/* Public function in file form.c */
void begin_form (struct evbuffer * buf, char * title, char * plug_name);
void add_string (struct evbuffer * buf, char * label, int mandatory, char * param, char * def_value, char * comment);
void add_int (struct evbuffer * buf, char * label, int mandatory, char * param, int def_value, char * comment);
void title_row (struct evbuffer * buf, char * title);
void begin_selection (struct evbuffer * buf, char * label, char * param);
void add_option  (struct evbuffer * buf, char * option, char * value);
void end_selection (struct evbuffer * buf, char * comment);
void add_boolean (struct evbuffer * buf, char * label, char * param, char * def_value, char * comment);
void end_form (struct evbuffer * buf);

/* Public function in file virtual.c */
void vfs_init (vfs * vfs, unsigned int size);
void vfs_insert (vfs * vfs, char * key, char * value);
char * vfs_lookup (vfs * vfs, char * key);
void vfs_free (vfs * vfs, GNode * root);
char * get_basename (char * path);
void vfs_mkfile (vfs * vfs, GNode * node, char * dirname, char * filename, char * value);
void vfs_mkdir (GNode * node, char * parent, char * dirname);
void print_html_tree (struct evbuffer * buf, const char * name, vfs * vfs, GNode * root, unsigned lev);

#endif /* ARK_HTML_H */
