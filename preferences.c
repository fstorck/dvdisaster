/*  dvdisaster: Additional error correction for optical media.
 *  Copyright (C) 2004-2009 Carsten Gnoerlich.
 *  Project home page: http://www.dvdisaster.com
 *  Email: carsten@dvdisaster.com  -or-  cgnoerlich@fsfe.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA,
 *  or direct your browser at http://www.gnu.org.
 */

#include "dvdisaster.h"

extern int CurrentMediumSize(int);  /* from scsi-layer.h */

/***
 *** debugging workaround
 ***/

#if 0
#define gtk_widget_set_sensitive(widget, state) {  printf("%s, line %d - gtk_widget_set_sensitive()\n", __FILE__, __LINE__); gtk_widget_set_sensitive(widget, state); }
#endif

/***
 *** Protected widget mutators
 ***
 * We might have some race conditions where a callback tries to change
 * a not yet initialized widget.
 */

static void activate_toggle_button(GtkToggleButton *toggle, int state)
{  if(toggle)
      gtk_toggle_button_set_active(toggle, state);
}

static void set_widget_sensitive(GtkWidget *widget, int state)
{  if(widget)
      gtk_widget_set_sensitive(widget, state);
}

static void set_entry_text(GtkEntry *entry, char *text)
{  if(entry)
      gtk_entry_set_text(entry, text);
}

/***
 *** Local data structs
 ***/

/* non linear scale housekeeping */

typedef struct
{  GtkWidget *label;  /* for help system linkage */
   LabelWithOnlineHelp *lwoh;   
   int action;
   int *values;
   char *format;
   struct _prefs_context *pc;
} non_linear_info;

/* color button housekeeping */

typedef struct 
{  GtkWidget *button;
   GtkWidget *dialog;
   GtkWidget *frame;
   GtkWidget *image;
   GdkColor  *color;
   GdkPixmap *pixmap;
   GdkGC     *gc;
   int userData;
} color_button_info;

/***
 *** Preferences window housekeeping
 ***/

typedef struct _prefs_context
{  char *formatLinear;
   char *formatAdaptive;

   GPtrArray *helpPages;   /* The online help frame structures */

  /* Widgets for changing preferences settings. The are two copies (A and B) 
     of each; one for the standard dialog and one embedded in the online help. */

   GtkWidget *radioDriveA, *radioDriveB;
   GtkWidget *radioISOA, *radioISOB;
   GtkWidget *radioECCA, *radioECCB;
   GtkWidget *suffixA, *suffixB;
   GtkWidget *splitA, *splitB;
   GtkWidget *radioLinearA, *radioLinearB;
   GtkWidget *radioAdaptiveA, *radioAdaptiveB;
   GtkWidget *minAttemptsScaleA, *minAttemptsScaleB;
   GtkWidget *maxAttemptsScaleA, *maxAttemptsScaleB;
   GtkWidget *readMediumA, *readMediumB;
   GtkWidget *cacheDefectiveA, *cacheDefectiveB;
   GtkWidget *cacheDefectiveDirA, *cacheDefectiveDirB;
   GtkWidget *cacheDefectiveChooser;
   GtkWidget *cacheDefectivePrefixA, *cacheDefectivePrefixB;
   GtkWidget *rangeToggleA, *rangeToggleB;
   GtkWidget *rangeSpin1A, *rangeSpin1B;
   GtkWidget *rangeSpin2A, *rangeSpin2B;
   GtkWidget *rawButtonA, *rawButtonB;
   GtkWidget *jumpScaleA, *jumpScaleB;
   GtkWidget *daoButtonA, *daoButtonB;
   GtkWidget *dsmButtonA, *dsmButtonB;
   GtkWidget *byteEntryA, *byteEntryB;
   GtkWidget *byteCheckA, *byteCheckB;
   GtkWidget *spinUpA, *spinUpB;
   GtkWidget *internalAttemptsA, *internalAttemptsB;
   GtkWidget *radioRawMode20A, *radioRawMode20B;
   GtkWidget *radioRawMode21A, *radioRawMode21B;
   GtkWidget *radioRawModeOtherA, *radioRawModeOtherB;
   GtkWidget *rawModeValueA, *rawModeValueB;
   GtkWidget *fatalSenseA, *fatalSenseB;
   GtkWidget *ejectA, *ejectB;
   GtkWidget *readAndCreateButtonA, *readAndCreateButtonB;
   GtkWidget *unlinkImageButtonA, *unlinkImageButtonB;
   GtkWidget *mainNotebook;
   GtkWidget *methodChooserA,*methodChooserB;
   GtkWidget *methodNotebook;
   GtkWidget *cancelOKA, *cancelOKB;
   GtkWidget *verboseA, *verboseB;
   GtkWidget *logFileA, *logFileB;
   GtkWidget *logFilePathA, *logFilePathB;
   GtkWidget *logFileChooser;

   color_button_info *redA, *redB;
   color_button_info *yellowA, *yellowB;
   color_button_info *greenA, *greenB;
   color_button_info *blueA, *blueB;
   color_button_info *whiteA, *whiteB;
   color_button_info *darkA, *darkB;
   color_button_info *redTextA, *redTextB;
   color_button_info *greenTextA, *greenTextB;
   color_button_info *barColorA, *barColorB;
   color_button_info *logColorA, *logColorB;
   color_button_info *curveColorA, *curveColorB;

   non_linear_info *jumpScaleInfoA, *jumpScaleInfoB;
   LabelWithOnlineHelp *jumpScaleLwoh;

   non_linear_info *minAttemptsScaleInfoA, *minAttemptsScaleInfoB;
   non_linear_info *maxAttemptsScaleInfoA, *maxAttemptsScaleInfoB;
   LabelWithOnlineHelp *minAttemptsScaleLwoh, *maxAttemptsScaleLwoh;
} prefs_context;

void FreePreferences(void *context)
{  prefs_context *pc = (prefs_context*)context;
   int i;

   for(i=0; i<pc->helpPages->len; i++)
   {  LabelWithOnlineHelp *lwoh = g_ptr_array_index(pc->helpPages,i);

      FreeLabelWithOnlineHelp(lwoh);
   }
   g_ptr_array_free(pc->helpPages, FALSE);

   if(pc->formatLinear) g_free(pc->formatLinear);
   if(pc->formatAdaptive) g_free(pc->formatAdaptive);

   if(pc->jumpScaleInfoA) g_free(pc->jumpScaleInfoA);
   if(pc->jumpScaleInfoB) g_free(pc->jumpScaleInfoB);
   if(pc->minAttemptsScaleInfoA->format) g_free(pc->minAttemptsScaleInfoA->format);
   if(pc->minAttemptsScaleInfoB->format) g_free(pc->minAttemptsScaleInfoB->format);
   if(pc->minAttemptsScaleInfoA) g_free(pc->minAttemptsScaleInfoA);
   if(pc->minAttemptsScaleInfoB) g_free(pc->minAttemptsScaleInfoB);
   if(pc->maxAttemptsScaleInfoA->format) g_free(pc->maxAttemptsScaleInfoA->format);
   if(pc->maxAttemptsScaleInfoB->format) g_free(pc->maxAttemptsScaleInfoB->format);
   if(pc->maxAttemptsScaleInfoA) g_free(pc->maxAttemptsScaleInfoA);
   if(pc->maxAttemptsScaleInfoB) g_free(pc->maxAttemptsScaleInfoB);

   if(pc->redA) g_free(pc->redA);
   if(pc->redB) g_free(pc->redB);
   if(pc->yellowA) g_free(pc->yellowA);
   if(pc->yellowB) g_free(pc->yellowB);
   if(pc->greenA) g_free(pc->greenA);
   if(pc->greenB) g_free(pc->greenB);
   if(pc->blueA) g_free(pc->blueA);
   if(pc->blueB) g_free(pc->blueB);
   if(pc->whiteA) g_free(pc->whiteA);
   if(pc->whiteB) g_free(pc->whiteB);
   if(pc->darkA) g_free(pc->darkA);
   if(pc->darkB) g_free(pc->darkB);
   if(pc->redTextA) g_free(pc->redTextA);
   if(pc->redTextB) g_free(pc->redTextB);
   if(pc->greenTextA) g_free(pc->greenTextA);
   if(pc->greenTextB) g_free(pc->greenTextB);
   if(pc->barColorA) g_free(pc->barColorA);
   if(pc->barColorB) g_free(pc->barColorB);
   if(pc->logColorA) g_free(pc->logColorA);
   if(pc->logColorB) g_free(pc->logColorB);
   if(pc->curveColorA) g_free(pc->curveColorA);
   if(pc->curveColorB) g_free(pc->curveColorB);

   g_free(pc);
}

static gboolean delete_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
   FreePreferences(Closure->prefsContext);

   Closure->prefsWindow = NULL;
   Closure->prefsContext = NULL;
   return FALSE;
}

void HidePreferences(void)
{  prefs_context *pc = (prefs_context*)Closure->prefsContext;
   Method *method;
   const char *value1, *value2;
   int method_index;
   int i;

   /* Get reading range values */

   if(   gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pc->rangeToggleA))   
      || gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pc->rangeToggleB)))   
   {     
      Closure->readStart = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pc->rangeSpin1A));
      Closure->readEnd   = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pc->rangeSpin2A));
   }
   else Closure->readStart = Closure->readEnd = 0;

   /* Get fill byte and recalculate the dead sector marker */

   if(pc->byteCheckA && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pc->byteCheckA)))   
   {  const char *value1 = gtk_entry_get_text(GTK_ENTRY(pc->byteEntryA));
      const char *value2 = gtk_entry_get_text(GTK_ENTRY(pc->byteEntryB));
      int v1 = strtol(value1, NULL, 0);
      int v2 = strtol(value2, NULL, 0);

      /* both fields may contain different values */

      if(Closure->fillUnreadable != v2)
      {  Closure->fillUnreadable = v2;
 	 gtk_entry_set_text(GTK_ENTRY(pc->byteEntryA), value2);
      }
      else
	if(Closure->fillUnreadable != v1)
	{  Closure->fillUnreadable = v1;
	   gtk_entry_set_text(GTK_ENTRY(pc->byteEntryB), value1);
	}

      if(Closure->fillUnreadable < 0)
	Closure->fillUnreadable = 1;

      if(Closure->fillUnreadable > 255)
	Closure->fillUnreadable = 255;
   }

   /* Get raw reading mode */

   if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pc->radioRawModeOtherA)))   
   {  const char *value1 = gtk_entry_get_text(GTK_ENTRY(pc->rawModeValueA));
      const char *value2 = gtk_entry_get_text(GTK_ENTRY(pc->rawModeValueB));
      int v1 = strtol(value1, NULL, 0);
      int v2 = strtol(value2, NULL, 0);

      /* both fields may contain different values */

      if(Closure->rawMode != v2)
      {  Closure->rawMode = v2;
 	 gtk_entry_set_text(GTK_ENTRY(pc->rawModeValueA), value2);
      }
      else
	if(Closure->rawMode != v1)
	{  Closure->rawMode = v1;
	   gtk_entry_set_text(GTK_ENTRY(pc->rawModeValueB), value1);
	}

      if(Closure->rawMode < 0)
	Closure->rawMode = 0;

      if(Closure->rawMode > 255)
	Closure->rawMode = 255;
   }

   /* Get defective sector cache prefix.
      Both entries might contain different input. */

   value1 = gtk_entry_get_text(GTK_ENTRY(pc->cacheDefectivePrefixA));
   value2 = gtk_entry_get_text(GTK_ENTRY(pc->cacheDefectivePrefixB));
   if(strcmp(Closure->dDumpPrefix, value1))
   {  g_free(Closure->dDumpPrefix);
      Closure->dDumpPrefix = g_strdup(value1);
      gtk_entry_set_text(GTK_ENTRY(pc->cacheDefectivePrefixB), value1);
   }
   else if(strcmp(Closure->dDumpPrefix, value2))
   {  g_free(Closure->dDumpPrefix);
      Closure->dDumpPrefix = g_strdup(value2);
      gtk_entry_set_text(GTK_ENTRY(pc->cacheDefectivePrefixA), value2);
   }

   /* Ask currently selected method to update its settings
      from the preferences */

   method_index = gtk_notebook_get_current_page(GTK_NOTEBOOK(pc->methodNotebook));
   method = g_ptr_array_index(Closure->methodList, method_index);
   if(method->readPreferences)
     method->readPreferences(method);

   /* hide preferences and finish */

   gtk_widget_hide(GTK_WIDGET(Closure->prefsWindow));

   for(i=0; i<pc->helpPages->len; i++)
   {  LabelWithOnlineHelp *lwoh = g_ptr_array_index(pc->helpPages,i);
      gtk_widget_hide(lwoh->helpWindow);
   }
}

static void close_cb(GtkWidget *widget, gpointer data)
{  
   HidePreferences();
}

/***
 *** Setting preferences from external functions 
 ***/

