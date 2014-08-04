#include <fennekin_treemodel.h>

// These functions are used in fennekin_treemode_get_type()

static void fennekin_treemodel_init(FennekinTreemodel* pkg_tree);
static void fennekin_treemodel_class_init(FennekinTreemodelClass* klass);
static void fennekin_treemodel_tree_model_init (GtkTreeModelIface* iface);
static void fennekin_treemodel_finalize(GObject* object); // the class destructor

static GObjectClass *parent_class = NULL; 

//
// Implementation of basic functions used in fennekin_treemode_get_type()
//

// This is called when an instance is created
static void fennekin_treemodel_init(FennekinTreemodel* treemodel)
{
  // TODO: Initialize member variables
}
 
// Init callback for the type system, called once when our new class is created.
static void fennekin_treemodel_class_init(FennekinTreemodelClass* klass)
{
  GObjectClass *object_class;

  parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
  object_class = (GObjectClass*) klass;

  object_class->finalize = fennekin_treemodel_finalize;
}

// This is called just before a treemodel is destroyed. Free dynamically allocated memory here.
static void fennekin_treemodel_finalize(GObject* object)
{
  // TODO: free all memory used by the object

  (* parent_class->finalize) (object); // must chain up - finalize parent
}


//
// Public functions
//

GType fennekin_treemodel_get_type(void)
{
  static GType fennekin_treemodel_type = 0;

  /* Some boilerplate type registration stuff */
  if (fennekin_treemodel_type == 0)
    {
      static const GTypeInfo fennekin_treemodel_info =
	{
	  sizeof (FennekinTreemodelClass),
	  NULL,                                         /* base_init */
	  NULL,                                         /* base_finalize */
	  (GClassInitFunc) fennekin_treemodel_class_init,
	  NULL,                                         /* class finalize */
	  NULL,                                         /* class_data */
	  sizeof (FennekinTreemodel),
	  0,                                           /* n_preallocs */
	  (GInstanceInitFunc) fennekin_treemodel_init
	};
      static const GInterfaceInfo tree_model_info =
	{
	  (GInterfaceInitFunc) fennekin_treemodel_tree_model_init,
	  NULL,
	  NULL
	};
      
      /* First register the new derived type with the GObject type system */
      fennekin_treemodel_type = g_type_register_static (G_TYPE_OBJECT,  "FennekinTreemodel", 
							&fennekin_treemodel_info, (GTypeFlags)0);
      
      /* Now register our GtkTreeModel interface with the type system */
      g_type_add_interface_static (fennekin_treemodel_type, GTK_TYPE_TREE_MODEL, &tree_model_info);
    }
  
  return fennekin_treemodel_type;
}

FennekinTreemodel* fennekin_treemodel_new(void)
{
   FennekinTreemodel* new_treemodel;

  new_treemodel = (FennekinTreemodel*) g_object_new (FENNEKIN_TREEMODEL_TYPE, NULL);

  g_assert( new_treemodel != NULL );

  return new_treemodel;
}


//
// Member functions for the GtkTreeModelIface interface
//

static GtkTreeModelFlags fennekin_treemodel_get_flags(GtkTreeModel* tree_model);
static gint fennekin_treemodel_get_n_columns(GtkTreeModel* tree_model);
static GType fennekin_treemodel_get_column_type(GtkTreeModel* tree_model, gint index);
static gboolean fennekin_treemodel_get_iter(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreePath* path);
static GtkTreePath* fennekin_treemodel_get_path(GtkTreeModel* tree_model, GtkTreeIter* iter);
static void fennekin_treemodel_get_value(GtkTreeModel* tree_model, GtkTreeIter *iter, gint column, GValue* value);
static gboolean fennekin_treemodel_iter_next(GtkTreeModel* tree_model, GtkTreeIter* iter);
static gboolean fennekin_treemodel_iter_children(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreeIter* parent);
static gboolean fennekin_treemodel_iter_has_child(GtkTreeModel* tree_model, GtkTreeIter* iter);
static gint fennekin_treemodel_iter_n_children(GtkTreeModel* tree_model, GtkTreeIter* iter);
static gboolean fennekin_treemodel_iter_nth_child(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreeIter* parent, gint n);
static gboolean fennekin_treemodel_iter_parent(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreeIter* child);

// Assign interface member functions
static void fennekin_treemodel_tree_model_init (GtkTreeModelIface* iface)
{
  iface->get_flags = fennekin_treemodel_get_flags;
  iface->get_n_columns = fennekin_treemodel_get_n_columns;
  iface->get_column_type = fennekin_treemodel_get_column_type;
  iface->get_iter = fennekin_treemodel_get_iter;
  iface->get_path = fennekin_treemodel_get_path;
  iface->get_value = fennekin_treemodel_get_value;
  iface->iter_next = fennekin_treemodel_iter_next;
  iface->iter_children = fennekin_treemodel_iter_children;
  iface->iter_has_child  = fennekin_treemodel_iter_has_child;
  iface->iter_n_children = fennekin_treemodel_iter_n_children;
  iface->iter_parent = fennekin_treemodel_iter_parent;
}

//
// Implement the interface
//

static GtkTreeModelFlags fennekin_treemodel_get_flags(GtkTreeModel* tree_model)
{
}
static gint fennekin_treemodel_get_n_columns(GtkTreeModel* tree_model)
{
}
static GType fennekin_treemodel_get_column_type(GtkTreeModel* tree_model, gint index)
{
}
static gboolean fennekin_treemodel_get_iter(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreePath* path)
{
}
static GtkTreePath* fennekin_treemodel_get_path(GtkTreeModel* tree_model, GtkTreeIter* iter)
{
}
static void fennekin_treemodel_get_value(GtkTreeModel* tree_model, GtkTreeIter *iter, gint column, GValue* value)
{
}
static gboolean fennekin_treemodel_iter_next(GtkTreeModel* tree_model, GtkTreeIter* iter)
{
}
static gboolean fennekin_treemodel_iter_children(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreeIter* parent)
{
}
static gboolean fennekin_treemodel_iter_has_child(GtkTreeModel* tree_model, GtkTreeIter* iter)
{
}
static gint fennekin_treemodel_iter_n_children(GtkTreeModel* tree_model, GtkTreeIter* iter)
{
}
static gboolean fennekin_treemodel_iter_nth_child(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreeIter* parent, gint n)
{
}
static gboolean fennekin_treemodel_iter_parent(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreeIter* child)
{
}






