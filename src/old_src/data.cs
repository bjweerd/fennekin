using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;

using Common; // Provides the NodeTree class from codeproject.com

// NodeTree class
// **************
//
// Originally explained in: http://www.codeproject.com/csharp/TreeCollection.asp
//
// I Chose this one over the one in http://www.codeproject.com/useritems/phSharpTree.asp because
// this one offers pruning and grafting.
//


namespace WebDiver
{
    // Tree representation of the xml file. Casaded Search Engines work like this:
    // 1) user clicks a node.
    // 2) the engines for that node are put in a list, and the parent is visited
    // 3) for the parent, the engines for the parentnode are added to the list, and so on.
    // 4) working backward, the engines are 'overwritten' so the cascading effect occurs.
    // 5) this whole thing results in a list of engines to be updated in the gui 
    //    structure (dropdown combo) on the fly.

    class Data
    {
        //
        // Webdiver main data structure
        //

        // search engine data
        public class Engine
        {
            public string name;
            public string query;

            public Engine(string _name, string _query)
            {
                name = _name;
                query = _query;
            }
        }

        // term data - a term has a name and engine-related data
        public class Term 
        {
            public string name;
            public List<string> enginesets;
            public List<Engine> engines;

            public Term() { 
                enginesets = new List<string>(); 
                engines = new List<Engine>(); 
                name = ""; 
            }
        }

        // this class manages a whole tree. We have a list of these
        // in a global List<>
        public class TermTree
        {
            // we want a tree of Term.
            public ITree tree;
            
            public TermTree() { 
                tree = NodeTree.NewTree(typeof(Term));
            }
        }

        // engine set is list of engines
        public class EngineSet : List<Engine>
        {
            public string name;

            public EngineSet(string _name) { name = _name; }
        }

        public List<EngineSet> engineSets;
        public List<TermTree> termTrees;
        public List<Engine> rootEngines;

        //
        // constructor
        //
        
        public Data()
        {
            engineSets = new List<EngineSet>();
            termTrees = new List<TermTree>();
            rootEngines = new List<Engine>();
            Reset();
        }

        public void Reset()
        {
            engineSets.Clear();
            termTrees.Clear();
            rootEngines.Clear();
        }

        public void TreeVerify()
        {
            string message;

            // we want detailled statistics
            int nCountTerms = 0;
            int nCountEnginesInSets = 0;
            int nCountEnginesInTerms = 0;
            int nCountEngineSets = 0;
            int nCountRootTerms = 0;
            int nCountTermsWithSetRefs = 0;
            int nCountSetRefs = 0;
            int nCountRootEngines = 0;

            // count root engines
            {
                nCountRootEngines = rootEngines.Count;
            }

            // count enginesets and engines in enginesets
            {
                foreach (EngineSet eSet in engineSets)
                {
                    nCountEngineSets++;
                    foreach (Engine e in eSet)
                    {
                        nCountEnginesInSets++;
                    }
                }
            }

            // count the term trees statistics
            {
                foreach (TermTree tTree in termTrees)
                {
                    nCountRootTerms++;
                    CountTermStats(tTree.tree.Root,
                           ref nCountTerms,
                           ref nCountEnginesInTerms,
                           ref nCountTermsWithSetRefs,
                           ref nCountSetRefs);
                }
            }

            message = "Statistics:\r\n\r\n";

            if (nCountEngineSets > 0)
            {
                message += nCountEnginesInSets.ToString() + " engines in "
            + nCountEngineSets.ToString() + " enginesets. ";
            }
            
            message +=
            nCountRootTerms.ToString() + " term trees contain "
            + nCountTerms.ToString() + " terms";
            if (nCountEnginesInTerms > 0)
            {
                message += " and "
             + nCountEnginesInTerms.ToString() + " engines";
            }
            message +=".\r\n";

            if (nCountRootEngines > 0)
            {
                message += "There are " + nCountRootEngines.ToString() + " toplevel (root) engines\r\n";
            }
            if (nCountSetRefs > 0)
            {

                message += nCountTermsWithSetRefs.ToString() + " terms contain "
                + nCountSetRefs.ToString() + " engineset references totally.\r\n";
            }
            // TODO: we also want unreferenced and unused engineset reporting
            // -->idea: List<string> foundEngineSetNames = new List<string>();

            // Print verification
            MainWindow.Notice(message);
        }
        
