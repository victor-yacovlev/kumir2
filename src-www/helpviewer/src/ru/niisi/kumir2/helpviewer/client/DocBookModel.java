package ru.niisi.kumir2.helpviewer.client;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.extjs.gxt.ui.client.core.FastMap;
import com.extjs.gxt.ui.client.data.ChangeEvent;
import com.extjs.gxt.ui.client.data.ChangeListener;
import com.extjs.gxt.ui.client.data.ModelData;
import com.extjs.gxt.ui.client.data.TreeModel;
import com.google.gwt.xml.client.*;

public class DocBookModel implements TreeModel {
	
	
	private final static DocBookModelType[] EXPANDABLE_TAGS = { 
		DocBookModelType.Book,
		DocBookModelType.Article,
		DocBookModelType.Chapter,
		DocBookModelType.Section
	};
		
	private final static int MAX_SECTION_LEVEL = 1;
	
	private DocBookType docBookType = null;
	private DocBookModelType modelType = null;
	private int sectionLevel = 0;
	private String title = null;
	private String subtitle = null;
	private List<DocBookModel> children = new LinkedList<DocBookModel>(); 
	private DocBookModel abstractOrPreface = null;
	private DocBookModel reference = null;
	private DocBookModel parent = null;
	private String html = "";
	
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
	
	
	
	protected void init(DocBookModel parent, DocBookType docBookType, DocBookModelType modelType, final Element element) {
		this.parent = parent;
		this.docBookType = docBookType;
		this.modelType = modelType;
		if (element.getNodeName().equalsIgnoreCase("section")) {
			if (parent!=null) {
				sectionLevel = parent.getSectionLevel();
			}
			sectionLevel += 1;
		}
		final NodeList nodes = element.getChildNodes();
		for (int i=0; i<nodes.getLength(); i++){
			final Node n = nodes.item(i);
			if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("title")) {
				title = extractStringData((Element)n);
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
				children.add(new DocBookModel(this, docBookType, DocBookModelType.Chapter, (Element)n));
			}
			else if (n.getNodeType()==Node.ELEMENT_NODE && n.getNodeName().equalsIgnoreCase("section")) {
				children.add(new DocBookModel(this, docBookType, DocBookModelType.Section, (Element)n));
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
			else {
				html += n.toString();
			}
		}
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
	
	public String getHTML() {
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
		// 1. Add title
		switch (modelType) {
		case Book:
			if (title!=null && !title.isEmpty())
				result += "<booktitle>"+docBookToHtml(title)+"</booktitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<booksubtitle>"+docBookToHtml(subtitle)+"</booksubtitle>\n";
			break;
		case Article:
			if (title!=null && !title.isEmpty())
				result += "<booktitle>"+docBookToHtml(title)+"</booktitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<booksubtitle>"+docBookToHtml(subtitle)+"</booksubtitle>\n";
			break;
		case Chapter:
			if (title!=null && !title.isEmpty())
				result += "<chaptertitle>"+docBookToHtml(title)+"</chaptertitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<chaptersubtitle>"+docBookToHtml(subtitle)+"</chaptersubtitle>\n";
			break;
		case Section:
			if (title!=null && !title.isEmpty())
				result += "<sectiontitle class=\"level"+(new Integer(sectionLevel-MAX_SECTION_LEVEL).toString())+"\">"+docBookToHtml(title)+"</sectiontitle>\n";
			if (subtitle!=null && !subtitle.isEmpty())
				result += "<sectionsubtitle class=\"level"+(new Integer(sectionLevel-MAX_SECTION_LEVEL).toString())+"\">"+docBookToHtml(subtitle)+"</chaptersubtitle>\n";
			break;
		case Example:
			if (title!=null && !title.isEmpty())
				result += "<exampletitle>"+docBookToHtml(title)+"</exampletitle>\n";
			break;
		default:
			break;
		}
		
		if (modelType==DocBookModelType.Book || modelType==DocBookModelType.Article) {
			if (abstractOrPreface!=null) {
				result += abstractOrPreface.getHTML();
			}
		}
		if (modelType==DocBookModelType.Book || 
				modelType==DocBookModelType.Article ||
				modelType==DocBookModelType.Chapter ||
				(modelType==DocBookModelType.Section && sectionLevel<MAX_SECTION_LEVEL)
				) 
		{
			result += generateIndex();
		}
		else {
			for (final DocBookModel child : children) {
				result += child.getHTML();
			}
		}
		
		if (modelType==DocBookModelType.Para) {
			result += "<p>"+docBookToHtml(html)+"</p>\n";
		}
		else if (modelType==DocBookModelType.ProgramListing) {
			result += "<pre>"+html+"</pre>\n";
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
		
		return result;
	}

	private String docBookToHtml(String s) {
		String result = s;
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

	private String generateIndex() {
		return "";
	}

	@SuppressWarnings("unchecked")
	@Override
	public <X> X get(String property) {
		if (property.equalsIgnoreCase("title")) {
			return (X) getTitle();
		}
		return null;
	}

	@Override
	public Map<String, Object> getProperties() {
		Map<String, Object> newMap = new FastMap<Object>();
		for (String s : getPropertyNames()) {
			newMap.put(s, get(s));
		}
		return newMap;
	}

	@Override
	public Collection<String> getPropertyNames() {
		LinkedList<String> result = new LinkedList<String>();
		result.add("title");
		return result;
	}

	@Override
	public <X> X remove(String property) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public <X> X set(String property, X value) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void addChangeListener(ChangeListener... listener) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void removeChangeListener(ChangeListener... listener) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void removeChangeListeners() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setSilent(boolean silent) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void notify(ChangeEvent event) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void add(ModelData child) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public ModelData getChild(int index) {
		return getChildren().get(index);
	}

	@Override
	public int getChildCount() {
		return getChildren().size();
	}

	@Override
	public List<ModelData> getChildren() {
		List<ModelData> result = new LinkedList<ModelData>();
		for (DocBookModel m : children) {
			DocBookModelType t = m.getModelType();
			boolean candidate = false;
			for (DocBookModelType tt : EXPANDABLE_TAGS) {
				if (tt==t) {
					candidate = true;
					break;
				}
			}
			if (t==DocBookModelType.Section && m.getSectionLevel()>MAX_SECTION_LEVEL+1)
				candidate = false;
			if (candidate)
				result.add(m);
		}
		return result;
		
	}

	@Override
	public TreeModel getParent() {
		return parent;
	}

	@Override
	public int indexOf(ModelData child) {
		return children.indexOf(child);
	}

	@Override
	public void insert(ModelData child, int index) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean isLeaf() {
		for ( DocBookModelType t  : EXPANDABLE_TAGS ) {
			if (modelType==t) {
				if (modelType==DocBookModelType.Section && sectionLevel>MAX_SECTION_LEVEL)
					return true;
				else
					return false;
			}
		}
		return true;
	}

	@Override
	public void remove(ModelData child) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void removeAll() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setParent(TreeModel parent) {
		// TODO Auto-generated method stub
		
	}

	public DocBookType getDocBookType() {
		return docBookType;
	}


	public DocBookModelType getModelType() {
		return modelType;
	}
	
}
