#include <fennekin_treemodel.h>


#include <stack>
#include <deque>

namespace fennekin
{

  // These functions are used in 'fennekin_treemode_get_type()'

  static void fennekin_treemodel_init(FennekinTreemodel* pkg_tree);
  static void fennekin_treemodel_class_init(FennekinTreemodelClass* klass);
  static void fennekin_treemodel_tree_model_init (GtkTreeModelIface* iface);
  static void fennekin_treemodel_finalize(GObject* object); // the class destructor

  static GObjectClass *parent_class = NULL;

  //
  // Implementation of basic functions used in 'fennekin_treemode_get_type()'
  //

  // Init callback for the type system, called once when our new class is created.
  static void fennekin_treemodel_class_init(FennekinTreemodelClass* klass)
  {
    GObjectClass *object_class;
    
    parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
    object_class = (GObjectClass*) klass;
    
    object_class->finalize = fennekin_treemodel_finalize;
  }
  
  // This is called when an instance is created
  static void fennekin_treemodel_init(FennekinTreemodel* treemodel)
  {
    treemodel->stamp = g_random_int();
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
  // Implement the interface member functions
  //

  static GtkTreeModelFlags fennekin_treemodel_get_flags(GtkTreeModel* tree_model)
  {
    // we're not a list and our iterators might not survive signals. so let's return 0;
    return static_cast<GtkTreeModelFlags>(0);
  }
  static gint fennekin_treemodel_get_n_columns(GtkTreeModel* tree_model)
  {
    return 1;			// yep, we have only one column
  }
  static GType fennekin_treemodel_get_column_type(GtkTreeModel* tree_model, gint index)
  {
    g_assert(index == 0);		// because we have only one column
    return G_TYPE_STRING;
  }
  static gboolean fennekin_treemodel_get_iter(GtkTreeModel* tree_model, GtkTreeIter* iter, GtkTreePath* path)
  {
    FennekinTreemodel* fennekin_treemodel = FENNEKIN_TREEMODEL(tree_model);
    TreeModelNode* root = Application::app->doc->root;

    // the length of the 'indices' array is 'depth'.
    gint* indices = gtk_tree_path_get_indices(path);
    gint depth = gtk_tree_path_get_depth(path);

    // TreeModelNodes are MIXED DATA so we will not only have to walk the tree, we also have to keep
    // checking the node is of the right type. There could only be search engines and no search terms!

    TreeModelNode* p = root, *q = NULL;

    for (int i = 0; i < depth; i++)
      {
	gint pos = indices[i], n = 0;
	q = p;

	while (n < pos)
	  {
	    if (q == NULL) break;
	    if (q->node_type == TreeModelNode::search_term) {
	      ++n;
	    }
	    q = q->next;
	  }
	if (n != pos) return FALSE;
	p = q->children;
      }

    if (!q) return FALSE;

    // now 'q' holds the pointer we want
    iter->stamp = fennekin_treemodel->stamp;
    iter->user_data = q;
    iter->user_data2 = iter->user_data3 = NULL;

    return TRUE;
  }
  // this struct is for implementing the fennekin_treemodel_get_path() interface member below
  struct fennekin_treemodel_get_path_algo {
    std::stack<gint> stack;

    GtkTreePath* path;
    TreeModelNode* target_node;
    
    // constructor
    fennekin_treemodel_get_path_algo(GtkTreePath* path, TreeModelNode* target_node)
    {
      this->path = path; this->target_node = target_node;
    }

    // return false if the target_node cannot be found
    bool calculate(TreeModelNode* p)
    {
      stack.push(0);

      while (p) {
	if (p == target_node) {
	  // whoooo! we found it.
	  return true;
	}

	if (p->node_type == TreeModelNode::search_term) {
	  if (p->children)  {
	    if (calculate(p->children) == true) return true;
	  }
	  ++ stack.top();
	}

	p=p->next;
      }

      stack.pop();
      return false;
    }
  };
  static GtkTreePath* fennekin_treemodel_get_path(GtkTreeModel* tree_model, GtkTreeIter* iter)
  {
    FennekinTreemodel* fennekin_treemodel = FENNEKIN_TREEMODEL(tree_model);
    TreeModelNode* root = Application::app->doc->root;
    TreeModelNode* target_node = static_cast<TreeModelNode*>(iter->user_data);

    GtkTreePath* path;
    path = gtk_tree_path_new();

    // OMG This is horrible!
    //
    // To find where target_node is, we basically have zero knowledge and all we can do
    // is walk the entire tree recursively and compare pointers to figure out if we found it.
    // On top of that we need to have the entire tree_path available for that node.
    fennekin_treemodel_get_path_algo algo(path,target_node);
    if (algo.calculate(root) == true)
      {
	// oh, i forgot: std::stack<> has no iterators
	std::deque<gint> tmp;
	while (!algo.stack.empty()) { 
	  tmp.push_front( algo.stack.top() ); 
	  algo.stack.pop(); 
	}

	for (std::deque<gint>::iterator i = tmp.begin(); i != tmp.end(); i++)
	  gtk_tree_path_append_index(path, *i);
      }

    return path;
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
  
  
} // end namespace fennekin