	void CountTermStats(INode parent,
			    ref int nCountTerms,
			    ref int nCountEnginesInTerms,
			    ref int nCountTermsWithSetRefs,
			    ref int nCountSetRefs)
        {
            foreach (INode child in parent.DirectChildren)
            {
                Term term = (Term)child.Data;
                int numChildren = child.DirectChildCount;

                // got a term, obviously
                nCountTerms++;

                // count the engines for this term
                if (term.engines.Count > 0)
                    nCountEnginesInTerms += term.engines.Count;

                // check engineset references
                if (term.enginesets.Count > 0)
                {
                    nCountTermsWithSetRefs++;
                    nCountSetRefs += term.enginesets.Count;
                }

                // walk its childs
		CountTermStats(child,
			       ref nCountTerms,
			       ref nCountEnginesInTerms,
			       ref nCountTermsWithSetRefs,
			       ref nCountSetRefs);
            }
        }

















        // 
        // XML Serialization: READING FILES
        //

        
        class SemanticException : System.Xml.XmlException
        {
            public string message;
            public int line;

            public SemanticException(string _message)
            {
                message = _message;
                line = 0;
            }
            public SemanticException(string _message, int _line)
            {
                message = _message;
                line = _line;
            }
        }



        public bool ReadFromFile(string fileName)
        {
            Reset();

            // 
            // This part of the function conserns the actual reading of the XML file
            //

            StreamReader streamReader = new StreamReader(fileName);

            try
            {
                bool AllowNode_webdiver = true;
                bool AllowNode_term = false;
                bool AllowNode_engineset = false;
                bool AllowNode_engine = false;
                bool AllowNode_rootEngine = false;

                {
                    XmlReaderSettings xmlReaderSettings = new XmlReaderSettings();
                    xmlReaderSettings.CheckCharacters = true; // check for illegal characters

                    XmlReader xmlReader = XmlReader.Create(streamReader, xmlReaderSettings);
                    {
                        while (xmlReader.Read())
                        {
                            switch (xmlReader.NodeType)
                            {
                                case XmlNodeType.Element:
                                    {
                                        string nodeName = xmlReader.Name;

                                        if (AllowNode_webdiver && nodeName == "webdiver")
                                        {
                                            while (xmlReader.MoveToNextAttribute())
                                            {
                                                string attrName = xmlReader.Name;
                                                string attrValue = xmlReader.Value;

                                                if (attrName == "version")
                                                {
                                                    if (attrValue != "2.0")
                                                        throw new SemanticException("webdiver file version mismatch (this program reads 2.0 only)");
                                                }
                                                else
                                                    throw new SemanticException("`webdiver' xml element contains invalid attribute `" + attrName + "'");
                                            }

                                            AllowNode_webdiver = false;
                                            AllowNode_engineset = true;
                                            AllowNode_term = true;
                                            AllowNode_engine = false;
                                            AllowNode_rootEngine = true;
                                        }
                                        else if (AllowNode_engineset && nodeName == "engineset")
                                        {
                                            while (xmlReader.MoveToNextAttribute())
                                            {
                                                string attrName = xmlReader.Name;
                                                string attrValue = xmlReader.Value;

                                                if (attrName == "name")
                                                {
                                                    // create new EngineSet
                                                    EngineSet engineSet = new EngineSet(attrValue);
                                                    
                                                    // append EngineSet to our global array
                                                    engineSets.Add(engineSet);
                                                }
                                                else
                                                    throw new SemanticException("`engineset' xml element contains invalid attribute `" + attrName + "'");
                                            }

                                            AllowNode_engine = true;
                                            AllowNode_term = false;
                                            AllowNode_engineset = false;
                                            AllowNode_rootEngine = false;
                                        }
                                        else if (AllowNode_rootEngine && nodeName == "engine")
                                        {
                                            string name = "", query = "";

                                            while (xmlReader.MoveToNextAttribute())
                                            {
                                                string attrName = xmlReader.Name;
                                                string attrValue = xmlReader.Value;

                                                if (attrName == "name") name = attrValue;
                                                else if (attrName == "query") query = attrValue;
                                                else
                                                    throw new SemanticException("`engine' xml element contains invalid attribute `" + attrName + "'");
                                            }

                                            if (name == "" || query == "")
                                                throw new SemanticException("`engine' xml element requires (non-empty) `name' and `query' attributes");

                                            // create an Engine object
                                            Engine engine = new Engine(name, query);

                                            // put in our global array
                                            rootEngines.Add(engine);
                                        }
                                        else if (AllowNode_engine && nodeName == "engine")
                                        {
                                            string name = "";
                                            string query = "";

                                            while (xmlReader.MoveToNextAttribute())
                                            {
                                                string attrName = xmlReader.Name;
                                                string attrValue = xmlReader.Value;

                                                if (attrName == "name") name = attrValue;
                                                else if (attrName == "query") query = attrValue;
                                                else
                                                    throw new SemanticException("`engine' xml element contains invalid attribute `" + attrName + "'");
                                            }

                                            if (name == "" || query == "")
                                                throw new SemanticException("`engine' xml element requires (non-empty) `name' and `query' attributes");

                                            // create an Engine object
                                            Engine engine = new Engine(name, query);

                                            // find last engineset in our global array
                                            // and add the engine
                                            engineSets[engineSets.Count - 1].Add(engine); ;

                                        }
                                        else if (AllowNode_term && nodeName == "term")
                                        {
                                            AllowNode_rootEngine = false;
                                            TermTree termTree = new TermTree();

                                            // recurse into term tree structure
                                            ReadXmlTerm(termTree.tree.Root, ref xmlReader);

                                            termTrees.Add(termTree);
                                            AllowNode_rootEngine = true;
                                        }
                                        else
                                        {
                                            throw new SemanticException("Invalid XML element `" + nodeName + "' not allowed here");
                                        }
                                    }
                                    break;

                                case XmlNodeType.EndElement:
                                    {
                                        string nodeName = xmlReader.Name;

                                        if (nodeName == "webdiver")
                                        {
                                            AllowNode_engineset = false;
                                            AllowNode_term = false;
                                            AllowNode_engine = false;
                                            AllowNode_rootEngine = false;
                                        }
                                        else if (nodeName == "engineset")
                                        {
                                            AllowNode_engine = false;
                                            AllowNode_term = true;
                                            AllowNode_engineset = true;
                                            AllowNode_rootEngine = true;
                                        }
                                        else
                                        {
                                            throw new SemanticException("Invalid XML end element `" + nodeName + "'");
                                        }
                                    }
                                    break;
                            }
                        }
                    }
                }

                streamReader.Close();
            }
            catch (SemanticException e)
            {
                string message = "Semantic error: Line "+e.LineNumber.ToString()+":" + e.message + " (" + e.Message + ")";

                MainWindow.Notice(message);
                Reset();
                streamReader.Close();
                return false;
            }
            catch (XmlException e)
            {
                string message = "XML Error: syntax error(s) at line "+e.LineNumber.ToString()+": " + e.Message;

                MainWindow.Notice(message);
                Reset();
                streamReader.Close();
                return false;
            }
            catch (Exception e)
            {
                string message = "File error: while reading file `" + fileName + "': " + e.Message;

                MainWindow.Notice(message);
                Reset();
                streamReader.Close();
                return false;
            }

            return true; // File loaded OK into memory
        }

