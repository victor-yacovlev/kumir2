package ru.niisi.kumir2.helpviewer.client;

import java.util.LinkedList;
import java.util.List;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.logical.shared.ResizeEvent;
import com.google.gwt.event.logical.shared.ResizeHandler;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.user.client.ui.Frame;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.SplitLayoutPanel;
import com.google.gwt.user.client.ui.TabLayoutPanel;
import com.google.gwt.user.client.Window;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.Element;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.google.gwt.xml.client.Text;
import com.google.gwt.xml.client.XMLParser;


public class ViewerRoot implements EntryPoint, ResizeHandler  {

	private SplitLayoutPanel viewport = null;
	private ViewContentPanel viewContentPanel = null;
	private TabLayoutPanel tabPanel = null;
	private TableOfContentsPanel tableOfContentsPanel = null;
	private IndexPanel indexPanel = null;
	private List<Document> books = new LinkedList<Document>();
	private List<Document> articles = new LinkedList<Document>();
	private Frame printFrame;
	
	public ViewerRoot() {
		createComponents();
	}
	
	protected void createComponents() {
		viewport = new SplitLayoutPanel();
		viewport.ensureDebugId("viewport");
		tabPanel = new TabLayoutPanel(2.5, Unit.EM);
		tabPanel.setWidth("100%");
		tabPanel.setAnimationDuration(600);
		
		tabPanel.ensureDebugId("tabpanel");
		viewContentPanel = new ViewContentPanel();
		viewport.addWest(tabPanel, 240);
		viewport.add(viewContentPanel);
		tableOfContentsPanel = new TableOfContentsPanel();
		tableOfContentsPanel.addOpenNodeHandler(viewContentPanel);
		indexPanel = new IndexPanel();
		indexPanel.addOpenNodeHandler(viewContentPanel);
		//indexPanel.addOpenNodeHandler(tableOfContentsPanel);
		tabPanel.add(tableOfContentsPanel, "Содержание");
		tabPanel.add(indexPanel, "Поиск");
		printFrame = new Frame("_print.html");
		printFrame.getElement().setAttribute("id", "printframe");
	}
	
	protected void loadIncludes(final String baseUrl, final Document doc, final boolean isBook)
	{
		NodeList includes = doc.getElementsByTagName("include");
		if (includes.getLength()==0) {
			if (isBook) {
				books.add(doc);
				tableOfContentsPanel.addBook(doc);
			}
			else {
				articles.add(doc);
				tableOfContentsPanel.addArticle(doc);
			}
		}
		else {
			final Element firstInclude = (Element)includes.item(0);
		
			String href = firstInclude.getAttribute("href");
			String baseDir = baseUrl;
			int slashPos = baseUrl.lastIndexOf('/');
			if (slashPos!=-1) {
				baseDir = baseUrl.substring(0, slashPos);
			}
			final String absHref = baseDir + "/" + href;
			RequestBuilder builder = new RequestBuilder(RequestBuilder.GET, absHref);
			builder.setCallback(new RequestCallback() {
				
				@Override
				public void onResponseReceived(Request request, Response response) {
					final String docbookData = response.getText();
					final Document docbookDocument = XMLParser.parse(docbookData);
					Node n = doc.importNode(docbookDocument.getDocumentElement(), true);
//					doc.getDocumentElement().replaceChild(n, firstInclude);
					firstInclude.getParentNode().replaceChild(n, firstInclude);
					loadIncludes(baseUrl, doc, isBook);
				}
				
				@Override
				public void onError(Request request, Throwable exception) {
					Element errorPara = doc.createElement("para");
					Text errorText = doc.createTextNode("Can't load "+absHref);
					errorPara.appendChild(errorText);
					doc.getDocumentElement().replaceChild(errorPara, firstInclude);
					loadIncludes(baseUrl, doc, isBook);
				}
			});
			try {
				builder.send();
			} catch (RequestException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
	
	protected void loadBooksAndArticles() {
		final String documentList = Window.Location.getParameter("documents");
		if (documentList==null)
			return;
		final String[] documents = documentList.split(",");
		for (final String documentURL : documents) {
			if (!documentURL.isEmpty()) {
				RequestBuilder builder = new RequestBuilder(RequestBuilder.GET, documentURL);
				builder.setCallback(new RequestCallback() {
					
					@Override
					public void onResponseReceived(Request request, Response response) {
						final String docbookData = response.getText();
						final Document docbookDocument = XMLParser.parse(docbookData);
						if (docbookDocument.getDocumentElement().getNodeName().equalsIgnoreCase("book")) {
							loadIncludes(documentURL, docbookDocument, true);
						}
						else if (docbookDocument.getDocumentElement().getNodeName().equalsIgnoreCase("article")) {
							loadIncludes(documentURL, docbookDocument, false);
						}
					}
					
					@Override
					public void onError(Request request, Throwable exception) {
						
					}
				});
				try {
					builder.send();
				} catch (RequestException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	@Override
	public void onModuleLoad() {
		Window.addResizeHandler(this);
		String w = new Integer(Window.getClientWidth()).toString()+"px";
		String h = new Integer(Window.getClientHeight()).toString()+"px";
		viewport.setSize(w, h);
		RootPanel.get().add(viewport);
		tabPanel.selectTab(0);
		final String allowPrintable = Window.Location.getParameter("printable");
		boolean printable = allowPrintable!=null && allowPrintable.equalsIgnoreCase("true");
		DocBookModel.printable = printable;
		loadBooksAndArticles();
		printFrame.setSize("210mm", "297mm");
		printFrame.setVisible(false);
		RootPanel.get().add(printFrame);
	}

	@Override
	public void onResize(ResizeEvent event) {
		if (viewport!=null) {
			viewport.setSize(
					new Integer(event.getWidth()).toString()+"px",
					new Integer(event.getHeight()).toString()+"px"
					);
		}
	}

}
