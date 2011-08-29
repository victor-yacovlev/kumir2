package ru.niisi.kumir2.helpviewer.client;

import java.util.LinkedList;
import java.util.List;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.user.client.ui.RootPanel;
import com.extjs.gxt.ui.client.Style.LayoutRegion;
import com.extjs.gxt.ui.client.util.Margins;
import com.extjs.gxt.ui.client.widget.*;
import com.extjs.gxt.ui.client.widget.layout.BorderLayout;
import com.extjs.gxt.ui.client.widget.layout.BorderLayoutData;
import com.google.gwt.user.client.Window;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.XMLParser;


public class ViewerRoot implements EntryPoint  {

	private Viewport viewport = null;
	private ViewContentPanel viewContentPanel = null;
	private TabPanel tabPanel = null;
	private TableOfContentsPanel tableOfContentsPanel = null;
	private IndexPanel indexPanel = null;
	private List<Document> books = new LinkedList<Document>();
	private List<Document> articles = new LinkedList<Document>();
	
	public ViewerRoot() {
		createComponents();
	}
	
	protected void createComponents() {
		viewport = new Viewport();
		BorderLayout layout = new BorderLayout();
		viewport.setLayout(layout);
		tabPanel = new TabPanel();
		tabPanel.setStyleAttribute("minimum-width", "240px");
		viewContentPanel = new ViewContentPanel();
		BorderLayoutData westData  = new BorderLayoutData(LayoutRegion.WEST, 240);
		westData.setSplit(true);
		westData.setCollapsible(true);
		westData.setMargins(new Margins(0,5,0,0)); 
		BorderLayoutData centerData= new BorderLayoutData(LayoutRegion.CENTER);
		viewport.add(tabPanel, westData);
		viewport.add(viewContentPanel, centerData);
		tableOfContentsPanel = new TableOfContentsPanel();
		tableOfContentsPanel.addOpenNodeHandler(viewContentPanel);
		indexPanel = new IndexPanel();
		TabItem contentsTab = new TabItem("Содержание");
		contentsTab.add(tableOfContentsPanel);
		TabItem indexTab = new TabItem("Указатель");
		indexTab.add(indexPanel);
		tabPanel.add(contentsTab);
		tabPanel.add(indexTab);
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
		RootPanel.get().add(viewport);
		loadBooksAndArticles();
	}

}
