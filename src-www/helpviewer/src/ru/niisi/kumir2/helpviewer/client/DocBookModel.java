package ru.niisi.kumir2.helpviewer.client;

import java.util.LinkedList;
import java.util.List;

import com.google.gwt.user.client.ui.TreeItem;
import com.google.gwt.xml.client.*;

public class DocBookModel extends TreeItem {
	
	
	private final static DocBookModelType[] EXPANDABLE_TAGS = { 
		DocBookModelType.Book,
		DocBookModelType.Article,
		DocBookModelType.Chapter,
		DocBookModelType.Section
	};
		
	private final static int MAX_SECTION_LEVEL = 2;
	
	private DocBookType docBookType = null;
	private DocBookModelType modelType = null;
	private int sectionLevel = 0;
	private String title = null;
	private String subtitle = null;
	private List<DocBookModel> children = new LinkedList<DocBookModel>(); 
	private DocBookModel abstractOrPreface = null;
	private DocBookModel reference = null;
	private DocBookModel parent = null;
	private String nodeText = "";
	private String id = "";
	private String XrefLinkend = null;
	private String XrefEndTerm = null;
	private String EmphasisRole = null;
	static int generateIdCounter = 0;
	
	public DocBookModel(final Element root) {
		super();
		if (root.getNodeName().equalsIgnoreCase("book")) {
			init(null, DocBookType.Book, DocBookModelType.Book, root);
		}
		else if (root.getNodeName().equalsIgnoreCase("article")) {
			init(null, DocBookType.Article, DocBookModelType.Article, root);
		}
	}
	
	public DocBookModel() {
		super();
	}
	
	public DocBookModel(DocBookModel parent, String text) {
		super();
		init(parent, text);
	}
	
	@Override
	public String getHTML() {
		return getTitle();
	}
	
	public DocBookModel(DocBookModel parent, DocBookType docBookType, DocBookModelType modelType, final Element element) {
		init(parent, docBookType, modelType, element);
	}
	
	public int getSectionLevel() {
		return sectionLevel;
	}
	
	protected String extractStringData(final Element element) {
		String t = "";
		final NodeList nodes = element.getChildNodes();
		for (int i=0; i<nodes.getLength(); i++) {
			final Node n = nodes.item(i);
			t += " "+n.toString();
		}
		return t.trim();
	}
	
	@Override
	public TreeItem getParentItem() {
		return parent;
	}
	
	protected String getDocumentId() {
		DocBookModel p = this;
		while (p.modelType!=DocBookModelType.Book && p.modelType!=DocBookModelType.Article) {
			p = p.parent;
		}
		return p.id;
	}
	
	public void setSectionLevel(int value) {
		this.sectionLevel = value;
	}
	
	protected void init(DocBookModel parent, String text) {
		// Init text node
		this.parent = parent;
		this.modelType = DocBookModelType.__text__;
		if (parent==null) {
			setSectionLevel(0);
		}
		else {
			setSectionLevel(parent.getSectionLevel());
		}
		this.nodeText = text;
	}
	
