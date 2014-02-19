<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">


	<xsl:template match="/webdiver">
	
		<html xmlsns="http://www.w3.org/199/xhtml">
		<head><title>output</title></head>
		<body>
		<h1>Webdiver related webdiver document:)</h1>

<p>Okay, Here is a dump of the term tree data...</p>


<xsl:apply-templates/>

		</body>
		</html>

	</xsl:template>

	<xsl:template match="term">
	<li><xsl:value-of select="@name"/>
		<ul>
			<xsl:apply-templates/>
		</ul>
	</li>
	</xsl:template>

</xsl:stylesheet>

