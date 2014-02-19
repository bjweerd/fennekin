
netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");

// ===========================================================================
// eXe
// Copyright 2004-2005, University of Auckland

// Returns the tree item from index which can come from 'tree.getCurrentIndex'
function getOutlineItem(tree, index) {
    // Get the appropriate treeitem element
    // There's a dumb thing with trees in that mytree.currentIndex
    // Shows the index of the treeitem that's selected, but if there is a
    // collapsed branch above that treeitem, all the items in that branch are
    // not included in the currentIndex value, so
    // "var treeitem = mytree.getElementsByTagName('treeitem')[mytree.currentIndex]"
    // doesn't work. We have to do this!
    var mytree = tree
    if (!mytree) { mytree = document.getElementById('outlineTree') }
    var items = mytree.getElementsByTagName('treeitem')
    for (var i=0; i<items.length; i++) {
        if (mytree.contentView.getIndexOfItem(items[i]) == index) {
            return items[i]
        }
    }
    return null // Should never get here
}


// Returns the currently selected tree item.
function currentOutlineItem(tree) {
    // Get the appropriate treeitem element
    var mytree = tree
    if (!mytree) { mytree = document.getElementById('outlineTree') }
    return getOutlineItem(tree, mytree.currentIndex)
}

// Returns the _exe_nodeid attribute of the currently selected row item
function currentOutlineId(index)
{
    var treeitem = currentOutlineItem()
    return treeitem.getElementsByTagName('treerow')[0].getAttribute('_exe_nodeid')
}

function treeDragGesture(event) {
    netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect")
    var tree = document.getElementById('outlineTree') 
    var treeitem = currentOutlineItem(tree)
    // Only allow dragging of treeitems below main
  //  if (tree.view.getIndexOfItem(treeitem) <= 1) { alert("nodrag"); return }
    // Don't start drag because they are moving the scroll bar!
    var row = { }
    var col = { }
    var child = { }
    tree.treeBoxObject.getCellAt(event.pageX, event.pageY, row, col, child)
    if (!col.value) {  return }
    // CRAPINESS ALERT!
    // If they're moving, (without ctrl down) the target node becomes our sibling
    // above us. If copying, the source node becomes the first child of the target node
    var targetNode = getOutlineItem(tree, row.value)
    // Start packaging the drag data (Which we don't use but have to do anyway)
    var data = new Array(treeitem)
    var ds = Components.classes["@mozilla.org/widget/dragservice;1"].getService(Components.interfaces.nsIDragService);
    var trans = Components.classes["@mozilla.org/widget/transferable;1"].createInstance(Components.interfaces.nsITransferable);
    trans.addDataFlavor("text/plain");
    var textWrapper = Components.classes["@mozilla.org/supports-string;1"].createInstance(Components.interfaces.nsISupportsString);
    textWrapper.data = currentOutlineId(); // Get the id of the node bieng dragged
    trans.setTransferData("text/plain", textWrapper, textWrapper.data.length);  // double byte data
    // create an array for our drag items, though we only have one this time
    var transArray = Components.classes["@mozilla.org/supports-array;1"].createInstance(Components.interfaces.nsISupportsArray);
    // Put it into the list as an |nsISupports|
    //var data = trans.QueryInterface(Components.interfaces.nsISupports);
    transArray.AppendElement(trans);
    // Actually start dragging
    ds.invokeDragSession(treeitem, transArray, null, ds.DRAGDROP_ACTION_COPY + ds.DRAGDROP_ACTION_MOVE);
    event.preventBubble(); // This line was in an example, will test if we need it later...
}


function treeDragEnter(event) {
window.status = "treeDragEnter";
    event.preventBubble(); // This line was in an example, will test if we need it later...
    netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect")
    var ds = Components.classes["@mozilla.org/widget/dragservice;1"].getService(Components.interfaces.nsIDragService);
    var ses = ds.getCurrentSession()
    var tree = document.getElementById('outlineTree')
    //tree.treeBoxObject.onDragEnter(event)
    if (ses) { ses.canDrop = 'true' }
}


function DragOverContentArea ( event )
{
  var validFlavor = false;
  var dragSession = null;


    var tree = document.getElementById('outlineTree')
    var row = { }
    var col = { }
    var child = { }
    tree.treeBoxObject.getCellAt(event.pageX, event.pageY, row, col, child)
    var targetNode = getOutlineItem(tree, row.value)
	 document.getElementById("myout").value = targetNode.id;
	 targetNode.style.backgroundColor = "red";
 	 targetNode.style.color = "red";
  
	try {
		netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
	} catch (e) {
		alert("Permission to save file was denied.");
	}
/*	var file = Components.classes["@mozilla.org/file/local;1"]
		.createInstance(Components.interfaces.nsILocalFile);
	var outputStream = Components.classes["@mozilla.org/network/file-output-stream;1"]
		.createInstance( Components.interfaces.nsIFileOutputStream );
*/  
  

//  var dragService =
//    Components.classes["component://netscape/widget/dragservice"].getService(Components.interfaces.nsIDragService);
//     Components.classes["@mozilla.org/netscape/widget/dragservice"].getService(Components.interfaces.nsIDragService);

	var dragService = Components.classes["@mozilla.org/widget/dragservice;1"].
		getService().QueryInterface(Components.interfaces.nsIDragService);
  

  if ( dragService ) {
    dragSession = dragService.getCurrentSession();
    if ( dragSession ) {
      if ( dragSession.isDataFlavorSupported("moz/toolbaritem") )
        validFlavor = true;
      else if ( dragSession.isDataFlavorSupported("text/plain") )
        validFlavor = true;
      //XXX other flavors here...such as files from the desktop?

      if ( validFlavor ) {
        // XXX do some drag feedback here, set a style maybe???

        dragSession.canDrop = true;
        event.preventBubble();
      }
    }
  }
} // DragOverContentArea



