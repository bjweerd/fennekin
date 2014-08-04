#ifndef INCLUDED_FENNEKIN_TREEMODEL_H
#define INCLUDED_FENNEKIN_TREEMODEL_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gtk/gtk.h>

/* GObject standard macro's */

#define FENNEKIN_TREEMODEL_TYPE                    (fennekin_treemodel_get_type ())
#define FENNEKIN_TREEMODEL(obj)                      (G_TYPE_CHECK_INSTANCE_CAST ((obj), FENNEKIN_TREEMODEL_TYPE, FennekinTreemodel))
#define FENNEKIN_TREEMODEL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass),    FENNEKIN_TREEMODEL_TYPE, FennekinTreemodelClass))
#define FENNEKIN_IS_TREEMODEL(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE ((obj),  FENNEKIN_TREEMODEL_TYPE))
#define FENNEKIN_IS_TREEMODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),     FENNEKIN_TREEMODEL_TYPE))
#define FENNEKIN_TREEMODEL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),   FENNEKIN_TREEMODEL_TYPE, FennekinTreemodelClass))

typedef struct _FennekinTreemodel FennekinTreemodel;
typedef struct _FennekinTreemodelClass FennekinTreemodelClass;

struct _FennekinTreemodel
{
  GObject parent;		/* this MUST be the first member */

  // my data goes here...
};

// standard GObject class
struct _FennekinTreemodelClass
{
  GObjectClass parent_class;
};

// expose these functions

GType fennekin_treemodel_get_type(void);
FennekinTreemodel* fennekin_treemodel_new(void);

#endif