        private class ReadXmlTermData
        {
            Term termResult;
            public ReadXmlTermData() { termResult = new Term(); }
        }

        private void ReadXmlTerm(INode parent, ref XmlReader xmlReader)
        {
            string nodeName = xmlReader.Name;
            bool isEmptyElement = xmlReader.IsEmptyElement, isAddedToParent = false;
            string termName = "";

            Term termResult = new Term();

            // get attributes
            while (xmlReader.MoveToNextAttribute())
            {
                string attrName = xmlReader.Name;
                string attrValue = xmlReader.Value;

                if (attrName == "name")
                {
                    termName = attrValue;
                    termResult.name = termName;
                }
                else if (attrName == "engineset")
                {
                    string enginesetName = attrValue;

                    if (enginesetName == "")
                        throw new SemanticException("the `engineset' attribute in `term' element must be non-empty");

                    termResult.enginesets.Add(enginesetName);
                }
                else
                    throw new SemanticException("`term' xml element contains invalid attribute `" + attrName + "'");
            }

            if (termResult.name == "")
                throw new SemanticException("`term' xml element must contain a (non-empty) `name' attribute");

            // does it have *no* inner xml?
            if (isEmptyElement)
            {
                parent.AddChild(termResult);
                return;
            }

            //
            // else: read inner children
            //

            INode newParent = null; // kludge?

            while (xmlReader.Read())
            {
                switch (xmlReader.NodeType)
                {
                    case XmlNodeType.Element:
                        {
                            nodeName = xmlReader.Name;
                            if (nodeName == "term")
                            {
                                if (newParent == null)
                                {
                                    newParent = parent.AddChild(termResult);
                                }
                                isAddedToParent = true; // flag needed to decide when we hit </term> to parent.AddChild(..)... or not

                                // parse term subtree
                                ReadXmlTerm(newParent, ref xmlReader); 
                            }
                            else if (nodeName == "engine")
                            {
                                string name = "";
                                string query = "";

                                while (xmlReader.MoveToNextAttribute())
                                {
                                    string attrName = xmlReader.Name;
                                    string attrValue = xmlReader.Value;

                                    if (attrName == "name") name = attrValue;
                                    else if (attrName == "query") query = attrValue;
                                    else
                                        throw new SemanticException("`engine' xml element contains invalid attribute `" + attrName + "'");
                                }

                                if (name == "" || query == "")
                                    throw new SemanticException("`engine' xml element requires `name' and `query' attributes");

                                termResult.engines.Add(new Engine(name, query));
                            }
                            else
                                throw new SemanticException("Invalid XML element `" + nodeName + "'");
                        }
                        break;
                    
                    case XmlNodeType.EndElement:
                        {
                            nodeName = xmlReader.Name;

                            if (nodeName == "term")
                            {
                                if (isAddedToParent == false)
                                    parent.AddChild(termResult);

                                return;
                            }
                            else
                                throw new SemanticException("Invalid XML end element `" + nodeName + "'");
                        }
                }
            }

            throw new SemanticException("vague </term> or </webdiver> error");
        }












