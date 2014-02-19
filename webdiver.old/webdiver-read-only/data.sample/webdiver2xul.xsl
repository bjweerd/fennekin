<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

   <xsl:output method="xml" indent="yes" encoding="utf-8" />

   <xsl:template match="/webdiver">
      <treechildren>
	 <xsl:apply-templates/>
      </treechildren>
   </xsl:template>
	

   <xsl:template match="term">
      <!-- see if we have child terms -->
      <xsl:choose>
	 <xsl:when test=".//term">
	    <treeitem container="true">
	       <treerow>
		  <xsl:text disable-output-escaping="yes">&lt;treecell label="</xsl:text>
		  <xsl:value-of select="@name"/>
		  <xsl:text disable-output-escaping="yes">"/&gt;</xsl:text>
	       </treerow>
	       <treechildren>
		  <xsl:apply-templates/>
	       </treechildren>
	    </treeitem>
	 </xsl:when>
	 <xsl:otherwise>
	    <treeitem>
	       <treerow>
		  <xsl:text disable-output-escaping="yes">&lt;treecell label="</xsl:text>
		  <xsl:value-of select="@name"/>
		  <xsl:text disable-output-escaping="yes">"/&gt;</xsl:text>
	       </treerow>
	    </treeitem>
	 </xsl:otherwise>
      </xsl:choose>
   </xsl:template>
   
</xsl:stylesheet>