void UpdatePrefsQuerySize(void)
{  prefs_context *pc = (prefs_context*)Closure->prefsContext;

   if(Closure->prefsContext)
     switch(Closure->querySize)
     {  case 0: activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioDriveA), TRUE); 
	        activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioDriveB), TRUE); 
	        break;
        case 1: activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioISOA), TRUE);
	        activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioISOB), TRUE);
	        break;
        case 2: activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioECCA), TRUE);
	        activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioECCB), TRUE);
	        break;
     }
}

/*
 * Register a preferences help window 
 */

void RegisterPreferencesHelpWindow(LabelWithOnlineHelp *lwoh)
{  prefs_context *pc = (prefs_context*)Closure->prefsContext;

   g_ptr_array_add(pc->helpPages, lwoh);
}

/*
 * Actions used in the preferences
 */

enum 
{  TOGGLE_READ_CREATE,
   TOGGLE_UNLINK,
   TOGGLE_SUFFIX,
   TOGGLE_DAO,
   TOGGLE_DSM,
   TOGGLE_2GB,
   TOGGLE_RANGE,
   TOGGLE_RAW,
   TOGGLE_RAW_20H,
   TOGGLE_RAW_21H,
   TOGGLE_RAW_OTHER,
   TOGGLE_CACHE_DEFECTIVE,
   TOGGLE_CANCEL_OK,
   TOGGLE_FATAL_SENSE,
   TOGGLE_EJECT,
   TOGGLE_VERBOSE,
   TOGGLE_LOGFILE,

   SPIN_DELAY,
   SPIN_INTERNAL_ATTEMPTS,
   SPIN_READ_MEDIUM,

   SLIDER_JUMP,
   SLIDER_MIN_READ_ATTEMPTS,
   SLIDER_MAX_READ_ATTEMPTS,

   COLOR_RED,
   COLOR_YELLOW,
   COLOR_GREEN,
   COLOR_BLUE,
   COLOR_WHITE,
   COLOR_DARK,
   COLOR_RED_TEXT,
   COLOR_GREEN_TEXT,
   COLOR_BAR,
   COLOR_LOG,
   COLOR_CURVE
};

/*
 * Create a new notebook page
 */

static GtkWidget *create_page(GtkWidget *notebook, char *label)
{  GtkWidget *vbox,*tab_label;

   tab_label = gtk_label_new(label);
   vbox = gtk_vbox_new(FALSE, 5);
   gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);
   
   gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, tab_label);

   return vbox;
}

/***
 *** Toggle button actions
 ***/

static void toggle_cb(GtkWidget *widget, gpointer data)
{  int state  = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
   int action = GPOINTER_TO_INT(data);
   prefs_context *pc = (prefs_context*)Closure->prefsContext;

   switch(action)
   {  case TOGGLE_READ_CREATE:
	Closure->readAndCreate = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->readAndCreateButtonA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->readAndCreateButtonB), state);
	if(state && Closure->adaptiveRead)  /* set reading strategy to linear */
	{  prefs_context *pc = Closure->prefsContext;

	   Closure->adaptiveRead = FALSE;
	   pc->jumpScaleInfoA->format = pc->formatLinear;
	   pc->jumpScaleInfoB->format = pc->formatLinear;
	   activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioLinearA), TRUE);
	   activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioLinearB), TRUE);

	   ShowMessage(Closure->prefsWindow,
		       _("Switched to the linear reading strategy."), 
		       GTK_MESSAGE_INFO);
	}
	   
	break;

      case TOGGLE_UNLINK:
	Closure->unlinkImage = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->unlinkImageButtonA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->unlinkImageButtonB), state);
	break;

      case TOGGLE_SUFFIX:
	Closure->autoSuffix = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->suffixA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->suffixB), state);
	break;

      case TOGGLE_CACHE_DEFECTIVE:
	Closure->defectiveDump = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->cacheDefectiveA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->cacheDefectiveB), state);
	break;

      case TOGGLE_LOGFILE:
	Closure->logFileEnabled = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->logFileA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->logFileB), state);
	break;

      case TOGGLE_CANCEL_OK:
	Closure->reverseCancelOK = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->cancelOKA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->cancelOKB), state);
	break;

      case TOGGLE_DAO:
	Closure->noTruncate = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->daoButtonA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->daoButtonB), state);
	break;

      case TOGGLE_DSM:
	Closure->dsmVersion = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->dsmButtonA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->dsmButtonB), state);
	if(state)
	{  if(pc->byteCheckA)
	      activate_toggle_button(GTK_TOGGLE_BUTTON(pc->byteCheckA), FALSE);
	   if(pc->byteCheckB)
	      activate_toggle_button(GTK_TOGGLE_BUTTON(pc->byteCheckB), FALSE);
	}
	break;

      case TOGGLE_2GB:
	Closure->splitFiles = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->splitA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->splitB), state);
	break;

      case TOGGLE_RAW:
	Closure->readRaw = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->rawButtonA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->rawButtonB), state);
	break;

      case TOGGLE_FATAL_SENSE:
	Closure->ignoreFatalSense = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->fatalSenseA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->fatalSenseB), state);
	break;

      case TOGGLE_EJECT:
	Closure->eject = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->ejectA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->ejectB), state);
	break;

      case TOGGLE_RAW_20H:
	if(state)
	{  Closure->rawMode = 0x20;
	   activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioRawMode20A), state);
	   activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioRawMode20B), state);
	   set_widget_sensitive(pc->rawModeValueA, FALSE);
      	   set_widget_sensitive(pc->rawModeValueB, FALSE);
	   set_entry_text(GTK_ENTRY(pc->rawModeValueA), "0x20");
	   set_entry_text(GTK_ENTRY(pc->rawModeValueB), "0x20");
	}
	break;

      case TOGGLE_RAW_21H:
	if(state)
	{  Closure->rawMode = 0x21;
	   activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioRawMode21A), state);
	   activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioRawMode21B), state);
	   set_widget_sensitive(pc->rawModeValueA, FALSE);
	   set_widget_sensitive(pc->rawModeValueB, FALSE);
	   set_entry_text(GTK_ENTRY(pc->rawModeValueA), "0x21");
	   set_entry_text(GTK_ENTRY(pc->rawModeValueB), "0x21");
	}
	break;

      case TOGGLE_RAW_OTHER:
	if(state)
	{  activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioRawModeOtherA), state);
	   activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioRawModeOtherB), state);
	   set_widget_sensitive(pc->rawModeValueA, TRUE);
      	   set_widget_sensitive(pc->rawModeValueB, TRUE);
	}
	break;

      case TOGGLE_VERBOSE:
	Closure->verbose = state;
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->verboseA), state);
	activate_toggle_button(GTK_TOGGLE_BUTTON(pc->verboseB), state);
	break;

      case TOGGLE_RANGE:
      {  int image_size = CurrentMediumSize(FALSE) - 1;

	 set_widget_sensitive(pc->rangeSpin1A, state);
	 set_widget_sensitive(pc->rangeSpin1B, state);
	 set_widget_sensitive(pc->rangeSpin2A, state);
	 set_widget_sensitive(pc->rangeSpin2B, state);

	 gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1A), 0.0);
	 gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1B), 0.0);
	 
	 if(state) 
	 {  gtk_spin_button_set_range(GTK_SPIN_BUTTON(pc->rangeSpin2A), 0.0, image_size);
	    gtk_spin_button_set_range(GTK_SPIN_BUTTON(pc->rangeSpin2B), 0.0, image_size);
	    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2A), image_size);
	    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2B), image_size);
	 }
	 else
	 {  gtk_spin_button_set_range(GTK_SPIN_BUTTON(pc->rangeSpin2A), 0.0, 1.0);
	    gtk_spin_button_set_range(GTK_SPIN_BUTTON(pc->rangeSpin2B), 0.0, 1.0);
	    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2A), 0.1);
	    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2B), 0.1);
	 }

	 activate_toggle_button(GTK_TOGGLE_BUTTON(pc->rangeToggleA), state);
	 activate_toggle_button(GTK_TOGGLE_BUTTON(pc->rangeToggleB), state);
      }
	break;
   }
}

/***
 *** Spin button actions
 ***/

static void spin_cb(GtkWidget *widget, gpointer data)
{  int which = GPOINTER_TO_INT(data);
   int value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   prefs_context *pc = (prefs_context*)Closure->prefsContext;

   switch(which)
   {  case SPIN_DELAY:
	Closure->spinupDelay = value;
	if(widget == pc->spinUpA)
	{  if(pc->spinUpB)
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->spinUpB), value);
	}
	if(widget == pc->spinUpB)
	{  if(pc->spinUpA)
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->spinUpA), value);
	}
	break;

      case SPIN_INTERNAL_ATTEMPTS:
	Closure->internalAttempts = value;
	if(widget == pc->internalAttemptsA)
	{  if(pc->internalAttemptsB)
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->internalAttemptsB), value);
	}
	if(widget == pc->internalAttemptsB)
	{  if(pc->internalAttemptsA)
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->internalAttemptsA), value);
	}
	break;

      case SPIN_READ_MEDIUM:
	Closure->readingPasses = value;
	if(widget == pc->readMediumA)
	{  if(pc->readMediumB)
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->readMediumB), value);
	}
	if(widget == pc->readMediumB)
	{  if(pc->readMediumA)
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->readMediumA), value);
	}
	break;
   }
}

/*
 * Make sure the reading range is a valid interval
 */

static void read_range_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)data;

   if(pc->rangeSpin1A == widget || pc->rangeSpin2A == widget)
   {  int from = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pc->rangeSpin1A));
      int to = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pc->rangeSpin2A));

      if(from > to)
      {  if(widget == pc->rangeSpin1A)
	 {    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1A), to);
	      gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1B), to);
	 }
	 else 
	 {    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2A), from);
	      gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2B), from);
	 }
      }
      else 
      {    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1B), from);
	   gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2B), to);
      }
   }
   else
   {  int from = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pc->rangeSpin1B));
      int to = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pc->rangeSpin2B));

      if(from > to)
      {  if(widget == pc->rangeSpin1B)
	 {    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1A), to);
	      gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1B), to);
	 }
	 else 
	 {    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2A), from);
	      gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2B), from);
	 }
      }
      else 
      {    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin1A), from);
	   gtk_spin_button_set_value(GTK_SPIN_BUTTON(pc->rangeSpin2A), to);
      }
   }
}

/***
 *** Color buttons
 ***/

/*
 * Create a color button. We need to do this manually as the GTK color button
 * won't let us manipulate the button order.
 */

#define COLOR_BUTTON_WIDTH 32
#define COLOR_BUTTON_HEIGHT 12

static gboolean color_delete_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{  color_button_info *cbi = (color_button_info*)data;

   cbi->dialog = NULL;
   return FALSE;
}

static void update_color_buttons(color_button_info *a, color_button_info *b, int redraw)
{  GdkRectangle rect;
   GdkWindow *window;
   gint ignore;

   /* Note that a->color and b->color point to the same object. */

   gdk_colormap_alloc_color(gdk_colormap_get_system(), a->color, FALSE, TRUE);

   gdk_gc_set_rgb_fg_color(a->gc, a->color);   
   gdk_gc_set_rgb_bg_color(a->gc, a->color);
   gdk_gc_set_rgb_fg_color(b->gc, b->color);   
   gdk_gc_set_rgb_bg_color(b->gc, b->color);

   gdk_draw_rectangle(a->pixmap, a->gc, TRUE, 0, 0, COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
   gdk_draw_rectangle(b->pixmap, b->gc, TRUE, 0, 0, COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);

   /* Trigger an expose event for the button. Since the button has no own window
      this will actually redraw the whole dialog box. */

   if(redraw) /* Useful when all buttons are reset to default at once */
   {  window = gtk_widget_get_parent_window(a->button);
      if(window) 
      {  gdk_window_get_geometry(window, &rect.x, &rect.y, &rect.width, &rect.height, &ignore);

	 gdk_window_invalidate_rect(a->button->window, &rect, TRUE); 
      }
   }

   window = gtk_widget_get_parent_window(b->button);
   if(window)
   {  gdk_window_get_geometry(window, &rect.x, &rect.y, &rect.width, &rect.height, &ignore);
	 
      gdk_window_invalidate_rect(window, &rect, TRUE); 
   }
}

static void color_ok_cb(GtkWidget *widget, gpointer data)
{  color_button_info *cbi = (color_button_info*)data;
   prefs_context *pc = (prefs_context*)Closure->prefsContext;


   gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(((GtkColorSelectionDialog*)cbi->dialog)->colorsel), cbi->color);

   switch(cbi->userData)
   {  case COLOR_RED:
	 update_color_buttons(pc->redA, pc->redB, TRUE);
	 break;

      case COLOR_YELLOW:
	 update_color_buttons(pc->yellowA, pc->yellowB, TRUE);
	 break;

      case COLOR_GREEN:
	 update_color_buttons(pc->greenA, pc->greenB, TRUE);
	 break;

      case COLOR_BLUE:
	 update_color_buttons(pc->blueA, pc->blueB, TRUE);
	 break;

      case COLOR_WHITE:
	 update_color_buttons(pc->whiteA, pc->whiteB, TRUE);
	 break;

      case COLOR_DARK:
	 update_color_buttons(pc->darkA, pc->darkB, TRUE);
	 break;

      case COLOR_RED_TEXT:
	 update_color_buttons(pc->redTextA, pc->redTextB, TRUE);
	 UpdateMarkup(&Closure->redMarkup, Closure->redText);
	 break;

      case COLOR_GREEN_TEXT:
	 update_color_buttons(pc->greenTextA, pc->greenTextB, TRUE);
	 UpdateMarkup(&Closure->greenMarkup, Closure->greenText);
	 break;

      case COLOR_BAR:
	 update_color_buttons(pc->barColorA, pc->barColorB, TRUE);
	 break;

      case COLOR_LOG:
	 update_color_buttons(pc->logColorA, pc->logColorB, TRUE);
	 break;

      case COLOR_CURVE:
	 update_color_buttons(pc->curveColorA, pc->curveColorB, TRUE);
	 break;
   }

   gtk_widget_hide(cbi->dialog);
}