        // 
        // XML Serialization: WRITING FILES
        //



        private string ToXML(string s)
        {
            string result = "";

            foreach (char ch in s)

                if (ch == '&')
                    result += "&amp;";
                else if (ch == '<')
                    result += "&lt;";
                else if (ch == '>')
                    result += "&gt;";
                else if (ch == '\'')
                    result += "&apos;";
                else if (ch == '\"')
                    result += "&quot;";
                else
                    result += ch;

            return result;
        }

        
        public void WriteToFile(string fileName)
        {
            // TODO: line below throws exceptions, place this in exception handlers
            StreamWriter swOut = new StreamWriter(new FileStream(fileName,FileMode.Create), Encoding.UTF8);

            swOut.WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
            swOut.WriteLine();
            swOut.WriteLine("<webdiver version=\"2.0\">");
            swOut.WriteLine();

            WriteToFileBody(swOut);

            swOut.WriteLine();
            swOut.WriteLine("</webdiver>");
            swOut.Close();

        }

        private void WriteIndent(StreamWriter swOut, int indent)
        {
            for (int i = 0; i < indent; i++)
                swOut.Write("    "); // one indent is four spaces
        }

        public void WriteToFileBody(StreamWriter swOut)
        {
            // these variables are for formatting only
            int indent = 1;
            bool wroteEngineSets = false, wroteTermTree = false, wroteRootEngines=false;

            // write root engines
            {
                foreach (Engine e in rootEngines)
                {
                    WriteIndent(swOut, indent);
                    swOut.WriteLine("<engine name=\"{0}\" query=\"{1}\"/>", ToXML(e.name), ToXML(e.query));
                    wroteRootEngines = true;
                }
            }

            // write enginesets and engines in enginesets
            {
                if (wroteRootEngines) { wroteRootEngines = false; swOut.WriteLine(); }
                
                foreach (EngineSet eSet in engineSets)
                {
                    WriteIndent(swOut, indent); swOut.WriteLine("<engineset name=\"{0}\">", ToXML(eSet.name)); ++indent;

                    foreach (Engine e in eSet)
                    {
                        WriteIndent(swOut, indent);
                        swOut.WriteLine("<engine name=\"{0}\" query=\"{1}\"/>", ToXML(e.name), ToXML(e.query));
                    }

                    --indent; WriteIndent(swOut, indent); swOut.WriteLine("</engineset>");
                    wroteEngineSets = true;
                }
            }


            // write the term trees
            {
                foreach (TermTree tTree in termTrees)
                {
                    if (wroteRootEngines) { wroteRootEngines = false; swOut.WriteLine(); }
                    if (wroteEngineSets) { wroteEngineSets = false; swOut.WriteLine(); }
                    if (wroteTermTree) swOut.WriteLine();
                    WriteToFileTerm(swOut, tTree.tree.Root, 1); // root term is at indent level 1
                    wroteTermTree = true;
                }
            }
        }

