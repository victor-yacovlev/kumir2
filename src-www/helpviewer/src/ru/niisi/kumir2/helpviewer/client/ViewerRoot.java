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
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.SplitLayoutPanel;
import com.google.gwt.user.client.ui.TabLayoutPanel;
import com.google.gwt.user.client.Window;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.XMLParser;


public class ViewerRoot implements EntryPoint, ResizeHandler  {

	private SplitLayoutPanel viewport = null;
	private ViewContentPanel viewContentPanel = null;
	private TabLayoutPanel tabPanel = null;
	private TableOfContentsPanel tableOfContentsPanel = null;
	private IndexPanel indexPanel = null;
	private List<Document> books = new LinkedList<Document>();
	private List<Document> articles = new LinkedList<Document>();
	
	public ViewerRoot() {
		createComponents();
	}
	
	protected void createComponents() {
		viewport = new SplitLayoutPanel();
		viewport.ensureDebugId("viewport");
		tabPanel = new TabLayoutPanel(2.5, Unit.EM);
		tabPanel.setWidth("100%");
		tabPanel.setAnimationDuration(1000);
		
		tabPanel.ensureDebugId("tabpanel");
		viewContentPanel = new ViewContentPanel();
		viewport.addWest(tabPanel, 240);
		viewport.add(viewContentPanel);
		tableOfContentsPanel = new TableOfContentsPanel();
		tableOfContentsPanel.addOpenNodeHandler(viewContentPanel);
		indexPanel = new IndexPanel();
		tabPanel.add(tableOfContentsPanel, "Содержание");
		tabPanel.add(indexPanel, "Указатель");
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
							books.add(docbookDocument);
							tableOfContentsPanel.addBook(docbookDocument);
						}
						else if (docbookDocument.getDocumentElement().getNodeName().equalsIgnoreCase("article")) {
							articles.add(docbookDocument);
							tableOfContentsPanel.addArticle(docbookDocument);
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
		loadBooksAndArticles();
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
