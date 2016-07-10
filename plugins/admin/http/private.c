/*
 * private.c - Load and keep configuration parameters
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



/* Initialize option-time variables to their default values
 * (both -1 and NULL mean absent)
 */
static opttime_t * defaultopt (void)
{
  opttime_t * op = (opttime_t *) calloc (sizeof (opttime_t), 1);
  if (! op)
    return NULL;

  op -> conffile     = NULL;
  op -> logfile      = NULL;
  op -> accessfile   = NULL;

  op -> accounts     = NULL;

  op -> expired_time = -1;

  op -> addrs        = NULL;

  op -> hdcolor      = NULL;
  op -> bgcolors     = NULL;

  return op;
}


/* Initialize configuration variables to their default values
 * (both -1 and NULL mean absent)
 *
 * Each field is set to a reasonable value and could be overridden
 * by the corresponding command line options if specified.
 */
static cfgtime_t * defaultcfg (void)
{
  cfgtime_t * cf = (cfgtime_t *) calloc (sizeof (cfgtime_t), 1);
  if (! cf)
    return NULL;

  cf -> logfile      = NULL;
  cf -> accessfile   = NULL;

  cf -> enable_admin = -1;
  cf -> accounts     = NULL;

  cf -> expired_time = -1;

  cf -> addrs        = NULL;

  cf -> hdcolor      = NULL;
  cf -> bgcolors     = NULL;

  return cf;
}


/* Initialize run-time variables to their default values
 * (both -1 and NULL mean absent)
 *
 * Each field is set to a reasonable value and could be
 * overridden by the corresponding configuration value,
 * if one specified, or command line options.
 */
static runtime_t * defaultrun (char * name)
{
  runtime_t * rc = (runtime_t *) calloc (sizeof (runtime_t), 1);
  if (! rc)
    return NULL;

  gettimeofday (& rc -> boottime, NULL);
  rc -> name         = strdup (name);

  rc -> conffile     = NULL;
  rc -> logfile      = strdup (DEFAULT_LOG_FILE);
  rc -> accessfile   = strdup (DEFAULT_ACCESS_FILE);

  rc -> enable_admin = DEFAULT_ENABLEADMIN;
  rc -> accounts     = argsadd (NULL, DEFAULT_ADMIN);

  rc -> sessionIds   = NULL;
  init_sessiontable (& rc -> sessions, SESSIONS_TABLE_LEN);
  rc -> expired_time = DEFAULT_EXPIREDTIME;

  init_desctable (& rc -> desc, DESC_TABLE_LEN);

  rc -> addrs        = argsadd (NULL, DEFAULT_ADDRESS);

  rc -> pending      = NULL;

  rc -> hdcolor      = strdup (HDCOLOR);
  rc -> bgcolors     = argsblanks (DEFAULT_BGCOLORS);

  /* Log file descriptor */
  if (! strcmp (rc -> logfile, "*"))
    rc -> logfp = NULL;
  else if (! strcmp (rc -> logfile, "-"))
    rc -> logfp = stdout;
  else
    rc -> logfp = fopen (rc -> logfile, "w+");

  /* Access file descriptor */
  if (! strcmp (rc -> accessfile, "*"))
    rc -> accessfp = NULL;
  else if (! strcmp (rc -> accessfile, "-"))
    rc -> accessfp = stdout;
  else
    rc -> accessfp = fopen (rc -> accessfile, "w+");

  return rc;
}


/* Free the memory used to save command line parameters */
static void nomoreopt (opttime_t * op)
{
  if (! op)
    return;

  if (op -> conffile)
    free (op -> conffile);
  if (op -> logfile)
    free (op -> logfile);
  if (op -> accessfile)
    free (op -> accessfile);

  argsfree (op -> accounts);

  argsfree (op -> addrs);

  if (op -> hdcolor)
    free (op -> hdcolor);
  argsfree (op -> bgcolors);

  free (op);
}


/* Free the memory used to save configuration parameters */
static void nomorecfg (cfgtime_t * cf)
{
  if (! cf)
    return;

  if (cf -> logfile)
    free (cf -> logfile);
  if (cf -> accessfile)
    free (cf -> accessfile);

  argsfree (cf -> accounts);

  argsfree (cf -> addrs);

  if (cf -> hdcolor)
    free (cf -> hdcolor);
  argsfree (cf -> bgcolors);

  free (cf);
}


/* Free the memory used to save run-time parameters */
static void nomorerun (runtime_t * rc)
{
  if (! rc)
    return;

  if (rc -> name)
    free (rc -> name);

  if (rc -> conffile)
    free (rc -> conffile);
  if (rc -> logfile)
    free (rc -> logfile);
  if (rc -> accessfile)
    free (rc -> accessfile);

  argsfree (rc -> accounts);

  argsfree (rc -> sessionIds);
  free_sessiontable (& rc -> sessions);

  free_desctable (& rc -> desc);

  argsfree (rc -> addrs);

  pnd_free (rc -> pending);

  if (rc -> hdcolor)
    free (rc -> hdcolor);
  argsfree (rc -> bgcolors);
  if (rc -> logfp && rc -> logfp != stdout)
    fclose (rc -> logfp);
  if (rc -> accessfp && rc -> accessfp != stdout)
    fclose (rc -> accessfp);

  free (rc);
}


