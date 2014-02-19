/*_webdiver.js - WebDiver javascript code
 *
 */


var gTreeNodeSelected = false;		// flag for browse_new() to see if a treenode already has been selected


/****************
 *
 * Global functions
 * 
 */

// Get the real 'main' window where this sidebar is running in,
// method used from http://developer.mozilla.org/en/docs/Working_with_windows_in_chrome_code
function getMainWindow()
{
	var mainWindow = window
			 .QueryInterface(Components.interfaces.nsIInterfaceRequestor)
			 .getInterface  (Components.interfaces.nsIWebNavigation)
			 .QueryInterface(Components.interfaces.nsIDocShellTreeItem)
			 .rootTreeItem
			 .QueryInterface(Components.interfaces.nsIInterfaceRequestor)
			 .getInterface  (Components.interfaces.nsIDOMWindow);

	return mainWindow;
}

// point the active browser (tab) to our wanted url
function browseNew()
{
	// return if we don't have a term yet
	if (!gTreeNodeSelected) return;

	// get text of currently selected engine
	var url = queryReplaceTerm(selectQuery(), selectTerm());

	// it took me 6 hours to figure out the line below...
	getMainWindow().content.location.assign(url);
}

// this function performs the replacement in the <engine> query tag
function queryReplaceTerm(query, term)
{
	// replace '$term' and '$encoded_term' in 'query' with 'term'.
	var encoded_term = encodeURI(term);
	var result = query.replace("$encoded_term", encoded_term);

	return result.replace("$term", term);
}


/*****************
 *
 * Menulist event handlers
 * 
 */


function menulistOnCommand()
{
	browseNew();
}


/*****************
 *
 * Tree event handlers
 * 
 */

function treeOnSelect(event)
{
	gTreeNodeSelected = true;
	browseNew();
}

/*****************
 *
 * onFileXxx() functionality
 * 
 */



//
// globals for file menu handling
//


var gIsDataChanged = false;		// 'true' if any data has been changed
var gFileName = "";			// name of the current file

//
// utility functions
//


// Save changes if any data has been changed, return false if
// cancel has been chosen by user.
function saveChanges()
{
	if (gIsDataChanged)
	{
		// from MDC Code snippets
		var prompts = Components.classes["@mozilla.org/embedcomp/prompt-service;1"]
			      .getService(Components.interfaces.nsIPromptService);

		var dummy = {value: false};

		var flags = prompts.BUTTON_POS_0 * prompts.BUTTON_TITLE_SAVE +
			    prompts.BUTTON_POS_1 * prompts.BUTTON_TITLE_DONT_SAVE +
			    prompts.BUTTON_POS_2 * prompts.BUTTON_TITLE_CANCEL;

		var button = prompts.confirmEx(null, "WebDiver",
					       "File changed. Do you want to save your changes?",
					       flags, null,null,null,null,dummy);

		// 'button' indicates which button was clicked
		if (button == 0)	// save
		{
			onFileSave();

			// if the data is still changed the save
			// somehow went wrong and we return false.
			if (gIsDataChanged) return false;
		}
		else if (button == 1)	// don't save
		{
			gIsDataChanged = false;
			return true;
		}
		else			// cancel (or window close)
		{
			return false;
		}
	}

	return true;
}

function disableReload()
{
	// disable 'Reload' menu option
	document.getElementById("idFileReload").setAttribute('disabled','true');
}

function enableReload()
{
	// enable 'Reload' menu option
	document.getElementById("idFileReload").setAttribute('disabled','false');
}

//
// onFileXxx() functions themselves
//

function onFileNew()
{
	if (!saveChanges()) return;

	// Reset everthing
	resetControls();
	disableReload();
	gFileName = "";
	gIsDataChanged = false;

	// TODO: call [File Creation Wizard] here.
}

function onFileOpen()
{
	if (!saveChanges()) return;

	const nsIFilePicker = Components.interfaces.nsIFilePicker;

	var fp = Components.classes["@mozilla.org/filepicker;1"]
		 .createInstance(nsIFilePicker);
	fp.init(window, "Open WebDiver document", nsIFilePicker.modeOpen);
	fp.appendFilters(nsIFilePicker.filterAll | nsIFilePicker.filterXML);

	var rv = fp.show();
	if (rv == nsIFilePicker.returnOK)
	{
		var file = fp.file;
		var path = fp.file.path;
		if (doOpenFile(path))
			addRecentFile();
	}
}

function onFileReload()
{
	if (!saveChanges()) return;

	doOpenFile(gFileName);
}

function onFileSave()
{
	if (gFileName == "")
		onFileSaveAs();
	else
		doSaveFile(gFileName);
}

function onFileSaveAs()
{
	// Essentially the same code as onFileOpen().
	const nsIFilePicker = Components.interfaces.nsIFilePicker;

	var fp = Components.classes["@mozilla.org/filepicker;1"]
		 .createInstance(nsIFilePicker);
	fp.init(window, "Save WebDiver document", nsIFilePicker.modeSave);
	fp.appendFilters(nsIFilePicker.filterAll | nsIFilePicker.filterXML);

	var rv = fp.show();
	if (rv == nsIFilePicker.returnOK || rv == nsIFilePicker.returnReplace)
	{
		var file = fp.file;
		var path = fp.file.path;
		if (doSaveFile(path))
			addRecentFile();
	}
}



