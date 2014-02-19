/*******************************************************************************
 *
 * Beta, but stable code
 *
 */


function selectTerm()
{
	// get the text of currently selected item in the tree
	var tree = document.getElementById("outlineTree");
	var term = tree.view.getCellText(tree.currentIndex, tree.columns.getNamedColumn("category"));

	return term;
}

function selectQuery()
{
	// get query for the currently selected engine in the menulist
	var query = "(undefined)";	

	switch (parseInt(document.getElementById("idMenuList").selectedItem.value))
	{
		case 1: query = "http://www.google.com/search?q=$term"; break;
		case 2: query = "http://images.google.com/images?q=$term"; break;
		case 3: query = "http://blogsearch.google.com/blogsearch?q=$term"; break;
		case 4: query = "http://news.google.com/news?ned=us\&q=$term"; break;
		case 5: query = "http://video.google.com/videosearch?q=$term"; break;
		case 6: query = "http://www.youtube.com/results?search_query=$term"; break;
		case 7: query = "http://en.wikipedia.org/wiki/$term"; break;
		case 8: query = "http://flickr.com/search/?q=$term"; break;
		case 9: query = "http://technorati.com/posts/tag/$term"; break;
		case 10:query = "http://www.google.com/search?q=$term site:wikipedia.org"; break;
	}

	return query;
}


/*******************************************************************************
 *
 * BETA CODE PLAYGROUND
 *
 */


function resetControls()
{
	// TODO: clear listbox and treeview
}

// return 'false' if an error occured
function doOpenFile(fileName)
{
	// TODO: real code here
	alert("Loading " + fileName);

	// Update globals to reflect new file info
	enableReload();
	gFileName = fileName;
	gIsDataChanged = false;

	return true;
}

// return 'false' if an error occured.
function doSaveFile(fileName)
{
	// TODO: real code here [C# data.WriteToFile(fileName)]
	alert("Saving " + fileName);

	// Update globals to reflect new file info
	enableReload();
	gFileName = fileName;
	gIsDataChanged = false;

	return true;
}




/**************
 *
 * Persistent data storage
 *
 */

var gRecentFilesCount = 0;
var gRecentFiles = Array();
const gRecentFilesMax = 10;

function addRecentFile()
{
	// TODO: add 'gFileName' on top of the recent file list
	alert("addRecentFile()");

	if (gFileName != "")
	{
		var tmp = Array(); var tmpCount = 0;
		
		tmp[tmpCount++] = gFileName; // add 'gFileName' to top of the list

		for (i = 0; i < gRecentFilesCount; i++)
		{
			if (gRecentFiles[i] != gFileName)
				tmp[tmpCount++] = gRecentFiles[i];

			if (tmpCount >= gRecentFilesMax)
				break;
		}

		menuUpdateRecentFiles();
	}
}


function menuUpdateRecentFiles()
{
	// TODO: gui-update dynamic menu for [Recent Files]
}

//
// Persistent storage and retrieval of application data
//
// see http://piro.sakura.ne.jp/xul/tips/x0007.html
//

function preferencesSave()
{
	var pref = Components.classes['@mozilla.org/preferences;1']
		   .getService(Components.interfaces.nsIPrefBranch);

	// Recent files
}

function preferencesLoad()
{
	var pref = Components.classes['@mozilla.org/preferences;1']
		   .getService(Components.interfaces.nsIPrefBranch);

	// Recent files
}


/***********************************************************/

function webdiverInit()
{
//	alert('webdiverInit()');
}

function webdiverDone()
{
//	alert('webdiverDone()');
}






/****************************************/

function tmpClick()
{
	alert('tmpClick()');
}








