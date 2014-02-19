<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="/">
	<wdml version="2.1">
		<xsl:apply-templates/>
	</wdml>
	</xsl:template>

	<xsl:template match="treeitem">
		<xsl:text disable-output-escaping="yes">&lt;term name="</xsl:text>
		<xsl:apply-templates/>
		<xsl:text disable-output-escaping="yes">&lt;/term&gt;</xsl:text>
	</xsl:template>
	<xsl:template match="treecell">
		<xsl:value-of select="@label"/>
		<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
		<xsl:apply-templates/>
	</xsl:template>


</xsl:stylesheet>