static void color_cancel_cb(GtkWidget *widget, gpointer data)
{  color_button_info *cbi = (color_button_info*)data;

   gtk_widget_hide(cbi->dialog);
}

static void color_choose_cb(GtkWidget *widget, gpointer data)
{  color_button_info *cbi = (color_button_info*)data;

   if(!cbi->dialog)
   {  GtkColorSelectionDialog *csd;
      cbi->dialog = gtk_color_selection_dialog_new(_utf("Color selection"));
      g_signal_connect(cbi->dialog, "delete_event", G_CALLBACK(color_delete_cb), cbi);

      csd = (GtkColorSelectionDialog*)cbi->dialog;
      g_signal_connect(G_OBJECT(csd->cancel_button), "clicked", G_CALLBACK(color_cancel_cb), cbi);
      g_signal_connect(G_OBJECT(csd->ok_button), "clicked", G_CALLBACK(color_ok_cb), cbi);

      ReverseCancelOK(GTK_DIALOG(cbi->dialog));
   }

   gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(((GtkColorSelectionDialog*)cbi->dialog)->colorsel), cbi->color);
   gtk_widget_show(cbi->dialog);
}

static void default_color_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)Closure->prefsContext;

   DefaultColors();

   update_color_buttons(pc->redA, pc->redB, FALSE);
   update_color_buttons(pc->yellowA, pc->yellowB, FALSE);
   update_color_buttons(pc->greenA, pc->greenB, FALSE);
   update_color_buttons(pc->blueA, pc->blueB, FALSE);
   update_color_buttons(pc->whiteA, pc->whiteB, FALSE);
   update_color_buttons(pc->darkA, pc->darkB, FALSE);
   update_color_buttons(pc->redTextA, pc->redTextB, FALSE);
   update_color_buttons(pc->greenTextA, pc->greenTextB, FALSE);
   update_color_buttons(pc->barColorA, pc->barColorB, FALSE);
   update_color_buttons(pc->logColorA, pc->logColorB, FALSE);
   update_color_buttons(pc->curveColorA, pc->curveColorB, TRUE);
}

static color_button_info *create_color_button(GdkColor *color, int user_data)
{  color_button_info *cbi = g_malloc0(sizeof(color_button_info));

   cbi->button = gtk_button_new();
   cbi->frame  = gtk_frame_new(NULL);
   gtk_container_set_border_width(GTK_CONTAINER(cbi->frame), 1);
   gtk_frame_set_shadow_type(GTK_FRAME(cbi->frame), GTK_SHADOW_ETCHED_OUT);
   gtk_container_add(GTK_CONTAINER(cbi->button), cbi->frame); 
   cbi->pixmap = gdk_pixmap_new(gdk_get_default_root_window(), COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT, -1);
   cbi->image  = gtk_image_new_from_pixmap(cbi->pixmap, NULL);
   gtk_container_add(GTK_CONTAINER(cbi->frame), cbi->image); 

   cbi->gc    = gdk_gc_new(cbi->pixmap);
   gdk_gc_set_foreground(cbi->gc, color);   
   gdk_gc_set_background(cbi->gc, color);
   gdk_draw_rectangle(cbi->pixmap, cbi->gc, TRUE, 0, 0, COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);
   cbi->color = color;
   cbi->userData = user_data;
   g_signal_connect(G_OBJECT(cbi->button), "clicked", G_CALLBACK(color_choose_cb), cbi);

   return cbi;

  //   gtk_widget_modify_bg(ebox, GTK_STATE_NORMAL, color);
}

/***
 *** Non-linear scales
 ***/

static int jump_values[] = { 0, 16, 32, 64, 128, 256, 384, 512, 768, 
			     1024, 2048, 4096, 10240, 20480 };
#define JUMP_VALUE_LENGTH 14

static int min_attempts_values[] = { 1, 2, 3, 4, 5, 7, 9, 11, 15, 20, 25, 30, 40, 50}; 
static int max_attempts_values[] = { 1, 2, 3, 4, 5, 7, 9, 11, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100}; 
#define MIN_ATTEMPTS_VALUE_LENGTH 14
#define MAX_ATTEMPTS_VALUE_LENGTH 19

static void non_linear_cb(GtkWidget *widget, gpointer data)
{  non_linear_info *nli = (non_linear_info*)data;
   int index  = gtk_range_get_value(GTK_RANGE(widget));
   char *text,*utf;

   text = g_strdup_printf(nli->format, nli->values[index]);
   utf = g_locale_to_utf8(text, -1, NULL, NULL, NULL);

   switch(nli->action)
   {  case SLIDER_JUMP:
        Closure->sectorSkip = nli->values[index];

	gtk_range_set_value(GTK_RANGE(nli->pc->jumpScaleB), index);
	gtk_label_set_markup(GTK_LABEL(nli->pc->jumpScaleInfoB->label), utf);

	gtk_range_set_value(GTK_RANGE(nli->pc->jumpScaleA), index);
	gtk_label_set_markup(GTK_LABEL(nli->pc->jumpScaleInfoA->label), utf);
	SetOnlineHelpLinkText(nli->pc->jumpScaleLwoh, text);
	break;

      case SLIDER_MIN_READ_ATTEMPTS:
      { int max  = gtk_range_get_value(GTK_RANGE(nli->pc->maxAttemptsScaleA));

        Closure->minReadAttempts = nli->values[index];

	gtk_range_set_value(GTK_RANGE(nli->pc->minAttemptsScaleB), index);
	gtk_label_set_markup(GTK_LABEL(nli->pc->minAttemptsScaleInfoB->label), utf);

	gtk_range_set_value(GTK_RANGE(nli->pc->minAttemptsScaleA), index);
	gtk_label_set_markup(GTK_LABEL(nli->pc->minAttemptsScaleInfoA->label), utf);
	SetOnlineHelpLinkText(nli->pc->minAttemptsScaleLwoh, text);

	if(index > max)
	{
	   gtk_range_set_value(GTK_RANGE(nli->pc->maxAttemptsScaleA), index);
	   gtk_range_set_value(GTK_RANGE(nli->pc->maxAttemptsScaleB), index);
	}
      }
      break;

      case SLIDER_MAX_READ_ATTEMPTS:
      { int min  = gtk_range_get_value(GTK_RANGE(nli->pc->minAttemptsScaleA));

        Closure->maxReadAttempts = nli->values[index];

	gtk_range_set_value(GTK_RANGE(nli->pc->maxAttemptsScaleB), index);
	gtk_label_set_markup(GTK_LABEL(nli->pc->maxAttemptsScaleInfoB->label), utf);

	gtk_range_set_value(GTK_RANGE(nli->pc->maxAttemptsScaleA), index);
	gtk_label_set_markup(GTK_LABEL(nli->pc->maxAttemptsScaleInfoA->label), utf);
	SetOnlineHelpLinkText(nli->pc->maxAttemptsScaleLwoh, text);

	if(index < min)
	{
	   gtk_range_set_value(GTK_RANGE(nli->pc->minAttemptsScaleA), index);
	   gtk_range_set_value(GTK_RANGE(nli->pc->minAttemptsScaleB), index);
	}
      }
      break;
   }

   g_free(utf);
   g_free(text);
}

static gchar* non_linear_format_cb(GtkScale *scale, gdouble value, gpointer data)
{  char *text;

   text = g_strdup(" ");
   FORGET(text);   /* The scale will free the old string by itself. Weird. */

   return text;
}

static GtkWidget* non_linear_scale(GtkWidget **hbox_out, non_linear_info *nli, 
				   GtkWidget *label, prefs_context *pc, 
				   int action, int *values, int n, int value)
{  GtkWidget *scale;
   GtkWidget *hbox;
   char *text,*utf;
   int index;

   for(index = 0; index < n; index++)
     if(values[index] > value)
       break;

   nli->action = action;
   nli->values = values;
   nli->pc     = pc;

   hbox = gtk_hbox_new(FALSE, 0);
   scale = gtk_hscale_new_with_range(0,n-1,1);
   gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
   gtk_range_set_increments(GTK_RANGE(scale), 1, 1);
   gtk_range_set_value(GTK_RANGE(scale), index > 0 ? index-1 : index);
   g_signal_connect(scale, "format-value", G_CALLBACK(non_linear_format_cb), nli);
   g_signal_connect(scale, "value-changed", G_CALLBACK(non_linear_cb), nli);

   gtk_box_pack_start(GTK_BOX(hbox), scale, TRUE, TRUE, 0);

   text = g_strdup_printf(nli->format, nli->values[index > 0 ? index-1 : index]);
   utf = g_locale_to_utf8(text, -1, NULL, NULL, NULL);
   nli->label = label;
   gtk_label_set_markup(GTK_LABEL(label), utf);

   if(nli->lwoh)
   {    SetOnlineHelpLinkText(nli->lwoh, text);
        gtk_box_pack_start(GTK_BOX(hbox), nli->lwoh->linkBox, FALSE, FALSE, 0);
   }
   else gtk_box_pack_start(GTK_BOX(hbox), nli->label, FALSE, FALSE, 0);
   g_free(utf);
   g_free(text);

   *hbox_out = hbox;
   return scale;
}

/*
 * Image size query method selection 
 */

static void imgsize_cb(GtkWidget *widget, gpointer data)
{  int state  = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
   int selection = GPOINTER_TO_INT(data);
   prefs_context *pc = (prefs_context*)Closure->prefsContext;

   if(!state)  /* only track changes to activate state */
     return;

   Closure->querySize = selection;

   switch(selection)
   {  case 0:
        if(pc->radioDriveA)
          activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioDriveA), TRUE); 
        if(pc->radioDriveB)
          activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioDriveB), TRUE); 
	break;

      case 1:
        if(pc->radioISOA)
          activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioISOA), TRUE); 
        if(pc->radioISOB)
          activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioISOB), TRUE); 
        break;

      case 2:
        if(pc->radioECCA)
          activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioECCA), TRUE); 
        if(pc->radioECCB)
          activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioECCB), TRUE); 
        break;
   }
}

/*
 * Read strategy selection 
 */

static void strategy_cb(GtkWidget *widget, gpointer data)
{  int state  = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
   prefs_context *pc = (prefs_context*)data;

   if(pc->jumpScaleA && pc->jumpScaleB && state == TRUE)
   {  double tmp;

      if(pc->radioLinearA == widget || pc->radioLinearB == widget)
      {  Closure->adaptiveRead = FALSE;
         pc->jumpScaleInfoA->format = pc->formatLinear;
         pc->jumpScaleInfoB->format = pc->formatLinear;
	 activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioLinearA), TRUE);
	 activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioLinearB), TRUE);
      }

      if(pc->radioAdaptiveA == widget || pc->radioAdaptiveB == widget)
      {  Closure->adaptiveRead = TRUE;
         pc->jumpScaleInfoA->format = pc->formatAdaptive;
         pc->jumpScaleInfoB->format = pc->formatAdaptive;
	 activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioAdaptiveA), TRUE);
	 activate_toggle_button(GTK_TOGGLE_BUTTON(pc->radioAdaptiveB), TRUE);

	 if(Closure->readAndCreate)
	 {  Closure->readAndCreate = FALSE;

	    activate_toggle_button(GTK_TOGGLE_BUTTON(pc->readAndCreateButtonA), FALSE);
	    activate_toggle_button(GTK_TOGGLE_BUTTON(pc->readAndCreateButtonB), FALSE);
	    ShowMessage(Closure->prefsWindow,
			_("Disabled automatic error correction file generation."), 
			GTK_MESSAGE_INFO);
	 }
      }

      /* poor hack to make the range widgets redraw */

      tmp = gtk_range_get_value(GTK_RANGE(pc->jumpScaleA));
      gtk_range_set_value(GTK_RANGE(pc->jumpScaleA), tmp+1.0);
      gtk_range_set_value(GTK_RANGE(pc->jumpScaleA), tmp);
      gtk_range_set_value(GTK_RANGE(pc->jumpScaleB), tmp+1.0);
      gtk_range_set_value(GTK_RANGE(pc->jumpScaleB), tmp);
   }
}

