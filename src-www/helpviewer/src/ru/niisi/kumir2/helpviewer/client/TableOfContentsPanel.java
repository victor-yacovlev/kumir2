package ru.niisi.kumir2.helpviewer.client;

import java.util.LinkedList;
import java.util.List;

import com.google.gwt.event.logical.shared.SelectionEvent;
import com.google.gwt.event.logical.shared.SelectionHandler;
import com.google.gwt.user.client.ui.ScrollPanel;
import com.google.gwt.user.client.ui.Tree;
import com.google.gwt.user.client.ui.TreeItem;
import com.google.gwt.xml.client.Document;

public class TableOfContentsPanel extends ScrollPanel {

	private List<Document> books = new LinkedList<Document>();
	private List<Document> articles = new LinkedList<Document>();
	private Tree tree = null;
	
	public TableOfContentsPanel()
	{
		super();
		tree = new Tree();
		setWidget(tree);
		tree.addSelectionHandler(new SelectionHandler<TreeItem>() {
			
			@Override
			public void onSelection(SelectionEvent<TreeItem> event) {
				final TreeItem item = event.getSelectedItem();
				if (item!=null) {
					final DocBookModel dbm = (DocBookModel)item;
					fireEvent(new OpenNodeEvent(dbm));
				}
				
			}
		});
	}
	
	public void addBook(final Document book) {
		books.add(book);
		TreeItem bookItem = new DocBookModel(book.getDocumentElement());
		tree.addItem(bookItem);
	}
	
	public void addArticle(final Document article) {
		articles.add(article);
		TreeItem articleItem = new DocBookModel(article.getDocumentElement());
		tree.addItem(articleItem);
	}
	
	@SuppressWarnings("unchecked")
	public void addOpenNodeHandler(OpenNodeHandler handler) {
		addHandler(handler, OpenNodeEvent.getType());
	}
	
}
