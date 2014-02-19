<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">


	<xsl:template match="/webdiver">
	<root>	
		<xsl:apply-templates/>
	</root>
	</xsl:template>

	<xsl:template match="term">
	<term>
		<name><xsl:value-of select="@name"/></name>
		<xsl:apply-templates/>
	</term>
	</xsl:template>

	<xsl:template match="engine">
	<engine>
		<name><xsl:value-of select="@name"/></name>
		<query><xsl:value-of select="@query"/></query>
	</engine>
	</xsl:template>
</xsl:stylesheet>