/*
 * Select the mode page value for raw reading
 */

static void rawvalue_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)data;
   const char *value = gtk_entry_get_text(GTK_ENTRY(widget));
   char new_value[11];

   Closure->rawMode = strtol(value, NULL, 0);

   if(Closure->rawMode < 0)
   {  Closure->rawMode = 0;
      set_entry_text(GTK_ENTRY(pc->rawModeValueA), "0");
      set_entry_text(GTK_ENTRY(pc->rawModeValueB), "0");
      return;
   }

   if(Closure->rawMode > 255)
   {  Closure->rawMode = 255;
   }

   g_snprintf(new_value, 10, "0x%02x", Closure->rawMode);
   set_entry_text(GTK_ENTRY(pc->rawModeValueA), new_value);
   set_entry_text(GTK_ENTRY(pc->rawModeValueB), new_value);
}

/*
 * Select the value for filling unreadable sectors
 */

static void bytefill_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)data;
   const char *value = gtk_entry_get_text(GTK_ENTRY(widget));
   char byte[11];

   Closure->fillUnreadable = strtol(value, NULL, 0);

   if(Closure->fillUnreadable < 0)
   {  Closure->fillUnreadable = 1;
      set_entry_text(GTK_ENTRY(pc->byteEntryA), "1");
      set_entry_text(GTK_ENTRY(pc->byteEntryB), "1");
      return;
   }

   if(Closure->fillUnreadable > 255)
   {  Closure->fillUnreadable = 255;
   }

   g_snprintf(byte, 10, "0x%02x", Closure->fillUnreadable);
   set_entry_text(GTK_ENTRY(pc->byteEntryA), byte);
   set_entry_text(GTK_ENTRY(pc->byteEntryB), byte);
}


static void bytefill_check_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)data;
   int state  = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

   if(pc->byteCheckA)
     activate_toggle_button(GTK_TOGGLE_BUTTON(pc->byteCheckA), state);
   if(pc->byteCheckB)
     activate_toggle_button(GTK_TOGGLE_BUTTON(pc->byteCheckB), state);

   if(state)
   {  char byte[11];

      set_widget_sensitive(pc->byteEntryA, TRUE);
      set_widget_sensitive(pc->byteEntryB, TRUE);

      if(Closure->fillUnreadable < 0 || Closure->fillUnreadable > 255)
	 Closure->fillUnreadable = 0xb0;

      g_snprintf(byte, 10, "0x%02x", Closure->fillUnreadable);

      set_entry_text(GTK_ENTRY(pc->byteEntryA), byte);
      set_entry_text(GTK_ENTRY(pc->byteEntryB), byte);

      activate_toggle_button(GTK_TOGGLE_BUTTON(pc->dsmButtonA), FALSE);
      activate_toggle_button(GTK_TOGGLE_BUTTON(pc->dsmButtonB), FALSE);

   }
   else
   {  Closure->fillUnreadable = -1;
      set_widget_sensitive(pc->byteEntryA, FALSE);
      set_widget_sensitive(pc->byteEntryB, FALSE);
   }
}

/*
 * Enter the prefix for raw sector files
 */

static void defective_prefix_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)data;
   char *value = (char*)gtk_entry_get_text(GTK_ENTRY(widget));

   if(!value || !*value)
   {  set_entry_text(GTK_ENTRY(pc->cacheDefectivePrefixA), "sector-");
      set_entry_text(GTK_ENTRY(pc->cacheDefectivePrefixB), "sector-");
      return;

   }

   if(widget == pc->cacheDefectivePrefixA)
      set_entry_text(GTK_ENTRY(pc->cacheDefectivePrefixB), value);

   if(widget == pc->cacheDefectivePrefixB)
      set_entry_text(GTK_ENTRY(pc->cacheDefectivePrefixA), value);
}

/*
 * Run the file chooser for the raw sector files directory
 */

static void cache_defective_select_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)Closure->prefsContext;
   int action = GPOINTER_TO_INT(data);

   switch(action)
   {  case 0:  /* destroy */
	 pc->cacheDefectiveChooser = NULL;
	 break;

      case 1: /* OK */
	 if(Closure->dDumpDir)
	    g_free(Closure->dDumpDir);
	 Closure->dDumpDir = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(pc->cacheDefectiveChooser)));
	 if(pc->cacheDefectiveDirA)
	    gtk_label_set_text(GTK_LABEL(pc->cacheDefectiveDirA), Closure->dDumpDir);
	 if(pc->cacheDefectiveDirB)
	    gtk_label_set_text(GTK_LABEL(pc->cacheDefectiveDirB), Closure->dDumpDir);
	 gtk_widget_hide(pc->cacheDefectiveChooser);
	 break;

      case 2: /* Cancel */
	 gtk_widget_hide(pc->cacheDefectiveChooser);
	 break;
   }
}

static void cache_defective_dir_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)data;
   GtkWidget *file_list;

   if(!pc->cacheDefectiveChooser)
   {  char filename[strlen(Closure->dDumpDir)+10];

      pc->cacheDefectiveChooser = gtk_file_selection_new(_utf("Raw sector caching"));
      ReverseCancelOK(GTK_DIALOG(pc->cacheDefectiveChooser));

      g_signal_connect(G_OBJECT(pc->cacheDefectiveChooser), "destroy",
		       G_CALLBACK(cache_defective_select_cb), 
		       GINT_TO_POINTER(0));
      g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(pc->cacheDefectiveChooser)->ok_button),
		       "clicked", G_CALLBACK(cache_defective_select_cb), GINT_TO_POINTER(1));
      g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(pc->cacheDefectiveChooser)->cancel_button),
		       "clicked", G_CALLBACK(cache_defective_select_cb), GINT_TO_POINTER(2));

      sprintf(filename, "%s/", Closure->dDumpDir);
      gtk_file_selection_set_filename(GTK_FILE_SELECTION(pc->cacheDefectiveChooser),
				      filename);
      gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(pc->cacheDefectiveChooser));

      /* Hide the file selection parts */

      file_list = GTK_FILE_SELECTION(pc->cacheDefectiveChooser)->file_list;
      set_widget_sensitive(file_list, FALSE);
      gtk_widget_hide(GTK_FILE_SELECTION(pc->cacheDefectiveChooser)->selection_entry);
      set_entry_text(GTK_ENTRY(GTK_FILE_SELECTION(pc->cacheDefectiveChooser)->selection_entry), "");
#if 0
      gtk_widget_hide(file_list->parent);
#endif
   }

   gtk_widget_show(pc->cacheDefectiveChooser);
}


/*
 * Run the file chooser for the log file
 */

static void logfile_select_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)Closure->prefsContext;
   int action = GPOINTER_TO_INT(data);

   switch(action)
   {  case 0:  /* destroy */
	 pc->logFileChooser = NULL;
	 break;

      case 1: /* OK */
	 g_free(Closure->logFile);
	 Closure->logFile = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(pc->logFileChooser)));
	 Closure->logFileStamped = FALSE;
	 if(pc->logFilePathA)
	    gtk_label_set_text(GTK_LABEL(pc->logFilePathA), Closure->logFile);
	 if(pc->logFilePathB)
	    gtk_label_set_text(GTK_LABEL(pc->logFilePathB), Closure->logFile);
	 gtk_widget_hide(pc->logFileChooser);
	 break;

      case 2: /* Cancel */
	 gtk_widget_hide(pc->logFileChooser);
	 break;
   }
}

#define LOGFILE_SELECT 1
#define LOGFILE_DELETE 2

static void logfile_cb(GtkWidget *widget, gpointer data)
{  prefs_context *pc = (prefs_context*)Closure->prefsContext;
   int action = GPOINTER_TO_INT(data);

   switch(action)
   {  case LOGFILE_SELECT:
	 if(!pc->logFileChooser)
	 {  
	    pc->logFileChooser = gtk_file_selection_new(_utf("Log file"));
	    ReverseCancelOK(GTK_DIALOG(pc->logFileChooser));

	    g_signal_connect(G_OBJECT(pc->logFileChooser), "destroy",
			     G_CALLBACK(logfile_select_cb), 
			     GINT_TO_POINTER(0));
	    g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(pc->logFileChooser)->ok_button),
			     "clicked", G_CALLBACK(logfile_select_cb), GINT_TO_POINTER(1));
	    g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(pc->logFileChooser)->cancel_button),
			     "clicked", G_CALLBACK(logfile_select_cb), GINT_TO_POINTER(2));
	    
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(pc->logFileChooser),
					    Closure->logFile);
	 }
	 gtk_widget_show(pc->logFileChooser);
	 break;

      case LOGFILE_DELETE:
      {  GtkWidget *dialog = gtk_message_dialog_new(Closure->prefsWindow,
						    GTK_DIALOG_DESTROY_WITH_PARENT,
						    GTK_MESSAGE_QUESTION,
						    GTK_BUTTONS_OK_CANCEL,
						    _utf("Delete the log file?"));
	 int answer;
	   
	 ReverseCancelOK(GTK_DIALOG(dialog));
	 answer = gtk_dialog_run(GTK_DIALOG(dialog));
	 
	 if(answer == GTK_RESPONSE_OK)
	    LargeUnlink(Closure->logFile);
	 gtk_widget_destroy(dialog);
	 break;
      }
   }
}

/***
 *** Error correction method selection
 ***/

static void method_select_cb(GtkWidget *widget, gpointer data)
{  Method *method;
   prefs_context *pc = (prefs_context*)data;
   int n;

   n = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));

   if(n<0 || !pc->methodNotebook)
     return;

   method = g_ptr_array_index(Closure->methodList, n);

   /* Switch methods if selection changed */

   if(strncmp(Closure->methodName, method->name, 4))
   {  GtkWidget *other;

      strncpy(Closure->methodName, method->name, 4);
      gtk_notebook_set_current_page(GTK_NOTEBOOK(pc->methodNotebook), n);

      if(pc->methodChooserA == widget)
	   other = pc->methodChooserB;
      else other = pc->methodChooserA;

      gtk_combo_box_set_active(GTK_COMBO_BOX(other), n);
   }
}

/*
 * Setting the notebook page does not work at creation time.
 */

static gboolean notebook_idle_func(gpointer data)
{  prefs_context *pc = (prefs_context*)data;
   int n;

   n = gtk_combo_box_get_active(GTK_COMBO_BOX(pc->methodChooserA));

   if(n>=0)
     gtk_notebook_set_current_page(GTK_NOTEBOOK(pc->methodNotebook), n);

   gtk_notebook_set_current_page(GTK_NOTEBOOK(pc->mainNotebook), 0);

   return FALSE;
}

/***
 *** Assemble and open the preferences window.
 ***/

void UpdateMethodPreferences(void)
{  int i;
   
   for(i=0; i<Closure->methodList->len; i++)
   {  Method *method = g_ptr_array_index(Closure->methodList, i);

      method->resetPrefsPage(method);
   }
}