function treeDragExit(event) {
    event.preventBubble(); // This line was in an example, will test if we need it later...
    netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect")
    var ds = Components.classes["@mozilla.org/widget/dragservice;1"].getService(Components.interfaces.nsIDragService);
    var ses = ds.getCurrentSession()
    var tree = document.getElementById('outlineTree')
}



function treeDragDrop(event) {
  
    event.preventBubble(); // This line was in an example, will test if we need it later...
    netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect")
    var ds = Components.classes["@mozilla.org/widget/dragservice;1"].getService(Components.interfaces.nsIDragService);
    var ses = ds.getCurrentSession()
    var sourceNode = ses.sourceNode
    var tree = document.getElementById('outlineTree')
    // We'll just get the node id from the source element
    var nodeId = sourceNode.firstChild.getAttribute('_exe_nodeid')
    // Get the new parent node
    var row = { }
    var col = { }
    var child = { }
    tree.treeBoxObject.getCellAt(event.pageX, event.pageY, row, col, child)
    // CRAPINESS ALERT!
    // If they're moving, (without ctrl down) the target node becomes our sibling
    // above us. If copying, the source node becomes the first child of the target node
    var targetNode = getOutlineItem(tree, row.value)
    if (ses.dragAction && ses.DRAGDROP_ACTION_COPY) {
        // Target node is our parent, sourceNode becomes first child
        var parentItem = targetNode
        var sibling = null  // Must be worked out after we get 'container' (treeitems)
        var before = true
    } else {
        // Target node is our sibling, we'll be inserted below (vertically) it on the same tree level
        var parentItem = targetNode.parentNode.parentNode
        var sibling = targetNode
        var before = false
    }

    // Do some sanity checking
    if ((sourceNode == parentItem) || (sourceNode == targetNode)) return;
    var parentItemId = parentItem.firstChild.getAttribute('_exe_nodeid')
    if (sibling && (tree.view.getIndexOfItem(sibling) <= 1)) { return } // Can't drag to top level
    try { if ((parentItem.getElementsByTagName('treechildren')[0].firstChild == sourceNode) && before) { return } // Can't drag into same position
    } catch(e) { } // Ignore when parentItem has no treechildren node
    // Check for recursion
    var node = targetNode.parentNode
    while (node) {
        if (node == sourceNode) { return } // Can't drag into own children
        node = node.parentNode
    }
    // Re-organise the tree...
    // See if parent is a container
    var isContainer = parentItem.getAttribute('container')
    if ((!isContainer) || (isContainer == 'false')) {
        // Make it one
        var container = parentItem.appendChild(document.createElement('treechildren'))
        parentItem.setAttribute('container', 'true')
        parentItem.setAttribute('open', 'true')
    } else {
        var container = parentItem.getElementsByTagName('treechildren')[0]
        // If still haven't got a 'treechildren' node, then make one
        if (!container) {
            var container = parentItem.appendChild(document.createElement('treechildren'))
        }
    }
    // Now we can work out our sibling if we don't already have it
    if (before) { sibling = container.firstChild }
    // Move the node
    var oldContainer = sourceNode.parentNode
    try { oldContainer.removeChild(sourceNode) } catch(e) { } // For some reason works, but still raises exception!
    if (sibling) {  // If the container has children
        // Insert either before or after the sibling
        if (before) {
            if (sibling) {
                container.insertBefore(sourceNode, sibling)
            } else {
                container.appendChild(sourceNode)
            }
        } else {
            // Append after target node
            if (sibling.nextSibling) {
                container.insertBefore(sourceNode, sibling.nextSibling)
            } else {
                container.appendChild(sourceNode)
            }
        }
    } else {
        // Otherwise, just make it be the only child
        container.appendChild(sourceNode)
    }
    // See if the old parent node is no longer a container
    if (oldContainer.childNodes.length == 0) {
        oldContainer.parentNode.setAttribute('container', 'false')
        oldContainer.parentNode.removeChild(oldContainer) // Remove the treechildren node
    }
    // Tell the server what happened
    var nextSiblingNodeId = null
    var sibling = sourceNode.nextSibling
    if (sibling) {
        nextSiblingNodeId = sibling.firstChild.getAttribute('_exe_nodeid')
    }
    nevow_clientToServerEvent('outlinePane.handleDrop', this, '', sourceNode.firstChild.getAttribute('_exe_nodeid'), parentItemId, nextSiblingNodeId)
}
