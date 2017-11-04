/*
    xigor - little generic flagging program for Xorg systray
    Copyright (C) 2017  Thomas Thiel

    This software is released under the X11/MIT license:

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, and/or sell copies of the Software, and to permit persons
    to whom the Software is furnished to do so, provided that the above
    copyright notice(s) and this permission notice appear in all copies of
    the Software and that both the above copyright notice(s) and this
    permission notice appear in supporting documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
    OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
    HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
    INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
    FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
    NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
    WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    The included XPM files are converted from the original xbm files
    included with X11 and are subject to the X11 license and their respective
    creators, whose names I did not find. I included these files
    in the hope that I may do so in the spirit of FOSS.

*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#include "flagup.xpm"
#include "flagdown.xpm"
#include "noletters.xpm"

// some defaultvalues for all parameters
#define D_MONITORFILE ".xigor-trigger"
#define D_DELAY 30

// config variables
unsigned int delay;
char *monitorfile;
char *activeicon;
char *systrayicon;
char *erroricon;
unsigned int debug;

// systray icon reference
GtkStatusIcon * st_icon;

// storage for modifaction time of trigger file
time_t          mod_time;

// Pixbuffers for Icon storage
GdkPixbuf *active_pixbuf;
GdkPixbuf *error_pixbuf;
GdkPixbuf *systray_pixbuf;

// helper, returns modification time of trigger file
// or display error icon in systray
time_t getModTimeMonitorFile() {
  struct stat fileStat;
  if(stat(monitorfile,&fileStat) >= 0) {
    return fileStat.st_mtime;
  } else gtk_status_icon_set_from_pixbuf(st_icon,error_pixbuf); 
};

// Systray icon click callback handler
void on_icon_clicked (GtkStatusIcon *status_icon, GdkEventButton *event, gpointer user_data)
{
  // get pressed mouse button; if left button was used,
  // reset the icon to the default one. If any other button was pressed,
  // remove the systray icon and close the application.
 if (event->button==1) {
   gtk_status_icon_set_from_pixbuf(st_icon,systray_pixbuf);
   mod_time=getModTimeMonitorFile();
   if (debug) fprintf(stdout,"on_icon_clicked(): mod_time reset. (%d)\n",mod_time);
 } else gtk_main_quit();
}

// callback handler for delay. handles the checking of
// the trigger file for changes and the update of the
// icon if a change was detected.
static gboolean on_timeout (gpointer userdata)
{
  if (mod_time==0) {
    mod_time=getModTimeMonitorFile();
    gtk_status_icon_set_from_pixbuf(st_icon,systray_pixbuf);
    if (debug) fprintf(stdout,"on_timeout(): mod_time initialized (%d)\n",mod_time);
  } else {
    if (mod_time<getModTimeMonitorFile()) {
      gtk_status_icon_set_from_pixbuf(st_icon,active_pixbuf);
    if (debug) fprintf(stdout,"on_timeout(): mod file triggered (%d<%d)\n",mod_time,getModTimeMonitorFile());
  } else {
    if (debug) fprintf(stdout,"on_timeout(): timeout reached, no trigger (%d>=%d)\n",mod_time,getModTimeMonitorFile());
    };
  };
  return TRUE;
}


// prints usage to stderr
void usage() {
  fprintf(stderr,"usage: xigor [options]\n");
  fprintf(stderr,"\t-h\t\tprint this usage and exit\n");
  fprintf(stderr,"\t-d\t\tdelay between checks for changes of the trigger file\n");
  fprintf(stderr,"\t-D\t\tdo not detach and print some debug output to stdout\n");
  fprintf(stderr,"\t-m\t\tpath to the monitorfile (default: ~/.xigor-trigger)\n");
  fprintf(stderr,"\t-s\t\tpath to the normal systray icon file\n");
  fprintf(stderr,"\t-a\t\tpath to the flagged systray icon file\n");
  fprintf(stderr,"\t-e\t\tpath to the error systray icon file\n\n");
  fprintf(stderr,"spawns a flagging icon that displays the icon specified by -s in your desktop environments systray area. This icon will be changed into the one specified by -a, as soon as the file specified by -m is changed/touched. The icon specified by -e is used if the monitor file is not existing or cannot be examined by stat(). Detaches immediately after start and outputs the PID of the detached process on sdtdout before doing so.\n\n");
  fprintf(stderr,"If you omit the icon files, the original X11 pixmaps that xbiff used are shown.\n\n");
  fprintf(stderr,"Copyright (C) 2017  Thomas Thiel\nxigor comes with ABSOLUTELY NO WARRANTY; This is free software, and you are welcome to redistribute it within the terms of the X11/MIT license.\n\n");
};

// main func
int main (int argc, char *argv[])
{

  // prefill config with defaults
	delay				= D_DELAY;
 
  // get the home directory of the user
  const char *home=getenv("HOME");

  // build the default path for the trigger file
  monitorfile = (char*) malloc(((strlen(D_MONITORFILE)+strlen(home))+2)*sizeof(char));
  strcpy(monitorfile, home);
  strcat(monitorfile, "/");
  strcat(monitorfile, D_MONITORFILE);

  debug=FALSE;

  int index;
  int c;

  opterr = 0;

  // parse command line switches
  while ((c = getopt (argc, argv, "Dhd:m:a:s:e:")) != -1)
	switch (c)
		{
		case 'D':
			debug=TRUE;
			break;
		case 'd':
			delay = strtoul(optarg,NULL,10);
			break;
		case 'm':
			monitorfile = optarg;
			break;
		case 'a':
			activeicon = optarg;
			break;
		case 's':
			systrayicon = optarg;
			break;
		case 'e':
			erroricon = optarg;
			break;
		case 'h':
		  usage();
		  exit(0);
		  break;
		case '?':
			if (optopt == 'd')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (optopt == 'm')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (optopt == 'a')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (optopt == 's')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (optopt == 'e')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,
									"Unknown option character `\\x%x'.\n",
									optopt);
			usage();
			return 1;
      default:
        usage();
        abort ();
	}

  // dump config if in debug mode
  if (debug) {
    fprintf(stdout, "options parsed. Config:\n");
    fprintf(stdout, "Delay:       %i \n", delay);
    fprintf(stdout, "Monitorfile: %s \n", monitorfile);
    fprintf(stdout, "Activeicon:  %s \n", activeicon);
    fprintf(stdout, "Systrayicon: %s \n", systrayicon);
    fprintf(stdout, "Erroricon:   %s \n", erroricon);
    fprintf(stdout, "Debug:       %i \n", debug);
  }

  // check for icon files and bail out if anyone is missing
  if (activeicon!=NULL) {
    if(access( activeicon, F_OK ) != 0 ) {
      fprintf(stderr, "Activeicon file not found: %s\n", activeicon);
      exit(150);
    } else {
      active_pixbuf = gdk_pixbuf_new_from_file(activeicon,NULL);
    };
  } else active_pixbuf = gdk_pixbuf_new_from_xpm_data((const char**)flagup_xpm);

  if (systrayicon!=NULL) {
    if( access( systrayicon, F_OK ) != 0 ) {
      fprintf(stderr, "Systrayicon file not found: %s\n", systrayicon);
      exit(150);
    } else {
      systray_pixbuf = gdk_pixbuf_new_from_file(systrayicon,NULL);
    };
  } else systray_pixbuf = gdk_pixbuf_new_from_xpm_data((const char**)flagdown_xpm);

  if (erroricon!=NULL) {
    if( access( erroricon, F_OK ) != 0 ) {
      fprintf(stderr, "Erroricon file not found: %s\n", erroricon);
      exit(150);
    } else {
      error_pixbuf = gdk_pixbuf_new_from_file(erroricon,NULL);
    };
  } else error_pixbuf = gdk_pixbuf_new_from_xpm_data((const char**)noletters_xpm);

  // init modification time of trigger file.
  mod_time=0;

  /* GTK+ initialisieren */
  gtk_init (&argc, &argv);

  // prepare systray icon and its connections
  st_icon = gtk_status_icon_new_from_pixbuf (systray_pixbuf);
  g_signal_connect (st_icon, "button-press-event", G_CALLBACK (on_icon_clicked), NULL);

  // prepare calling of our delay-expiration-handler
  g_timeout_add_seconds(delay, on_timeout, NULL);


  // fork and detach everything
  if (!debug) {
    int pid = fork();
    /* An error occurred */
    if (pid < 0)
        exit(255);
    /* Success: Let the parent terminate */
    if (pid > 0) {
        fprintf(stdout,"%d\n",pid);
        free(monitorfile);
        exit(0);
    };
  }

  // close std(in|out|err), we do not need them anymore...
  close(0);
  close(1);
  close(2);

  // start gtk event loop
  gtk_main();

  // clean up and wave goodbye
  free(monitorfile);
  return 0;
}

