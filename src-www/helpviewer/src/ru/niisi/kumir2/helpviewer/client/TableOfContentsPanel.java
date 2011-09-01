package ru.niisi.kumir2.helpviewer.client;


import com.google.gwt.dom.client.Style.Overflow;
import com.google.gwt.event.logical.shared.SelectionEvent;
import com.google.gwt.event.logical.shared.SelectionHandler;
import com.google.gwt.user.client.ui.ScrollPanel;
import com.google.gwt.user.client.ui.Tree;
import com.google.gwt.user.client.ui.TreeItem;
import com.google.gwt.xml.client.Document;

public class TableOfContentsPanel extends ScrollPanel {

	private static Tree tree = null;
	
	public TableOfContentsPanel()
	{
		super();
		ensureDebugId("tocPanel");
		tree = new Tree();
		exportStaticMethods();
		tree.ensureDebugId("tocTree");
		setWidget(tree);
		getElement().getStyle().setOverflowX(Overflow.HIDDEN);
		tree.addSelectionHandler(new SelectionHandler<TreeItem>() {
			
			@Override
			public void onSelection(SelectionEvent<TreeItem> event) {
				final TreeItem item = event.getSelectedItem();
				if (item!=null) {
					final DocBookModel dbm = (DocBookModel)item;
					fireEvent(new OpenNodeEvent(dbm));
				}
				scrollToLeft();
				
			}
		});
	}
	
	public void addBook(final Document book) {
		TreeItem bookItem = new DocBookModel(book.getDocumentElement());
		tree.addItem(bookItem);
		if (tree.getSelectedItem()==null) {
			tree.setSelectedItem(tree.getItem(0));
			tree.ensureSelectedItemVisible();
		}
	}
	
	public void addArticle(final Document article) {
		TreeItem articleItem = new DocBookModel(article.getDocumentElement());
		tree.addItem(articleItem);
		if (tree.getSelectedItem()==null) {
			tree.setSelectedItem(tree.getItem(0));
			tree.ensureSelectedItemVisible();
		}
	}
	
	@SuppressWarnings("unchecked")
	public void addOpenNodeHandler(OpenNodeHandler handler) {
		addHandler(handler, OpenNodeEvent.getType());
	}
	
	public static void doOpenXRef(String documentId, String targetId) {
		if (tree!=null) {
			TreeItem item = XrefDatabase.findTarget(documentId, targetId);
			if (item!=null) {
				tree.setSelectedItem(item);
				tree.ensureSelectedItemVisible();
			}
		}
	}
	
	public static native void exportStaticMethods() /*-{
		$wnd.openXref = $entry(@ru.niisi.kumir2.helpviewer.client.TableOfContentsPanel::doOpenXRef(Ljava/lang/String;Ljava/lang/String;));
	}-*/;
	
}
