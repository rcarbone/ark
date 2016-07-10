/*
 * ark-form.c - Utilities to handle web forms while configuring ark-aware plugins
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
 *         Rocco Carbone <rocco /at/ tecsiel /dot/ it>
 *
 * Released under the terms of GNU General Public License
 * at version 3;  see included COPYING file for details
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *
 */


/* Operating System header file(s) */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Private header file(s) */
#include "ark.h"
#include "ark-html.h"

/* Larghezza colonne */
#define WIDTH_1      350      /* Name on the first column               */
#define WIDTH_2      100      /* Value on the second column             */
#define WIDTH_3      400      /* Comments on the third column           */

#define HEIGHT       35       /* Height of the row with title/separator */
#define SIZE         20       /* Size textfield                         */

#define ALIGN_TITLE  "center" /* Title alignment                        */
#define ALIGN_COMM   "right"  /* Text alignment                         */


/* Start of the form page */
void begin_form (struct evbuffer * buf, char * title, char * plug_name)
{
  evbuffer_add_printf (buf,
                       "<!-- Begin form -->\n"
                       "<form name=input action=%s method=get>\n"
                       "<table align=center cellpadding=1 cellspacing=0 border=0>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=3>&nbsp;</td>\n"
                       " </tr>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=3 align=center><font face=verdana size=2><b>%s</b></font></td>\n"
                       " </tr>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=3>&nbsp;</td>\n"
                       " </tr>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td align=center><font face=verdana size=2><b>[Name]</b></font></td>\n"
                       "  <td align=center><font face=verdana size=2><b>[Value]</b></font></td>\n"
                       "  <td align=center><font face=verdana size=2><b>[Comments]</b></font></td>\n"
                       " </tr>\n",
                       URL_PARSE, HDCOLOR, HDCOLOR, title, HDCOLOR, HDCOLOR);

  /* Nome del plugin. Informazione necessaria per la load */
  evbuffer_add_printf (buf,
                       " <input type=hidden name=pname value=%s>\n",
                       plug_name);
}


/* End of the form page */
void end_form (struct evbuffer * buf)
{
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=3>&nbsp;</td>\n"
                       " </tr>\n"
                       "</table>\n",
                       HDCOLOR);

  /* Bottone 'Load' e bottone 'Reset' */
  evbuffer_add_printf (buf,
                       "<center>\n"
                       "<br><br>\n"
                       "<input type=submit value=Load>"
                       "&nbsp;&nbsp;&nbsp;\n"
                       "<input type=reset value=Reset>\n"
                       "</form>\n"
                       "</center><br><br><br>\n");
}


/* Aggiunge una riga in cui e' possibile specificare un titolo */
void title_row (struct evbuffer * buf, char * title)
{
  if (title != NULL)
    evbuffer_add_printf (buf,
			 " <tr height=%d bgcolor=%s>\n"
			 "  <td colspan=3 align=%s><font face=verdana size=2>%s</font></td>\n"
			 " </tr>\n",
			 HEIGHT, HDCOLOR, ALIGN_TITLE, title);
  else
    evbuffer_add_printf (buf,
			 " <tr height=%d bgcolor=%s>\n"
			 "  <td colspan=3>&nbsp;</td>\n"
			 " </tr>\n",
			 HEIGHT, HDCOLOR);
}


/*
 * Aggiunge una riga al form, con valore di tipo stringa
 *
 *   Parametri
 *     label:      Nome del campo, visibile prima del textfield nel form html
 *     mandatory:  1 indica 'obbligatorio' e aggiunge '*' dopo la label
 *                 0 indica 'opzionale'
 *     param:      Nome del parametro, visibile nella url dopo il submit
 *     def_value:  valore di default per il campo, se presente viene inserito nel textfield
 *     comment:    commento
 */
void add_string (struct evbuffer * buf, char * label, int mandatory, char * param, char * def_value, char * comment)
{
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=%d align=left><font face=verdana size=2>",
                       BGCOLOR_0, WIDTH_1);

  /* Controllo se il parametro e' obbligatorio */
  if (mandatory == 1)
    evbuffer_add_printf (buf,
                         "   &nbsp;<b>%s *</b></font></td>\n"
                         "  <td width=%d align=left><font face=verdana size=2>",
                         label, WIDTH_2);
  else
    if (mandatory == 0)
      evbuffer_add_printf (buf,
			   "   &nbsp;<b>%s</b></font></td>\n"
			   "  <td width=%d align=left><font face=verdana size=2>",
			   label, WIDTH_2);
    else
      printf("ERRORE\n"); /* TODO gestire */

  /* Controllo se esiste un valore di default */
  if (def_value != NULL)
    evbuffer_add_printf (buf,
                         "   <input type=text name=%s value=%s size=%d></font></td>\n",
			 param, def_value, SIZE);
  else
    evbuffer_add_printf (buf,
                         "   <input type=text name=%s size=%d></font></td>\n",
			 param, SIZE);

  /* Controllo se esiste un commento */
  if (comment != NULL)
    evbuffer_add_printf (buf,
			 " <td width=%d align=%s><font face=verdana size=2>%s</font></td>\n"
                         "</tr>\n",
                         WIDTH_3, ALIGN_COMM, comment);
  else
    evbuffer_add_printf (buf,
			 " <td width=%d align=left><font face=verdana size=2>&nbsp;</font></td>\n"
                         "</tr>\n",
                         WIDTH_3);
}