        void WriteToFileTerm(StreamWriter swOut, INode parent, int indent)
        {
            foreach (INode child in parent.DirectChildren)
            {
                Term term = (Term) child.Data;

		// KLUDGE: Somehow .NET fucks up when writing out
		// arabic unicode (right-to-left) These flushes fixes it.

		WriteIndent(swOut, indent);
		swOut.Write("<term name=\"{0}\"", ToXML(term.name));
                swOut.Flush();
                foreach (string engineset in term.enginesets) {
			swOut.Write(" engineset=\"{0}\"", ToXML(engineset));
			swOut.Flush();
		}

                // does our term contain inner xml elements?
                if (child.DirectChildCount > 0 || term.engines.Count > 0)
                {
                    swOut.WriteLine(">"); indent++;

                    // first do the engines
                    foreach (Engine e in term.engines)
                    {
                        WriteIndent(swOut, indent);
                        swOut.WriteLine("<engine name=\"{0}\" query=\"{1}\"/>", ToXML(e.name), ToXML(e.query));
                    }

                    // walk its childs
                    WriteToFileTerm(swOut, child, indent);

                    --indent; WriteIndent(swOut, indent); swOut.WriteLine("</term>");
                }
                else
                {
                    swOut.WriteLine("/>");
                }
            }
        }











        //
        // Non-file data generation functions
        // 
        // GenerateNewFileWizard() and GenerateFromTransformedXSL() are 
        // defined here.
        //


        public void GenerateNewFileWizard(EngineSet selectedEngineSet, string topic)
        {
            // 1) selectedEngineSet --> select search engine set
            // 2) topic             --> topic you want to explore

            // maak nieuwe data aan, volgens de parameters van de New File wizard.
            Reset();

            // Dump the selected engine set into the root engine list
            foreach (Engine e in selectedEngineSet)
            {
                rootEngines.Add(e);
            }
            
            // create tree with one term: `topic'
            TermTree termTree = new TermTree();
            Term term = new Term();
            term.name = topic;
            termTree.tree.AddChild(term);
            termTrees.Add(termTree);
        }
    }
}