/* Fill in the run-time values with values from command line or configuration */
static void override (opttime_t * op, cfgtime_t * cf, runtime_t * rc)
{
  if (! cf)
    return;

  /* Master configuration file */
  if (op -> conffile)
    {
      if (rc -> conffile)
        free (rc -> conffile);
      rc -> conffile = strdup (op -> conffile);
    }

  /* Log file */
  if (op -> logfile)
    {
      if (rc -> logfile)
        free (rc -> logfile);
      rc -> logfile = strdup (op -> logfile);
    }
  else if (cf -> logfile)
    {
      if (rc -> logfile)
        free (rc -> logfile);
      rc -> logfile = strdup (cf -> logfile);
    }

  /* Access file */
  if (op -> accessfile)
    {
      if (rc -> accessfile)
        free (rc -> accessfile);
      rc -> accessfile = strdup (op -> accessfile);
    }
  else if (cf -> accessfile)
    {
      if (rc -> accessfile)
        free (rc -> accessfile);
      rc -> accessfile = strdup (cf -> accessfile);
    }

  /* Enable admin */
  if (cf -> enable_admin != -1)
    rc -> enable_admin = cf -> enable_admin;

  /* Accounts */
  if (op -> accounts)
    argsfree (rc -> accounts),
      rc -> accounts = argsdup (op -> accounts);
  else if (cf -> accounts)
    argsfree (rc -> accounts),
      rc -> accounts = argsdup (cf -> accounts);

  /* Expired time */
  if (op -> expired_time != -1)
    rc -> expired_time = op -> expired_time;
  else if (cf -> expired_time != -1)
    rc -> expired_time = cf -> expired_time;

  /* Local addresses */
  if (op -> addrs)
    argsfree (rc -> addrs),
      rc -> addrs = argsdup (op -> addrs);
  else if (cf -> addrs)
    argsfree (rc -> addrs),
      rc -> addrs = argsdup (cf -> addrs);

  /* Colors for html rendering */
  if (op -> hdcolor)
    free (rc -> hdcolor),
      rc -> hdcolor = strdup (op -> hdcolor);
  else if (cf -> hdcolor)
    free (rc -> hdcolor),
      rc -> hdcolor = strdup (cf -> hdcolor);

  if (op -> bgcolors)
    argsfree (rc -> bgcolors),
      rc -> bgcolors = argsdup (op -> bgcolors);
  else if (cf -> bgcolors)
    argsfree (rc -> bgcolors),
      rc -> bgcolors = argsdup (cf -> bgcolors);
}


/* Slurp in the configuration file */
static int loadconfig (const char * section, char * conffile, cfgtime_t * cf)
{
  FILE * in;

  /* Variables in the [section] passed by the 'section' variable */
  cfgList * list = NULL;
  cfgList * item;

  if (! conffile)
    return -1;

  /* Check for existence and readability */
  in = ! strcmp (conffile, "-") ? fdopen (0, "r") : fopen (conffile, "r");
  if (! in)
    {
      printf ("%s [%s]: Error: file '%s' does not exist or it is not readable\n", progname, name, conffile);
      return -1;
    }
  fclose (in);

  /* Fetch all the configuration variables from the file */
  fetchVarFromCfgFile (conffile,       LOGFILE,     & cf -> logfile,      CFG_STRING,      CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile,       ACCESSFILE,  & cf -> accessfile,   CFG_STRING,      CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile,       ENABLEADMIN, & cf -> enable_admin, CFG_BOOL,        CFG_INI, 0, section);
  list = NULL;
  if (! fetchVarFromCfgFile (conffile, ACCOUNTS,    & list,               CFG_STRING_LIST, CFG_INI, 0, section))
    for (item = list; item; item = item -> next)
      cf -> accounts = argsadd (cf -> accounts, item -> str);
  fetchVarFromCfgFile (conffile,       EXPIREDTIME, & cf -> expired_time, CFG_INT,         CFG_INI, 0, section);
  list = NULL;
  if (! fetchVarFromCfgFile (conffile, LISTEN,      & list,               CFG_STRING_LIST, CFG_INI, 0, section))
    for (item = list; item; item = item -> next)
      cf -> addrs = argsadd (cf -> addrs, item -> str);
  fetchVarFromCfgFile (conffile,       HD_COLOR,    & cf -> hdcolor,      CFG_STRING,      CFG_INI, 0, section);
  list = NULL;
  if (! fetchVarFromCfgFile (conffile, BGCOLOR,     & list,               CFG_STRING_LIST, CFG_INI, 0, section))
    for (item = list; item; item = item -> next)
      cf -> bgcolors = argsadd (cf -> bgcolors, item -> str);

  return 0;
}


static int login_userok (char * user)
{
  return ! user || ! * user ||  strlen (user) > MAX_LOGINUSER_LEN ? 0 : 1;
}


static int login_pwdok (char * pwd)
{
  return ! pwd || ! * pwd || strlen (pwd) > MAX_LOGINPWD_LEN ? 0 : 1;
}


static int accountok (char ** accounts, char * user, char * pwd)
{
  char myaccount [256];

  if (! login_userok (user) || ! login_pwdok (pwd))
    return 0;

  sprintf (myaccount, "%s:%s", user, pwd);

  while (accounts && * accounts)
    if (! strcmp (* accounts ++, myaccount))
      return 1;

  return 0;
}

