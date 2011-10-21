package ru.niisi.kumir2.helpviewer.client;


import java.util.List;

import com.google.gwt.dom.client.Style.Overflow;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.logical.shared.ResizeEvent;
import com.google.gwt.event.logical.shared.ResizeHandler;
import com.google.gwt.event.logical.shared.SelectionEvent;
import com.google.gwt.event.logical.shared.SelectionHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DockLayoutPanel;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.ScrollPanel;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.Tree;
import com.google.gwt.user.client.ui.TreeItem;
import com.google.gwt.user.client.ui.VerticalPanel;


class SearchItem extends TreeItem
{
	private DocBookModel model;

	public SearchItem(DocBookModel m) {
		super(m.getTitle());
		getElement().addClassName("SearchItemClass");
		model = m;
	}
	
	public DocBookModel getModel() {
		return model;
	}
}

public class IndexPanel extends ScrollPanel implements ClickHandler, SelectionHandler<TreeItem> {

	private TextBox searchBox;
	private Button doSearchButton;
	private Tree resultsBox;
	private ScrollPanel scrollPanel;
	
	public IndexPanel() {
		super();
		getElement().setId("SearchPanel");
		VerticalPanel vpanel = new VerticalPanel();
		HorizontalPanel panel = new HorizontalPanel();
		panel.getElement().setId("SearchRequestPanel");
		searchBox = new TextBox();
		searchBox.getElement().setId("SearchTextBox");
		doSearchButton = new Button();
		doSearchButton.getElement().setId("DoSearchButton");
		panel.add(searchBox);
		panel.add(doSearchButton);
		
		vpanel.add(panel);
		
		resultsBox = new Tree();
		resultsBox.getElement().setId("SearchResults");
		
//		scrollPanel = new ScrollPanel();
//		scrollPanel.getElement().setId("ScrollPanel");
		
//		add(scrollPanel);
		getElement().getStyle().setOverflowX(Overflow.HIDDEN);
		vpanel.add(resultsBox);
		add(vpanel);
		
		doSearchButton.addClickHandler(this);
		
		resultsBox.addSelectionHandler(this);
		
		
	}

	@Override
	public void onClick(ClickEvent event) {
		resultsBox.clear();
		String query = searchBox.getText().trim().toLowerCase();
		List<DocBookModel> result = KeywordsDatabase.find(query);
		for ( DocBookModel m : result ) {
			resultsBox.addItem(new SearchItem(m));
		}
		scrollToLeft();
	}

	@Override
	public void onSelection(SelectionEvent<TreeItem> event) {
		final TreeItem item = event.getSelectedItem();
		if (item!=null) {
			final SearchItem si = (SearchItem)item;
			final DocBookModel dbm = si.getModel();
			fireEvent(new OpenNodeEvent(dbm));
		}
		scrollToLeft();
	}
	
	@SuppressWarnings("unchecked")
	public void addOpenNodeHandler(OpenNodeHandler handler) {
		addHandler(handler, OpenNodeEvent.getType());
	}
	
	
	public void setDefaultFocus() {
		searchBox.setFocus(true);
	}
	
}