void CreatePreferencesWindow(void)
{  
   if(!Closure->prefsWindow)  /* No window to reuse? */
   {  GtkWidget *window, *outer_box, *notebook, *space;
      GtkWidget *hbox, *vbox, *vbox2, *vbox3, *button, *frame, *table;
      GtkWidget *lab;
      LabelWithOnlineHelp *lwoh,*lwoh_clone;
      prefs_context *pc = g_malloc0(sizeof(prefs_context));
      int i, method_idx = 0;
 
      pc->helpPages = g_ptr_array_new();
      Closure->prefsContext = pc;
 
      window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      Closure->prefsWindow = GTK_WINDOW(window);
      gtk_window_set_title(GTK_WINDOW(window), _utf("Preferences"));
      gtk_window_set_default_size(GTK_WINDOW(window), -1, 150);
      gtk_window_set_icon(GTK_WINDOW(window), Closure->windowIcon);
      gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
      gtk_container_set_border_width(GTK_CONTAINER(window), 12);

      /* Connect with the close button from the window manager */

      g_signal_connect(window, "delete_event", G_CALLBACK(delete_cb), NULL);

      /* Create the main layout of the window */

      outer_box = gtk_vbox_new(FALSE, 0);
      gtk_container_add(GTK_CONTAINER(window), outer_box);

      notebook = pc->mainNotebook = gtk_notebook_new();
      gtk_box_pack_start(GTK_BOX(outer_box), notebook, TRUE, TRUE, 0);

      space = gtk_image_new();
      gtk_box_pack_start(GTK_BOX(outer_box), space, FALSE, FALSE, 4);

      hbox = gtk_hbox_new(FALSE, 0);
      gtk_box_pack_start(GTK_BOX(outer_box), hbox, FALSE, FALSE, 0);

      button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
      gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
      g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(close_cb), NULL);

      /*** Image creation page */

      vbox = create_page(notebook, _utf("Image"));

      /** Image size */

      frame = gtk_frame_new(_utf("Image size"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 15);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      lwoh = CreateLabelWithOnlineHelp(_("Image size determination"), _("Get Image size from: "));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *radio1, *radio2, *radio3;

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 radio1 = gtk_radio_button_new(NULL);
	 g_signal_connect(G_OBJECT(radio1), "toggled", G_CALLBACK(imgsize_cb), (gpointer)0);
	 gtk_box_pack_start(GTK_BOX(hbox), radio1, FALSE, FALSE, 0);
	 lab = gtk_label_new(_utf("Drive"));
	 gtk_container_add(GTK_CONTAINER(radio1), lab);

	 radio2 = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(radio1));
	 g_signal_connect(G_OBJECT(radio2), "toggled", G_CALLBACK(imgsize_cb), (gpointer)1);
	 gtk_box_pack_start(GTK_BOX(hbox), radio2, FALSE, FALSE, 0);
	 lab = gtk_label_new(_utf("ISO/UDF"));
	 gtk_container_add(GTK_CONTAINER(radio2), lab);

	 radio3 = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(radio2));
	 g_signal_connect(G_OBJECT(radio3), "toggled", G_CALLBACK(imgsize_cb), (gpointer)2);
	 gtk_box_pack_start(GTK_BOX(hbox), radio3, FALSE, FALSE, 0);
	 lab = gtk_label_new(_utf("ECC/RS02"));
	 gtk_container_add(GTK_CONTAINER(radio3), lab);

	 switch(Closure->querySize)
	 {  case 0: activate_toggle_button(GTK_TOGGLE_BUTTON(radio1), TRUE); break;
            case 1: activate_toggle_button(GTK_TOGGLE_BUTTON(radio2), TRUE); break;
            case 2: activate_toggle_button(GTK_TOGGLE_BUTTON(radio3), TRUE); break;
	 }

	 if(!i)
	 {  pc->radioDriveA = radio1;
	    pc->radioISOA   = radio2;
	    pc->radioECCA   = radio3;
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 }
	 else  
	 {  pc->radioDriveB = radio1;
	    pc->radioISOB   = radio2;
	    pc->radioECCB   = radio3;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Image size determination</b>\n\n"
			 "Use <i>ECC/RS02</i> for reading images augmented with error correction data; "
			 "else pick <i>ISO/UDF</i>.\n\n"

			 "<b>ECC/RS02:</b> The Image size is determined from the error correction data. "
			 "Reading RS02 augmented images requires this option; otherwise the images "
			 "may be incomplete. However if the medium does not contain error correction "
			 "data, the start of the reading operation may be delayed significantly.\n\n"

			 "<b>ISO/UDF:</b> The image size is determined from the ISO/UDF file system.\n"
			 "Caution: This is only suitable for working with error correction files. "
			 "Images containing RS02 error correction information may be truncated.\n\n"

			 "<b>Drive:</b> The image size reported by the drive will be used. "
			 "As this information is typically wrong for DVD-RW/+RW/-RAM media this option "
			 "is only present for backwards compatibility with older dvdisaster versions."));

      /** Reading preferences */
      
      frame = gtk_frame_new(_utf("Image creation"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* Reading strategy */

      lwoh = CreateLabelWithOnlineHelp(_("Reading strategy"), _("Reading strategy: "));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *lab, *radio1, *radio2;

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 radio1 = gtk_radio_button_new(NULL);
	 if(!i) pc->radioLinearA = radio1;
	 else   pc->radioLinearB = radio1;
	 g_signal_connect(G_OBJECT(radio1), "toggled", G_CALLBACK(strategy_cb), pc);
	 gtk_box_pack_start(GTK_BOX(hbox), radio1, FALSE, FALSE, 0);
	 lab = gtk_label_new(_utf("Linear"));
	 gtk_container_add(GTK_CONTAINER(radio1), lab);

	 radio2 = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(radio1));
	 if(!i) pc->radioAdaptiveA = radio2;
	 else   pc->radioAdaptiveB = radio2;
 	 g_signal_connect(G_OBJECT(radio2), "toggled", G_CALLBACK(strategy_cb), pc);
	 gtk_box_pack_start(GTK_BOX(hbox), radio2, FALSE, FALSE, 0);
	 lab = gtk_label_new(_utf("Adaptive (for defective media)"));
	 gtk_container_add(GTK_CONTAINER(radio2), lab);

	 if(Closure->adaptiveRead)
	      activate_toggle_button(GTK_TOGGLE_BUTTON(radio2), TRUE);
         else activate_toggle_button(GTK_TOGGLE_BUTTON(radio1), TRUE);

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, hbox);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Reading strategy</b>\n\n"
			 "Use the <b>linear strategy</b> for:\n"
			 "- processing undamaged media, or\n"
		         "- reading defective media when no error correction data is available.\n\n"
			 "The <b>adaptive strategy</b> is optimized for\n"
			 "- reading defective media\n"
			 "- if (and only if) error correction data is available.\n\n"
			 "Using the adaptive strategy without error correction data "
			 "is possible but it is recommended to use linear reading in that case."), Closure->invisibleDash);

      /* Reading range */

      lwoh = CreateLabelWithOnlineHelp(_("Reading range"), _("Read/Scan from sector"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *toggle,*spin1, *spin2;

	 //	 toggle = gtk_check_button_new_with_label(_utf("Read/Scan from sector"));
	 toggle = gtk_check_button_new();
	 if(!i) pc->rangeToggleA = toggle;
	 else   pc->rangeToggleB = toggle;
	 g_signal_connect(G_OBJECT(toggle), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_RANGE));
	 gtk_box_pack_start(GTK_BOX(hbox), toggle, FALSE, FALSE, 0);

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, 
			    FALSE, FALSE, 0);

	 spin1 = gtk_spin_button_new_with_range(0, 10000000, 1000);
	 if(!i) pc->rangeSpin1A = spin1;	 
	 else   pc->rangeSpin1B = spin1;
	 gtk_entry_set_width_chars(GTK_ENTRY(spin1), 9);
	 set_widget_sensitive(spin1, FALSE);
	 g_signal_connect(spin1, "value-changed", G_CALLBACK(read_range_cb), pc);
	 gtk_box_pack_start(GTK_BOX(hbox), spin1, FALSE, FALSE, 0);

	 lab = gtk_label_new(_utf("to sector"));
	 gtk_box_pack_start(GTK_BOX(hbox), lab, FALSE, FALSE, 0);

	 spin2 = gtk_spin_button_new_with_range(0, 1, 10000);
	 if(!i) pc->rangeSpin2A = spin2;	 
	 else   pc->rangeSpin2B = spin2;
	 gtk_entry_set_width_chars(GTK_ENTRY(spin2), 9);
	 set_widget_sensitive(spin2, FALSE);
	 g_signal_connect(spin2, "value-changed", G_CALLBACK(read_range_cb), pc);
	 gtk_box_pack_start(GTK_BOX(hbox), spin2, FALSE, FALSE, 0);

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, hbox);

	 activate_toggle_button(GTK_TOGGLE_BUTTON(toggle), FALSE);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Reading range</b>\n\n"
			 "Reading can be limited to a part of the medium (in sectors holding 2KB each). "
			 "The values include the borders: 0-100 will read 101 sectors.\n\n"

			 "<b>Note:</b> Limiting the reading range is not recommended for <i>adaptive reading</i> since it might "
			 "prevent sectors from being read which are required for a succesful error correction.\n\n"
			 "These settings are only effective for the current session and will not be saved."));

      /** Image properties */

      frame = gtk_frame_new(_utf("Image properties"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 15);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* DAO button */

      lwoh = CreateLabelWithOnlineHelp(_("DAO mode"), _("Assume image to be written in DAO mode (don't truncate)"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 if(!i) pc->daoButtonA = button;
	 else   pc->daoButtonB = button;

	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->noTruncate);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_DAO));
	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, hbox);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Assume DAO mode</b>\n\n"
			 "Media written in \"TAO\" (\"track at once\") mode may contain two sectors "
			 "with pseudo read errors at the end. By default these two sectors are ignored.\n\n"

			 "If you are extremely unlucky to have a \"DAO\" (\"disc at once\") medium "
			 "with exactly one or two real read errors at the end, dvdisaster may treat "
			 "this as a \"TAO\" disc and truncate the image by two sectors. In that case "
			 "activate this option to have the last two read errors handled correctly.\n\n"

			 "<b>Tip:</b> To avoid these problems, consider using the \"DAO / Disc at once\" "
			 "(sometimes also called \"SAO / Session at once\") mode for writing single "
			 "session media."));

      /** Image format */
