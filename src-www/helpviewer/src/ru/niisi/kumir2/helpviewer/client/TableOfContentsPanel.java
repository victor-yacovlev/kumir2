package ru.niisi.kumir2.helpviewer.client;

import java.util.LinkedList;
import java.util.List;

import com.extjs.gxt.ui.client.Style;
import com.extjs.gxt.ui.client.data.ModelData;
import com.extjs.gxt.ui.client.event.SelectionChangedEvent;
import com.extjs.gxt.ui.client.event.SelectionChangedListener;
import com.extjs.gxt.ui.client.store.TreeStore;
import com.extjs.gxt.ui.client.widget.LayoutContainer;
import com.extjs.gxt.ui.client.widget.treepanel.TreePanel;
import com.extjs.gxt.ui.client.widget.treepanel.TreePanelSelectionModel;
import com.google.gwt.xml.client.Document;

public class TableOfContentsPanel extends LayoutContainer {

	private List<Document> books = new LinkedList<Document>();
	private List<Document> articles = new LinkedList<Document>();
	private TreeStore<ModelData> store = new TreeStore<ModelData>();
	private TreePanel<ModelData> treePanel = null;
	
	public TableOfContentsPanel()
	{
		super();
		treePanel = new TreePanel<ModelData>(store);
		treePanel.setDisplayProperty("title");
		treePanel.setAutoHeight(true);
		treePanel.setStyleAttribute("overflow-x", "hidden");
		TreePanelSelectionModel<ModelData> sm = treePanel.getSelectionModel();
		sm.addSelectionChangedListener(new SelectionChangedListener<ModelData>() {
		
			@Override
			public void selectionChanged(SelectionChangedEvent<ModelData> se) {
				final DocBookModel md = (DocBookModel)se.getSelectedItem();
				fireEvent(new OpenNodeEvent(md));
			}
		});
		add(treePanel);
		
	}
	
	public void addBook(final Document book) {
		books.add(book);
		store.add(new DocBookModel(book.getDocumentElement()), true);
	}
	
	public void addArticle(final Document article) {
		articles.add(article);
		store.add(new DocBookModel(article.getDocumentElement()), true);
	}
	
	@SuppressWarnings("unchecked")
	public void addOpenNodeHandler(OpenNodeHandler handler) {
		addHandler(handler, OpenNodeEvent.getType());
	}
	
}
