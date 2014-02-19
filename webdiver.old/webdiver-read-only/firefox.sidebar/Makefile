all : webdiver.xpi

clean :
	rm -f webdiver.xpi chrome/webdiver.jar

webdiver.xpi : clean
	( cd chrome ; zip -r webdiver.jar content/ locale/ -x \*.svn\* \*~ )
	mv chrome.manifest chrome.manifest.tmp
	mv chrome.manifest.jarfile chrome.manifest
	zip webdiver.xpi install.rdf chrome.manifest chrome/webdiver.jar
	mv chrome.manifest chrome.manifest.jarfile
	mv chrome.manifest.tmp chrome.manifest