if(Closure->debugMode)  /* hidden until version 0.80 */
{                       /* because of severe compatibility issues */
      frame = gtk_frame_new(_utf("Image format"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 15);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* new style missing sector marker */

      lwoh = CreateLabelWithOnlineHelp(_("Missing sector tags"), _("Use new style missing sector tags (Warning: compatibility issues!)"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 if(!i) pc->dsmButtonA = button;
	 else   pc->dsmButtonB = button;

	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->dsmVersion);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_DSM));
	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, hbox);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Missing sector tagging</b>\n\n"
			 "Missing sectors are tagged with a special code sequence "
			 "in the image. If this value is activated, an improved "
			 "code is used which can detect some wilfully damaged "
			 "content. This includes media which have been created "
			 "from partially recovered images, and images containing "
			 "files from such partial media.\n"
			 "However only dvdisaster 0.72 and up will recognize "
			 "the new tags. You must NOT process the resulting "
			 "images with older dvdisaster versions as they would "
                         "not see any missing sectors in the resulting images.\n"
			 "N.b.: dvdisaster >= 0.72 will automatically recognize "
			 "both tag formats when reading images; setting this value "
			 "only affects the creation of new images."));
}
      /* byte filling */

      if(Closure->debugMode)
      {
	 lwoh = CreateLabelWithOnlineHelp(_("Filling of unreadable sectors"), 
					  _("Fill unreadable sectors with byte:"));
	 RegisterPreferencesHelpWindow(lwoh);

	 for(i=0; i<2; i++)
	 {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	    GtkWidget *check, *entry;

	    check = gtk_check_button_new();
	    g_signal_connect(check, "toggled", G_CALLBACK(bytefill_check_cb), pc);
	    gtk_box_pack_start(GTK_BOX(hbox), check, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	    entry = gtk_entry_new();
	    g_signal_connect(entry, "activate", G_CALLBACK(bytefill_cb), pc);
	    gtk_entry_set_width_chars(GTK_ENTRY(entry), 5);
	    gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);

	    if(!i)
	    {  pc->byteCheckA = check;
	       pc->byteEntryA = entry;
	       gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    }
	    else
	    {  pc->byteCheckB = check;
	       pc->byteEntryB = entry;
	       AddHelpWidget(lwoh, hbox);
	    }

	    if(Closure->fillUnreadable >= 0)
	    {  char value[11];
	
	       g_snprintf(value, 10, "0x%02x", Closure->fillUnreadable);
	       gtk_entry_set_text(GTK_ENTRY(entry), value);
	       activate_toggle_button(GTK_TOGGLE_BUTTON(check), TRUE);
	    }
	    else set_widget_sensitive(entry, FALSE);
	 }

	 AddHelpParagraph(lwoh, 
			  _("<b>Filling of unreadable sectors</b>\n\n"
			    
			    "dvdisaster marks unreadable sectors with a special filling pattern which "
			    "is very unlikely to occur in undamaged media.\n"
			    "In other data recovery software it is common to fill unreadable sectors "
			    "with a certain byte value. To allow interoperability with such programs, "
			    "you can specify the byte value they are using:\n"));

	 AddHelpListItem(lwoh,
			 _("0xb0 (176 decimal): for compatibility with h2cdimage published by \"c't\", "
			   "a German periodical.\n"));

	 AddHelpParagraph(lwoh,
			  _("<b>Note:</b> Using zero filling (0x00, decimal 0) is highly discouraged. "
			    "Most media contain regular zero filled sectors which can not be told apart "
			    "from unreadable sectors if zero filling is used."));
      }

      /*** Drive parameters page */

      vbox = create_page(notebook, _utf("Drive"));

      /** Drive initialisation */

      frame = gtk_frame_new(_utf("Drive initialisation"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      lwoh = CreateLabelWithOnlineHelp(_("Drive initialisation"), 
				       _("Wait"));
      RegisterPreferencesHelpWindow(lwoh);

      lwoh_clone = CloneLabelWithOnlineHelp(lwoh, _("seconds for drive to spin up"));
      RegisterPreferencesHelpWindow(lwoh_clone);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *spin;

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 spin = gtk_spin_button_new_with_range(0, 30, 1);
	 gtk_entry_set_width_chars(GTK_ENTRY(spin), 3);
	 gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), Closure->spinupDelay);
	 g_signal_connect(spin, "value-changed", G_CALLBACK(spin_cb), (gpointer)SPIN_DELAY);
	 gtk_box_pack_start(GTK_BOX(hbox), spin, FALSE, FALSE, 0);

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh_clone->normalLabel : lwoh_clone->linkBox, FALSE, FALSE, 0);
	 gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);

	 if(!i)
	 {  pc->spinUpA = spin;
	    gtk_container_add(GTK_CONTAINER(frame), hbox);
	 }
	 else
	 {  pc->spinUpB = spin;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Drive initialisation</b>\n\n"
			 "Waits the specified amount of seconds for letting the drive spin up. "
			 "This avoids speed jumps at the beginning of the reading curve."));

      /** Drive raw reading parameters */

      frame = gtk_frame_new(_utf("Raw reading parameters"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 15);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* Raw reading mode */

      lwoh = CreateLabelWithOnlineHelp(_("Raw reading mode"), _("Raw reading mode: "));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *lab, *radio1, *radio2, *radio3, *entry;
	 char value[11];

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 radio1 = gtk_radio_button_new(NULL);
	 if(!i) pc->radioRawMode20A = radio1;
	 else   pc->radioRawMode20B = radio1;
	 g_signal_connect(G_OBJECT(radio1), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_RAW_20H));
	 gtk_box_pack_start(GTK_BOX(hbox), radio1, FALSE, FALSE, 0);
	 lab = gtk_label_new("0x20");
	 gtk_container_add(GTK_CONTAINER(radio1), lab);

	 radio2 = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(radio1));
	 if(!i) pc->radioRawMode21A = radio2;
	 else   pc->radioRawMode21B = radio2;
	 g_signal_connect(G_OBJECT(radio2), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_RAW_21H));
	 gtk_box_pack_start(GTK_BOX(hbox), radio2, FALSE, FALSE, 0);
	 lab = gtk_label_new("0x21");
	 gtk_container_add(GTK_CONTAINER(radio2), lab);

	 radio3 = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(radio2));
	 if(!i) pc->radioRawModeOtherA = radio3;
	 else   pc->radioRawModeOtherB = radio3;
 	 g_signal_connect(G_OBJECT(radio3), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_RAW_OTHER));
	 gtk_box_pack_start(GTK_BOX(hbox), radio3, FALSE, FALSE, 0);
	 lab = gtk_label_new(_("other:"));
	 gtk_container_add(GTK_CONTAINER(radio3), lab);

	 entry = gtk_entry_new();
	 g_signal_connect(entry, "activate", G_CALLBACK(rawvalue_cb), pc);
	 gtk_entry_set_width_chars(GTK_ENTRY(entry), 5);
	 gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
	 if(!i) pc->rawModeValueA = entry;
	 else   pc->rawModeValueB = entry;
	
	 g_snprintf(value, 10, "0x%02x", Closure->rawMode);
	 gtk_entry_set_text(GTK_ENTRY(entry), value);

	 switch(Closure->rawMode)
	 {  case 0x20:
	       activate_toggle_button(GTK_TOGGLE_BUTTON(radio1), TRUE);
	       set_widget_sensitive(entry, FALSE);
	       break;

	    case 0x21:
	       activate_toggle_button(GTK_TOGGLE_BUTTON(radio2), TRUE);
	       set_widget_sensitive(entry, FALSE);
	       break;

	    default:
	       activate_toggle_button(GTK_TOGGLE_BUTTON(radio3), TRUE);
	       break;
	 }

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, hbox);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Raw reading mode</b>\n\n"
			 "There are several ways to put the drive into a mode "
			 "which transfers partially read data from defective sectors:\n\n"
			 "<b>0x20</b> This is the <i>recommended</i> mode. "
			 "The drive tries to apply "
			 "the built-in error correction to the best possible extent "
			 "before transfering a defective sector.\n\n"
			 "<b>0x21</b> In this mode the drive skips the last stage "
			 "of its internal error correction and returns the "
			 "uncorrected sector instead. This may result in sectors "
			 "being tagged and processed as defective which would come "
			 "out good in other reading modes, causing unnecessary "
			 "work or even uncorrectable sectors.\n" 
			 "However some drives appear to be unable to transfer data "
			 "in mode 0x20, but can do so in mode 0x21, so this is your "
			 "last resort then. Also, if sectors are not recoverable "
			 "after reading and caching sectors in mode 0x20, then adding "
			 "some mode 0x21 reads to the cache might deliver additional information.\n\n"
			 "<b>0x01</b> Some drives do the right thing when given this value, "
			 "although this makes no sense according to the SCSI specs. Try entering "
			 "this value in the \"other\" field if the other choices do not work. "
			 "See the mode page 01h documentation in chapter 6 of MMC3 or later "
			 "for additional information."));

      /* Firmware rereads */

      lwoh = CreateLabelWithOnlineHelp(_("Internal read attempts"), 
				       _("Reread defective sectors"));
      RegisterPreferencesHelpWindow(lwoh);

      lwoh_clone = CloneLabelWithOnlineHelp(lwoh, _("times"));
      RegisterPreferencesHelpWindow(lwoh_clone);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *spin;

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 spin = gtk_spin_button_new_with_range(-1, 10, 1);
	 gtk_entry_set_width_chars(GTK_ENTRY(spin), 3);
	 gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), Closure->internalAttempts);
	 g_signal_connect(spin, "value-changed", G_CALLBACK(spin_cb), (gpointer)SPIN_INTERNAL_ATTEMPTS);
	 gtk_box_pack_start(GTK_BOX(hbox), spin, FALSE, FALSE, 0);

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh_clone->normalLabel : lwoh_clone->linkBox, FALSE, FALSE, 0);
	 //	 gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);

	 if(!i)
	 {  pc->internalAttemptsA = spin;
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    //	    gtk_container_add(GTK_CONTAINER(frame), hbox);
	 }
	 else
	 {  pc->internalAttemptsB = spin;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Internal read attempts</b>\n\n"
			 "The drive firmware usually retries unreadable sectors "
			 "a few times before giving up and returning a read error.\n"
			 "But it is usually more efficient to manage the reading "
			 "attempts from the client software, e.g. through the "
			 "settings in the \"Read attempts\" preferences tab.\n"
			 "Lowering this value to 0 or 1 can speed up processing "
			 "of damaged media and reduce the drive wear; however "
			 "most drives will simply ignore what you enter here.\n"
			 "Use the value -1 to leave the drive at its default setting."));

      /* Fatal error handling */

      frame = gtk_frame_new(_utf("Fatal error handling"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      lwoh = CreateLabelWithOnlineHelp(_("Fatal error handling"), 
				       _("Ignore fatal errors"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *toggle;

	 toggle = gtk_check_button_new();
	 activate_toggle_button(GTK_TOGGLE_BUTTON(toggle), Closure->ignoreFatalSense);
       	 g_signal_connect(G_OBJECT(toggle), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_FATAL_SENSE));
	 gtk_box_pack_start(GTK_BOX(hbox), toggle, FALSE, FALSE, 0);

	 if(!i)
	 {  pc->fatalSenseA = toggle;
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_container_add(GTK_CONTAINER(frame), hbox);
	    gtk_container_set_border_width(GTK_CONTAINER(hbox), 12);
	 }
	 else
	 {  pc->fatalSenseB = toggle;
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	 }
      }	 

      AddHelpParagraph(lwoh, 
		       _("<b>Fatal error handling</b>\n\n"
			 "By default dvdisaster stops reading when the drive "
			 "reports a fatal error. This prevents further fruitless "
			 "read attempts and possible damage to the drive.\n"
			 "However some drives produce unfounded fatal messages. "
			 "For such drives ignoring fatal errors may be needed to "
			 "do uninterrupted reading of damaged media."));

      /* Eject medium */

      frame = gtk_frame_new(_utf("Media ejection"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      lwoh = CreateLabelWithOnlineHelp(_("Eject medium after sucessful read"), 
				       _("Eject medium after sucessful read"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *toggle;

	 toggle = gtk_check_button_new();
	 activate_toggle_button(GTK_TOGGLE_BUTTON(toggle), Closure->eject);
       	 g_signal_connect(G_OBJECT(toggle), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_EJECT));
	 gtk_box_pack_start(GTK_BOX(hbox), toggle, FALSE, FALSE, 0);

	 if(!i)
	 {  pc->ejectA = toggle;
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_container_add(GTK_CONTAINER(frame), hbox);
	    gtk_container_set_border_width(GTK_CONTAINER(hbox), 12);
	 }
	 else
	 {  pc->ejectB = toggle;
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	 }
      }	 

      AddHelpParagraph(lwoh, 
		       _("<b>Medium ejection</b>\n\n"
			 "Activate this option to have the medium ejected after "
			 "a successful read or scan operation.\n\n"
			 "Note that the desktop environment "
			 "may prevent other applications from ejecting media. "
			 "In that case eject the medium through the desktop "
			 "user interface."));

      /*** "Read attempts" page */

      vbox = create_page(notebook, _utf("Read attempts"));

      /** Reading preferences */
      
      frame = gtk_frame_new(_utf("Sector read errors"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* Raw verify */

      lwoh = CreateLabelWithOnlineHelp(_("Raw reading"), _("Read and analyze raw sectors"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);

	 button = gtk_check_button_new();
	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
       	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

 	 if(!i) pc->rawButtonA = button;
	 else   pc->rawButtonB = button;

         activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->readRaw);
         g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_RAW));

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, hbox);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Raw reading</b> (affects CD media only)\n\n"
			 "Activating this option has several effects:\n\n"
			 "C2 quality scanning will be performed when supported "
			 "by the drive.\n\n"
			 "Media sectors are read in raw mode. The L-EC P/Q "
			 "vectors, EDC checksum and MSF address contained "
			 "in the raw data are checked to make sure that the "
			 "sector was correctly read.\n\n"
			 "Additional data recovery heuristics and raw sector "
			 "caching becomes available if either\n"
			 "- adaptive reading is used, or\n"
			 "- linear reading is configured to skip 0 sectors after a read error.\n"
			 "Raw sector caching also needs checking of the respective option."
			 ));

      /* Minimum reading attempts */

      lwoh = CreateLabelWithOnlineHelp(_("Minimum number of reading attempts"), "ignore");
      RegisterPreferencesHelpWindow(lwoh);
      LockLabelSize(GTK_LABEL(lwoh->normalLabel), _utf("Min. %d reading attempts per sector"), 99);
      LockLabelSize(GTK_LABEL(lwoh->linkLabel), _utf("Min. %d reading attempts per sector"), 99);

      pc->minAttemptsScaleLwoh = lwoh;
      pc->minAttemptsScaleInfoA = g_malloc0(sizeof(non_linear_info));
      pc->minAttemptsScaleInfoB = g_malloc0(sizeof(non_linear_info));
      pc->minAttemptsScaleInfoA->format = g_strdup(_("Min. %d reading attempts per sector"));
      pc->minAttemptsScaleInfoB->format = g_strdup(_("Min. %d reading attempts per sector"));

      pc->minAttemptsScaleInfoA->lwoh = lwoh;

      for(i=0; i<2; i++)
	{  GtkWidget *scale,*scale_box;

	 scale = non_linear_scale(&scale_box, 
				  i ? pc->minAttemptsScaleInfoB : pc->minAttemptsScaleInfoA,
				  i ? lwoh->normalLabel : lwoh->linkLabel, 
				  pc, SLIDER_MIN_READ_ATTEMPTS, min_attempts_values, MIN_ATTEMPTS_VALUE_LENGTH,
				  Closure->minReadAttempts);

	 if(!i) pc->minAttemptsScaleA = scale;
	 else   pc->minAttemptsScaleB = scale;

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), scale_box, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, scale_box);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Minimum number of reading attempts</b>\n\n"
			 "If an unreadable sector is encountered, "
			 "dvdisaster tries to re-read it the given number of times.\n\n" 
			 "Increasing the number of reading attempts may improve data recovery "
			 "on marginal media, but will also increase processing time and "
			 "mechanical wear on the drive."));

      /* Maximum reading attempts */

      lwoh = CreateLabelWithOnlineHelp(_("Maximum number of reading attempts"), "ignore");
      RegisterPreferencesHelpWindow(lwoh);
      LockLabelSize(GTK_LABEL(lwoh->normalLabel), _utf("Max. %d reading attempts per sector"), 100);
      LockLabelSize(GTK_LABEL(lwoh->linkLabel), _utf("Max. %d reading attempts per sector"), 100);

      pc->maxAttemptsScaleLwoh = lwoh;
      pc->maxAttemptsScaleInfoA = g_malloc0(sizeof(non_linear_info));
      pc->maxAttemptsScaleInfoB = g_malloc0(sizeof(non_linear_info));
      pc->maxAttemptsScaleInfoA->format = g_strdup(_("Max. %d reading attempts per sector"));
      pc->maxAttemptsScaleInfoB->format = g_strdup(_("Max. %d reading attempts per sector"));

      pc->maxAttemptsScaleInfoA->lwoh = lwoh;

      for(i=0; i<2; i++)
	{  GtkWidget *scale,*scale_box;

	 scale = non_linear_scale(&scale_box, 
				  i ? pc->maxAttemptsScaleInfoB : pc->maxAttemptsScaleInfoA,
				  i ? lwoh->normalLabel : lwoh->linkLabel, 
				  pc, SLIDER_MAX_READ_ATTEMPTS, max_attempts_values, MAX_ATTEMPTS_VALUE_LENGTH,
				  Closure->maxReadAttempts);

	 if(!i) pc->maxAttemptsScaleA = scale;
	 else   pc->maxAttemptsScaleB = scale;

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), scale_box, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, scale_box);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Maximum number of reading attempts</b>\n\n"
			 "When the minimum number of reading attempts is reached "
			 "without success, dvdisaster might choose to perform additional "
			 "reading attempts upto this number.\n\n"

			 "The decision to do more attempts depends on the quality of "
			 "data gathered so far, which in turn is influenced by the "
			 "capabilities of your CD/DVD drive and the operating system. "
			 "So depending on your configuration, you may or "
			 "may not see dvdisaster using the maximum value."
			 ));

      /* Jump selector */

      lwoh = CreateLabelWithOnlineHelp(_("Treatment of unreadable areas"), "ignore");
      RegisterPreferencesHelpWindow(lwoh);
      if(  GetLabelWidth(GTK_LABEL(lwoh->normalLabel), _utf("Skip %d sectors after read error"), 20480)
	 > GetLabelWidth(GTK_LABEL(lwoh->normalLabel), _utf("Stop reading when unreadable intervals &lt; %d"), 20480))
      {    LockLabelSize(GTK_LABEL(lwoh->normalLabel), _utf("Skip %d sectors after read error"), 20480);
	   LockLabelSize(GTK_LABEL(lwoh->linkLabel), _utf("Skip %d sectors after read error"), 20480);
      }
      else 
      {    LockLabelSize(GTK_LABEL(lwoh->normalLabel), _utf("Stop reading when unreadable intervals &lt; %d"), 20480);
	   LockLabelSize(GTK_LABEL(lwoh->linkLabel), _utf("Stop reading when unreadable intervals &lt; %d"), 20480);
      }

      pc->jumpScaleLwoh = lwoh;
      pc->formatLinear = g_strdup(_("Skip %d sectors after read error"));
      pc->formatAdaptive  = g_strdup(_("Stop reading when unreadable intervals &lt; %d"));

      pc->jumpScaleInfoA = g_malloc0(sizeof(non_linear_info));
      pc->jumpScaleInfoB = g_malloc0(sizeof(non_linear_info));
      pc->jumpScaleInfoA->format = Closure->adaptiveRead ? pc->formatAdaptive : pc->formatLinear;
      pc->jumpScaleInfoB->format = Closure->adaptiveRead ? pc->formatAdaptive : pc->formatLinear;

      pc->jumpScaleInfoA->lwoh = lwoh;

      for(i=0; i<2; i++)
      {  GtkWidget *scale, *scale_box;
 
	 scale = non_linear_scale(&scale_box, 
				  i ? pc->jumpScaleInfoB : pc->jumpScaleInfoA, 
				  i ? lwoh->normalLabel : lwoh->linkLabel, 
				  pc, SLIDER_JUMP, jump_values, JUMP_VALUE_LENGTH,
				  Closure->sectorSkip);

	 if(!i) pc->jumpScaleA = scale;
	 else   pc->jumpScaleB = scale;

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), scale_box, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, scale_box);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Treatment of unreadable areas</b>\n\n"
			 "Defective media usually contain numerous read errors in a contigous region. "
			 "Skipping sectors after a read error reduces the processing time and the "
			 "mechanical wear on the drive, but leaves larger gaps in the image file.\n\n"
			 "Effects on the <b>linear reading strategy</b>:"));

      AddHelpListItem(lwoh, 
		       _("Skipping a large number of sectors (e.g. 1024) gives a quick overview of "
			 "damaged areas, but will usually not collect enough data for repairing the image."));
      
      AddHelpListItem(lwoh, 
		      _("Smaller values like 16, 32 or 64 are a good trade-off: The processing time will be"
			 "considerably shortened, but still enough data for repairing the image is collected.\n"));

      AddHelpParagraph(lwoh, 
		       _("The <b>adaptive reading strategy</b> uses this setting only if no error correction data "
			 "is available. In that case the reading process will stop when no unread areas "
			 "larger than the selected size remain. Values smaller than 128 <i>are not recommended</i> "
			 "as they cause the drive to carry out lots of laser head repositioning during the "
			 "final phase of the reading process. If adaptive reading with a setting of 128 is not "
			 "sufficient, try reading the remaining sectors with an additional linear reading pass.\n\n"

			 "On DVD and BD media read errors do usually extend over at least 16 sectors for technical "
			 "reasons. Therefore selecting a value less than 16 is not recommended for DVD and BD."
			 ));

      /** Media re-reads */
      
      frame = gtk_frame_new(_utf("Media read attempts"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      lwoh = CreateLabelWithOnlineHelp(_("Media read attempts"), 
				       _("Read the whole medium "));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *spin;
	 GtkWidget *label;

      	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);
	 spin = gtk_spin_button_new_with_range(1, 200, 1);
	 gtk_entry_set_width_chars(GTK_ENTRY(spin), 3);
	 gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), 
				   Closure->readingPasses < 2 ? 1 : Closure->readingPasses);
	 g_signal_connect(spin, "value-changed", G_CALLBACK(spin_cb), (gpointer)SPIN_READ_MEDIUM);
	 gtk_box_pack_start(GTK_BOX(hbox), spin, FALSE, FALSE, 0);
	 label = gtk_label_new(_utf(" times"));
	 gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	 if(!i)
	 {  pc->readMediumA = spin;
	    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
	    gtk_container_add(GTK_CONTAINER(frame), hbox);
	 }
	 else
	 {  pc->readMediumB = spin;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Media read attempts</b> for the linear reading strategy\n\n"
			 "If unreadable sectors remain after reading the medium from start to end, "
			 "the medium is read again upto he given number of times.\n\n"
			 "Only the missing sectors will be tried in the additional reading passes."));

      /** Defective sector caching */
      
      frame = gtk_frame_new(_utf("Raw sector caching"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* Toggle button */

      lwoh = CreateLabelWithOnlineHelp(_("Raw sector caching"), 
				       _("Keep uncorrectable raw sectors in the following directory:"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *table = gtk_table_new(3,2,FALSE);
	 GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *label = gtk_label_new(Closure->dDumpDir);
	 GtkWidget *select = gtk_button_new_with_label(_utf("Select"));

	 button = gtk_check_button_new();
	 gtk_table_attach(GTK_TABLE(table), button, 
			  0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
	 gtk_table_attach(GTK_TABLE(table), i ? lwoh->normalLabel : lwoh->linkBox,
			  1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	 gtk_misc_set_alignment(GTK_MISC(lwoh->linkLabel), 0.0, 0.0);
	 gtk_misc_set_alignment(GTK_MISC(lwoh->normalLabel), 0.0, 0.0);

	 hbox = gtk_hbox_new(FALSE, 0);
	 gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	 gtk_table_attach(GTK_TABLE(table), hbox, 
			  1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

	 gtk_table_attach(GTK_TABLE(table), select, 
			  2, 3, 0, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
	 g_signal_connect(G_OBJECT(select), "clicked", G_CALLBACK(cache_defective_dir_cb), pc);


 	 if(!i) 
	 {    pc->cacheDefectiveA = button;
	      pc->cacheDefectiveDirA = label;
	 }
	 else
	 {    pc->cacheDefectiveB = button;
	      pc->cacheDefectiveDirB = label;
	 }

	 if(Closure->readRaw && Closure->defectiveDump)
	      activate_toggle_button(GTK_TOGGLE_BUTTON(button), TRUE);
	 else activate_toggle_button(GTK_TOGGLE_BUTTON(button), FALSE);
         g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_CACHE_DEFECTIVE));

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), table, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, table);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Raw sector caching</b>\n\n"
			 "Some drives are capable of delivering partial data from defective "
			 "sectors. While one partial sector is useless by itself, "
			 "20 or more of them might combine into a complete sector.\n\n"
			 "If you activate this button, dvdisaster will collect partial "
			 "sectors in the selected directory. "
			 "When enough parts have been gathered in subsequent reading passes, "
			 "the respective sector is automatically recombined from the "
			 "stored parts.\n\n"
			 "Please note that not all drives and operating systems "
			 "support reading partial data. "
			 "It is not an error of the cache directory remains empty.\n"
			 "dvdisaster will not remove any files from the given directory; "
			 "you need to clean it up manually after a successful medium recovery."
			 ));

      /* Sector cache file prefix */

      lwoh = CreateLabelWithOnlineHelp(_("Raw sector file prefix"), 
				       _("Raw sector file prefix: "));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *entry = gtk_entry_new();

       	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);
	 g_signal_connect(entry, "activate", G_CALLBACK(defective_prefix_cb), pc);
	 gtk_entry_set_width_chars(GTK_ENTRY(entry), 20);
	 gtk_entry_set_text(GTK_ENTRY(entry), Closure->dDumpPrefix);
	 gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);

 	 if(!i) pc->cacheDefectivePrefixA = entry;
	 else   pc->cacheDefectivePrefixB = entry;

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, hbox);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Raw sector file prefix</b>\n\n"
			 "Use a different prefix for each disk you are trying "
			 "to recover, e.g. \"disk1-\" and so on."));

      /*** "Error correction" page */

      /* Method chooser menu */

      vbox = create_page(notebook, _utf("Error correction"));

      lwoh = CreateLabelWithOnlineHelp(_("Error correction method"), 
				       _("Storage method:"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 4);
	 GtkWidget *chooser;
	 int j;

	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 chooser = gtk_combo_box_new_text();

       	 g_signal_connect(G_OBJECT(chooser), "changed", G_CALLBACK(method_select_cb), pc);

	 for(j=0; j<Closure->methodList->len; j++)
	 {  Method *method = g_ptr_array_index(Closure->methodList, j);

	    gtk_combo_box_append_text(GTK_COMBO_BOX(chooser), method->menuEntry); 

	    if(!strncmp(Closure->methodName, method->name, 4))
	      method_idx = j;
	 }

	 gtk_combo_box_set_active(GTK_COMBO_BOX(chooser), method_idx);
	 gtk_box_pack_start(GTK_BOX(hbox), chooser, FALSE, FALSE, 0);
	 
	 if(!i)
	 {  pc->methodChooserA = chooser;
	    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	 }
	 else
	 {  pc->methodChooserB = chooser;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, _("<b>Error correction method</b>\n\n"
			       "dvdisaster creates error correction data which is used to recover "
			       "unreadable sectors if the disc becomes damaged later on. There are "
			       "two different ways available for storing the error correction "
			       "information:\n"));

      AddHelpListItem(lwoh, _("Error correction files (RS01 method)\n"
			      "Error correction files are the only way of protecting existing media "
			      "as they can be stored somewhere else. They are kept on a separate "
			      "medium which must also be protected by dvdisaster, as data loss in "
			      "an error correction file will render it unusable.\n"));


      AddHelpListItem(lwoh, _("Augmented images (RS02 method)\n"
			      "The error correction data will be stored along with the user data on the "
			      "same CD/DVD. This requires the creation of an image file prior to writing the "
			      "medium. The error correction data will be appended to that image. " 
			      "Damaged sectors in the error correction "
			      "information reduce the data recovery capacity, but do not make recovery "
			      "impossible - a second medium for keeping or protecting the error correction "
			      "information is not required."));


      /* sub pages for individual method configuration */
      
      pc->methodNotebook = gtk_notebook_new();
      gtk_notebook_set_show_tabs(GTK_NOTEBOOK(pc->methodNotebook), FALSE);
      gtk_notebook_set_show_border(GTK_NOTEBOOK(pc->methodNotebook), FALSE);
      gtk_box_pack_start(GTK_BOX(vbox), pc->methodNotebook, TRUE, TRUE, 0);

      for(i=0; i<Closure->methodList->len; i++)
      {  Method *method = g_ptr_array_index(Closure->methodList, i);
	 GtkWidget *vbox2 = gtk_vbox_new(FALSE, 0);
	 GtkWidget *ignore = gtk_label_new("method_tab");

	 if(method->createPrefsPage)
	    method->createPrefsPage(method, vbox2);
	 else
	 {  GtkWidget *lab;

	    lab = gtk_label_new("This method has no configuration options.");
	    gtk_box_pack_start(GTK_BOX(vbox2), lab, TRUE, TRUE, 0);
	 }

	 gtk_notebook_append_page(GTK_NOTEBOOK(pc->methodNotebook), vbox2, ignore);
	 if(i==method_idx)
      gtk_notebook_set_current_page(GTK_NOTEBOOK(pc->methodNotebook), method_idx);
      }

      g_idle_add(notebook_idle_func, pc);

      /*** "Files" page */

      vbox = create_page(notebook, _utf("Files"));

      /* file extension */

      frame = gtk_frame_new(_utf("Local files (on hard disc)"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 15);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      lwoh = CreateLabelWithOnlineHelp(_("Automatic file suffixes"), _("Automatically add .iso and .ecc file suffixes"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->autoSuffix);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_SUFFIX));

	 if(!i)
	 {  pc->suffixA = button;
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 }
	 else
	 {  pc->suffixB = button;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Automatically add file suffixes</b>\n\n"
			 "When this switch is set, files will be automatically appended with \".iso\" "
			 "or \".ecc\" suffixes if no other file name extension is already present."));

      /* 2GB button */

      lwoh = CreateLabelWithOnlineHelp(_("File splitting"), _("Split files into segments &lt;= 2GB"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);
	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->splitFiles);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_2GB));

	 if(!i)
	 {  pc->splitA = button;
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 }
	 else
	 {  pc->splitB = button;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>File splitting</b>\n\n"
			 "Allows working with file systems which are limited to 2GB per file, e.g. "
			 "FAT from Windows. Created files are spread over upto 100 segments "
			 "called \"medium00.iso\", \"medium01.iso\" etc. at the cost of a small "
			 "performance hit."));


      /*** Automatic file creation and deletion */

      frame = gtk_frame_new(_utf("Automatic file creation and deletion"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 15);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* automatic creation */

      lwoh = CreateLabelWithOnlineHelp(_("Automatic .ecc file creation"), _("Create error correction file after reading image"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
      	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);
	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->readAndCreate);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_READ_CREATE));

	 if(!i)
	 {  pc->readAndCreateButtonA = button;
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 }
	 else
	 {  pc->readAndCreateButtonB = button;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Automatic error correction file creation</b>\n\n"
			 "Automatically creates an error correction file after reading an image. "
			 "Together with the \"Remove image\" option this will speed up error correction "
			 "file generation for a series of different media."));

      /* automatic deletion */

      lwoh = CreateLabelWithOnlineHelp(_("Automatic image file removal"), _("Remove image after error correction file creation"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
      	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);
	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->unlinkImage);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_UNLINK));

	 if(!i)
	 {  pc->unlinkImageButtonA = button;
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 }
	 else
	 {  pc->unlinkImageButtonB = button;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Automatic image file removal</b>\n\n"
			 "If this switch is set the image file will be deleted following the successful "
			 "generation of the respective error correction file."));

      /*** GUI page */

      vbox = create_page(notebook, _utf("Appearance"));

      /** Color scheme
          Using a table gives better control over spacing between the frames. */

      table = gtk_table_new(2, 4, FALSE);
      gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);

      vbox3 = gtk_vbox_new(FALSE, 0);
      gtk_table_attach(GTK_TABLE(table), vbox3, 
		       0, 1, 0, 3, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 5, 5);

      frame = gtk_frame_new(_utf("Sector coloring"));
      gtk_box_pack_start(GTK_BOX(vbox3), frame, FALSE, FALSE, 0);

      gtk_box_pack_start(GTK_BOX(vbox3), gtk_label_new(NULL), TRUE, TRUE, 0);

      vbox2 = gtk_vbox_new(FALSE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);
 
      /* Green color */

      lwoh = CreateLabelWithOnlineHelp(_("Good sectors"), _("Good sector"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->greenSector, COLOR_GREEN);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->greenA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->greenB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Good sectors</b>\n\n"
		         "This color indicates good sectors."));

      /* Yellow color */

      lwoh = CreateLabelWithOnlineHelp(_("Checksum errors"), _("Checksum error"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->yellowSector, COLOR_YELLOW);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->yellowA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->yellowB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Checksum errors</b>\n\n"
		         "This color is used for displaying sectors with wrong check sums."));

      /* Red color */

      lwoh = CreateLabelWithOnlineHelp(_("Unreadable sectors"), _("Unreadable"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->redSector, COLOR_RED);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->redA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->redB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Unreadable sectors</b>\n\n"
		         "This color is used for marking unreadable sectors."));

      /* Dark color */

      lwoh = CreateLabelWithOnlineHelp(_("Present sectors"), _("Present sector"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->darkSector, COLOR_DARK);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->darkA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->darkB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Present sectors</b>\n\n"
		         "Sectors which are already present are marked with this color."));

      /* Blue color */

      lwoh = CreateLabelWithOnlineHelp(_("Ignored sectors"), _("Ignored sector"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->blueSector, COLOR_BLUE);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->blueA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->blueB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Ignored sectors</b>\n\n"
		         "Sectors marked with this color will not be processed "
			 "in the current run."));

      /* White color */

      lwoh = CreateLabelWithOnlineHelp(_("Highlit sectors"), _("Highlit sector"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->whiteSector, COLOR_WHITE);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->whiteA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->whiteB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Highlit sectors</b>\n\n"
		         "This color is used for temporarily highlighting "
			 "sectors during adaptive reading."));

      /** Text colors */

      frame = gtk_frame_new(_utf("Text colors"));
      gtk_table_attach(GTK_TABLE(table), frame, 
		       1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 5, 5);

      vbox2 = gtk_vbox_new(FALSE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* Positive text */

      lwoh = CreateLabelWithOnlineHelp(_("Positive text"), _("Positive text"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->greenText, COLOR_GREEN_TEXT);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->greenTextA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->greenTextB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Positive text</b>\n\n"
		         "Good news are printed in this color."));

      /* Positive text */

      lwoh = CreateLabelWithOnlineHelp(_("Negative text"), _("Negative text"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->redText, COLOR_RED_TEXT);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->redTextA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->redTextB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Negative text</b>\n\n"
		         "Bad news are printed in this color."));

      /** Curve colors */

      frame = gtk_frame_new(_utf("Curve colors"));
      gtk_table_attach(GTK_TABLE(table), frame, 
		       1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 5, 5);
      vbox2 = gtk_vbox_new(FALSE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      /* Reading speed curve */

      lwoh = CreateLabelWithOnlineHelp(_("Curve color"), _("Curve color"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->curveColor, COLOR_CURVE);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->curveColorA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->curveColorB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Curve color and labels</b>\n\n"
		         "The reading speed curve, its left side and top labels "
			 "are printed in this color."));

      /* Logarithmic scale (C2 errors) */

      lwoh = CreateLabelWithOnlineHelp(_("C2 errors"), _("C2 errors"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->logColor, COLOR_LOG);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->logColorA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->logColorB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>C2 error color</b>\n\n"
		         "The logarithmic bar graph showing the C2 errors "
			 "is rendered in this color during the \"read\" "
			 "and \"scan\" operations."));

      /* Error correction load */

      lwoh = CreateLabelWithOnlineHelp(_("Error correction load"), _("Error correction load"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox  = gtk_hbox_new(FALSE, 4);
	 color_button_info *cbi;

	 cbi = create_color_button(Closure->barColor, COLOR_BAR);
	 gtk_box_pack_start(GTK_BOX(hbox), cbi->button, FALSE, FALSE, 0);

	 if(!i)
	 {  gtk_box_pack_start(GTK_BOX(hbox), lwoh->linkBox, FALSE, FALSE, 0);
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	    pc->barColorA = cbi;
	 }
	 else
	 {  
	    gtk_box_pack_start(GTK_BOX(hbox), lwoh->normalLabel, FALSE, FALSE, 0);
	    AddHelpWidget(lwoh, hbox);
	    pc->barColorB = cbi;
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Error correction load</b>\n\n"
		         "The bar graph showing the error correction load "
			 "is rendered in this color during the \"Fix\" operation."));

      /* Padding space */
#if 0
      lab = gtk_label_new("");
      gtk_table_attach(GTK_TABLE(table), lab, 
		       1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 5, 5);
#endif
      /* Default color scheme */

      button = gtk_button_new_with_label(_utf("Default color scheme"));
      gtk_table_attach(GTK_TABLE(table), button, 
		       1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 5, 5);

      g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(default_color_cb), NULL);

      /** Reverse OK and Cancel buttons */

      frame = gtk_frame_new(_utf("Dialog boxes"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      lwoh = CreateLabelWithOnlineHelp(_("Reverse OK / Cancel buttons"), _("Reverse OK / Cancel buttons"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
      	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);
	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->reverseCancelOK);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_CANCEL_OK));

	 if(!i)
	 {  pc->cancelOKA = button;
	    gtk_container_add(GTK_CONTAINER(frame), hbox);
	 }
	 else
	 {  pc->cancelOKB = button;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Reverse OK / Cancel buttons</b>\n\n"
			 "This switch reverses the order of dialog buttons "
			 "(e.g. OK, Cancel).\n\n"
			 "Changes will become active after restarting dvdisaster."));

      /*** "Misc" page */

      vbox = create_page(notebook, _utf("Misc"));

      /** Logging **/

      frame = gtk_frame_new(_utf("Logging"));
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

      vbox2 = gtk_vbox_new(FALSE, 15);
      gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
      gtk_container_add(GTK_CONTAINER(frame), vbox2);

      lwoh = CreateLabelWithOnlineHelp(_("Verbose logging"), _("Verbose logging"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *button = gtk_check_button_new();

	 gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	 gtk_box_pack_start(GTK_BOX(hbox), i ? lwoh->normalLabel : lwoh->linkBox, FALSE, FALSE, 0);

	 activate_toggle_button(GTK_TOGGLE_BUTTON(button), Closure->verbose);
	 g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_VERBOSE));

	 if(!i)
	 {  pc->verboseA = button;
	    gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
	 }
	 else
	 {  pc->verboseB = button;
	    AddHelpWidget(lwoh, hbox);
	 }
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Verbose logging</b>\n\n"
			 "More information will be supplied in the Log window "
			 "and/or log file. Useful for debugging, but may lead "
			 "to slower performance."));

      /** Log file */

      lwoh = CreateLabelWithOnlineHelp(_("Logfile:"), 
				       _("Copy log to file:"));
      RegisterPreferencesHelpWindow(lwoh);

      for(i=0; i<2; i++)
      {  GtkWidget *table = gtk_table_new(4,2,FALSE);
	 GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	 GtkWidget *label = gtk_label_new(Closure->logFile);
	 GtkWidget *select = gtk_button_new_with_label(_utf("Select"));
	 GtkWidget *delete = gtk_button_new_with_label(_utf("Delete"));

	 button = gtk_check_button_new();
	 gtk_table_attach(GTK_TABLE(table), button, 
			  0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
	 gtk_table_attach(GTK_TABLE(table), i ? lwoh->normalLabel : lwoh->linkBox,
			  1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	 gtk_misc_set_alignment(GTK_MISC(lwoh->linkLabel), 0.0, 0.0);
	 gtk_misc_set_alignment(GTK_MISC(lwoh->normalLabel), 0.0, 0.0);
	 gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);

	 hbox = gtk_hbox_new(FALSE, 0);
	 gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	 gtk_table_attach(GTK_TABLE(table), hbox, 
			  1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

	 gtk_table_attach(GTK_TABLE(table), select, 
			  2, 3, 0, 2, GTK_SHRINK, GTK_SHRINK, 10, 0);
	 g_signal_connect(G_OBJECT(select), "clicked", G_CALLBACK(logfile_cb), 
			  GINT_TO_POINTER(LOGFILE_SELECT));

	 gtk_table_attach(GTK_TABLE(table), delete, 
			  3, 4, 0, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
	 g_signal_connect(G_OBJECT(delete), "clicked", G_CALLBACK(logfile_cb),
			  GINT_TO_POINTER(LOGFILE_DELETE));


 	 if(!i) 
	 {    pc->logFileA = button;
	      pc->logFilePathA = label;
	 }
	 else
	 {    pc->logFileB = button;
	      pc->logFilePathB = label;
	 }

	 if(Closure->verbose && Closure->logFileEnabled)
	      activate_toggle_button(GTK_TOGGLE_BUTTON(button), TRUE);
	 else activate_toggle_button(GTK_TOGGLE_BUTTON(button), FALSE);
         g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(toggle_cb), GINT_TO_POINTER(TOGGLE_LOGFILE));

	 if(!i) gtk_box_pack_start(GTK_BOX(vbox2), table, FALSE, FALSE, 0);
	 else   AddHelpWidget(lwoh, table);
      }

      AddHelpParagraph(lwoh, 
		       _("<b>Logfile</b>\n\n"
			 "A copy of the logging information from the log window "
			 "is written to the specified log file. This is useful to "
			 "collect information on program crashes, but affects "
			 "performance negatively."));
   }

   /* Show the created / reused window */

   gtk_widget_show_all(GTK_WIDGET(Closure->prefsWindow));
}