/*
 * Aggiunge una riga al form, con valore di tipo int
 *
 *   Parametri
 *     label:      Nome del campo, visibile prima del textfield nel form html
 *     mandatory:  1 indica 'obbligatorio' e aggiunge '*' dopo la label
 *                 0 indica 'opzionale'
 *     param:      Nome del parametro, visibile nella url dopo il submit
 *     def_value:  valore di default per il campo, se presente viene inserito nel textfield
 *     comment:    commento
 */
void add_int (struct evbuffer * buf, char * label, int mandatory, char * param, int def_value, char * comment)
{
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=%d align=left><font face=verdana size=2>",
                       BGCOLOR_0, WIDTH_1);

  /* Controllo se il parametro e' obbligatorio */
  if (mandatory == 1)
    evbuffer_add_printf (buf,
                         "   &nbsp;<b>%s *</b></font></td>\n"
                         "  <td width=%d align=left><font face=verdana size=2>",
                         label, WIDTH_2);
  else
    if (mandatory == 0)
      evbuffer_add_printf (buf,
                           "   &nbsp;<b>%s</b></font></td>\n"
                           "  <td width=%d align=left><font face=verdana size=2>",
                           label, WIDTH_2);
    else
      printf("ERRORE\n"); /* TODO gestire */

  /* Valore di default */
  evbuffer_add_printf (buf,
		       "   <input type=text name=%s value=%d size=%d></font></td>\n",
		       param, def_value, SIZE);

  /* Controllo se esiste un commento */
  if (comment != NULL)
    evbuffer_add_printf (buf,
                         " <td width=%d align=%s><font face=verdana size=2>%s</font></td>\n"
                         "</tr>\n",
                         WIDTH_3, ALIGN_COMM, comment);
  else
    evbuffer_add_printf (buf,
                         " <td width=%d align=left><font face=verdana size=2>&nbsp;</font></td>\n"
                         "</tr>\n",
                         WIDTH_3);
}


/* Aggiunge una riga con un 'menu a tendina'.
 *  Utilizzabile quando un parametro puo' assumere solamente
 *  un determinato insieme di valori.
 *
 * - begin_selection      <-- definisce l'inizio del menu
 * - add_option 1     -|
 *   ...               |  <-- opzioni tra le quali scegliere
 * - add_option n     -|
 * - end_selection        <-- definisce la fine del menu
 */

/*
 * Inizio del menu a tendina
 *
 *   Parametri
 *     label:      Nome del campo, visibile prima del menu
 *     param:      Nome del parametro, visibile nella url dopo il submit
 */
void begin_selection (struct evbuffer * buf, char * label, char * param)
{
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=%d align=left><font face=verdana size=2>&nbsp;<b>%s</b></font></td>"
                       "  <td><select name=%s>\n",
                       BGCOLOR_0, WIDTH_1, label, param);
}


/*
 * Aggiunge una opzione nel menu a tendina
 *
 *   Parametri
 *     option:     Nome dell'opzione, visualizzata nel menu
 *     value:      Valore da assegnare a 'param' nel caso sia selezionata questa opzione
 */
void add_option  (struct evbuffer * buf, char * option, char * value)
{
  evbuffer_add_printf (buf,
		       "<option value=%s>%s</option>\n",
		       option, value);
}


/*
 * Fine del menu a tendina
 *
 *   Parametri
 *     comment:      commento
 */
void end_selection (struct evbuffer * buf, char * comment)
{
  evbuffer_add_printf (buf,
                       " </select></td>\n");

  /* Controllo se esiste un commento */
  if (comment != NULL)
    evbuffer_add_printf (buf,
                         " <td width=%d align=%s><font face=verdana size=2>%s</font></td>\n"
                         "</tr>\n",
                         WIDTH_3, ALIGN_COMM, comment);
  else
    evbuffer_add_printf (buf,
                         " <td width=%d align=left><font face=verdana size=2>&nbsp;</font></td>\n"
                         "</tr>\n",
                         WIDTH_3);
}


/*
 * Aggiunge una riga al form, con un 'checkbox'
 *
 *   Parametri
 *     label:      Nome del campo, visibile prima del 'checkbox'
 *     param:      Nome del parametro, visibile nella url dopo il submit
 *     value:      Valore da assegnare al campo se viene selezionato il checkbox
 *     comment:    commento
 */
void add_boolean (struct evbuffer * buf, char * label, char * param, char * value, char * comment)
{
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
		       "  <td width=%d align=left><font face=verdana size=2>&nbsp;<b>%s</b></font></td>"
                       "  <td><input type=checkbox name=\"%s\" value=\"%s\"></td>",
                       BGCOLOR_0, WIDTH_1, label, param, value);

  /* Controllo se esiste un commento */
  if (comment != NULL)
    evbuffer_add_printf (buf,
                         " <td width=%d align=%s><font face=verdana size=2>%s</font></td>\n"
                         "</tr>\n",
                         WIDTH_3, ALIGN_COMM, comment);
  else
    evbuffer_add_printf (buf,
                         " <td width=%d align=left><font face=verdana size=2>&nbsp;</font></td>\n"
                         "</tr>\n",
                         WIDTH_3);
}