	protected void init(DocBookModel parent, DocBookType docBookType, DocBookModelType modelType, final Element element) {
		this.parent = parent;
		this.docBookType = docBookType;
		this.modelType = modelType;
		if (element.getNodeName().equalsIgnoreCase("section") ) {
			if (parent==null) {
				setSectionLevel(1);
			}
			else {
				setSectionLevel(parent.getSectionLevel()+1);
			}
		}
		else {
			if (parent==null) {
				setSectionLevel(0);
			}
			else {
				setSectionLevel(parent.getSectionLevel());
			}
		}
		if (element.hasAttribute("id")) {
			this.id = element.getAttribute("id").trim(); 
		}
		else {
			this.id = "GENERATED_"+(new Integer(generateIdCounter).toString());
			generateIdCounter ++;
		}
		
		if (element.getNodeName().equalsIgnoreCase("xref")) {
			if (element.hasAttribute("linkend"))
				XrefLinkend = element.getAttribute("linkend");
			if (element.hasAttribute("endterm"))
				XrefEndTerm = element.getAttribute("endterm");
		}
		if (element.getNodeName().equalsIgnoreCase("emphasis")) {
			if (element.hasAttribute("role"))
				EmphasisRole = element.getAttribute("role");
		}
		
		final NodeList nodes = element.getChildNodes();
		DocBookModel childTreeItem = null;
		for (int i=0; i<nodes.getLength(); i++){
			final Node n = nodes.item(i);
			if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("title")) {
				title = extractStringData((Element)n);
				setText(title);
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("subtitle")) {
				subtitle = extractStringData((Element)n);
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("abstract")) {
				abstractOrPreface = new DocBookModel(this, docBookType, DocBookModelType.Abstract, (Element)n);
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("preface")) {
				abstractOrPreface = new DocBookModel(this, docBookType, DocBookModelType.Preface, (Element)n);
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("reference")) {
				abstractOrPreface = new DocBookModel(this, docBookType, DocBookModelType.Reference, (Element)n);
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("chapter")) {
				childTreeItem = new DocBookModel(this, docBookType, DocBookModelType.Chapter, (Element)n); 
				children.add(childTreeItem);
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("section")) {
				childTreeItem = new DocBookModel(this, docBookType, DocBookModelType.Section, (Element)n);
				children.add(childTreeItem);
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("para")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.Para, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("programlisting")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.ProgramListing, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("example")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.Example, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("orderedlist")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.OrderedList, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("itemizedlist")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.ItemizedList, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("listitem")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.ListItem, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("xref")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.Xref, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("para")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.Para, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("emphasis")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.Emphasis, (Element)n));
			}
			else if (n.getNodeType()==Node.TEXT_NODE) {
				children.add(new DocBookModel(this, n.toString()));
			}
			if (childTreeItem!=null) {
				if (modelType==DocBookModelType.Book || modelType==DocBookModelType.Article || modelType==DocBookModelType.Chapter) {
					addItem(childTreeItem);
				}
				else if (modelType==DocBookModelType.Section && sectionLevel<=MAX_SECTION_LEVEL) {
					addItem(childTreeItem);
				}
			}
		}
		
		String xreflabel = "";
		if (element.hasAttribute("xreflabel")) {
			xreflabel = element.getAttribute("xreflabel").trim();
		}
		else {
			xreflabel = this.title;
		}
		String documentId = getDocumentId();
		XrefDatabase.add(documentId, this.id, xreflabel, this);
	}
	
	public String getTitle() {
		return title;
	}
	
	public String getSubTitle() {
		return subtitle;
	}
	
	public DocBookModel getPreface() {
		return abstractOrPreface;
	}
	
	public DocBookModel getAbstract() {
		return abstractOrPreface;
	}
	
	public DocBookModel getReference() {
		return reference;
	}
	
	public String getContentHTML() {
		String result = "";
		// 0. Add container if need
		if (modelType==DocBookModelType.Example) {
			result += "<example>\n";
		}
		else if (modelType==DocBookModelType.OrderedList) {
			result += "<ol>\n";
		}
		else if (modelType==DocBookModelType.ItemizedList) {
			result += "<ul>\n";
		}
		else if (modelType==DocBookModelType.ListItem) {
			result += "<li>\n";
		}
		else if (modelType==DocBookModelType.Para) {
			result += "<p>";
		}
		else if (modelType==DocBookModelType.ProgramListing) {
			result += "<pre>";
		}
		else if (modelType==DocBookModelType.Xref) {
			if (XrefLinkend!=null) {
				result += generateXref(XrefLinkend, XrefEndTerm);
			}
		}
		else if (modelType==DocBookModelType.Emphasis) {
			result += "<span class=\"emph_"+EmphasisRole+"\">";
		}
		// 1. Add title
		switch (modelType) {
		case Book:
			if (title!=null && !title.isEmpty())
				result += "<booktitle>"+normalizeText(title)+"</booktitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<booksubtitle>"+normalizeText(subtitle)+"</booksubtitle>\n";
			break;
		case Article:
			if (title!=null && !title.isEmpty())
				result += "<booktitle>"+normalizeText(title)+"</booktitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<booksubtitle>"+normalizeText(subtitle)+"</booksubtitle>\n";
			break;
		case Chapter:
			if (title!=null && !title.isEmpty())
				result += "<chaptertitle>"+normalizeText(title)+"</chaptertitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<chaptersubtitle>"+normalizeText(subtitle)+"</chaptersubtitle>\n";
			break;
		case Section:
			if (title!=null && !title.isEmpty())
				result += "<sectiontitle class=\"level"+(new Integer(sectionLevel-MAX_SECTION_LEVEL).toString())+"\">"+normalizeText(title)+"</sectiontitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<sectionsubtitle class=\"level"+(new Integer(sectionLevel-MAX_SECTION_LEVEL).toString())+"\">"+normalizeText(subtitle)+"</chaptersubtitle>\n";
			break;
		case Example:
			if (title!=null && !title.isEmpty())
				result += "<exampletitle>"+normalizeText(title)+"</exampletitle>\n";
			break;
		default:
			break;
		}
		
		if (modelType==DocBookModelType.Book || modelType==DocBookModelType.Article) {
			if (abstractOrPreface!=null) {
				result += abstractOrPreface.getContentHTML();
			}
		}
		if (!isLeaf()) 
		{
			result += generateIndex(true);
		}
		else {
			for (final DocBookModel child : children) {
				result += child.getContentHTML();
			}
		}
		
		if (modelType==DocBookModelType.__text__) {
			if (parent!=null && parent.getModelType()==DocBookModelType.ProgramListing) {
				result += nodeText;
			}
			else {
				result += normalizeText(nodeText);
			}
		}
		
		// Z. Close container if need
		if (modelType==DocBookModelType.Example) {
			result += "</example>\n";
		}
		else if (modelType==DocBookModelType.OrderedList) {
			result += "</ol>\n";
		}
		else if (modelType==DocBookModelType.ItemizedList) {
			result += "</ul>\n";
		}
		else if (modelType==DocBookModelType.ListItem) {
			result += "</li>\n";
		}
		else if (modelType==DocBookModelType.Para) {
			result += "</p>";
		}
		else if (modelType==DocBookModelType.ProgramListing) {
			result += "</p>";
		}
		else if (modelType==DocBookModelType.Emphasis) {
			result += "</span>";
		}
		
		return result;
	}

	private String normalizeText(String s) {
		String result = s;
		result = result.replace("&semi;", ";");
		result = result.replace("---", "&mdash;").replace("--", "&ndash;");
		result = result.replace("role=\"", "class=\"emph_");
		result = result.replace("<emphasis ", "<span ").replace("</emphasis>","</span>");
		boolean openQuote = true;
		int qp = 0;
		do {
			qp = result.indexOf("&quot;");
			if (qp!=-1) {
				String qr = openQuote? "&laquo;" : "&raquo;";
				result = result.substring(0, qp)+qr+result.substring(qp+6);
				openQuote = !openQuote;
			}
		} while (qp!=-1);
		return result;
	}

	protected String generateAHref() {
		String result = "";
		String documentId = getDocumentId();
		result += "<a class=\"indexelement\" href=\"#\" onclick=\"openXref('"+documentId+"','"+this.id+"')\">";
		result += XrefDatabase.findTitle(documentId, this.id);
		result += "</a>";
		return result;
	}
	
	protected String generateXref(String linkend, String endterm) {
		String result = " (см.&nbsp;";
		String documentId = getDocumentId();
		result += "<a href=\"#\" onclick=\"openXref('"+documentId+"','"+linkend+"')\">";
		if (endterm==null)
			result += XrefDatabase.findTitle(documentId, linkend);
		else
			result += XrefDatabase.findTitle(documentId, endterm);
		result += "</a>";
		result += ")";
		return result;
	}
	
	protected String generateIndex(boolean toplevel) {
		String result = "";
		
		if (modelType==DocBookModelType.__text__)
			return "";
		
		if (isLeaf()) {
			return generateAHref();
		}
		else {
			if (toplevel) {
				result += "<div class=\"tableofcontents\">\n";
				result += "<p>Содержание:</p>\n";
			}
			if (!toplevel)
				result += "<p>"+generateAHref()+"</p>\n";
			result += "<ul>\n";
			for ( DocBookModel child : children ) {
				if (child.getModelType()!=DocBookModelType.__text__) {
					result += "<li>";
					result += child.generateIndex(false);
					result += "</li>";
				}
			}
			result += "</ul>\n";
			if (toplevel) {
				result += "</div>\n";
			}
		}
		return result;
	}	

	
	public boolean isLeaf() {
		for ( DocBookModelType t  : EXPANDABLE_TAGS ) {
			if (modelType==t) {
				if (modelType==DocBookModelType.Section) {
					if (sectionLevel>=MAX_SECTION_LEVEL) {
						return true;
					}
					else {
						break; // Must check childs
					}
				}
				else {
					return false;
				}
			}
		}
		for (final DocBookModel child : children) {
			if (child.getModelType()==DocBookModelType.Section) {
				return false; // Section has subsections
			}
		}
		return true;
	}

	public DocBookType getDocBookType() {
		return docBookType;
	}


	public DocBookModelType getModelType() {
		return modelType;
	}
	
}
