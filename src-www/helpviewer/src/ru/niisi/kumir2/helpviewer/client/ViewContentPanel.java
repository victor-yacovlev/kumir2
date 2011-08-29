package ru.niisi.kumir2.helpviewer.client;


import com.extjs.gxt.ui.client.widget.ContentPanel;
import com.extjs.gxt.ui.client.*;
import com.google.gwt.user.client.ui.HTML;

public class ViewContentPanel extends ContentPanel implements OpenNodeHandler {

	private HTML html = null;
	
	public ViewContentPanel() {
		super();
		html = new HTML("Welcome!");
		setScrollMode(Style.Scroll.AUTO);
		add(html);
	}

	@Override
	public void onItemSelected(DocBookModel item) {
		String title = item.getTitle();
		DocBookModel p = (DocBookModel) item.getParent();
		while (p!=null) {
			title = p.getTitle() + " -> " + title;
			p = (DocBookModel)p.getParent();
		}
		setHeading(title);
		html.setHTML("<div class=\"docbook_screen\">\n"+item.getHTML()+"</div>");
	}
	
}
