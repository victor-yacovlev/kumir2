package ru.niisi.kumir2.helpviewer.client;


import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.ScrollPanel;

public class ViewContentPanel extends ScrollPanel implements OpenNodeHandler {

	private HTML html = null;
	
	public ViewContentPanel() {
		super();
		html = new HTML("");
		add(html);
	}

	@Override
	public void onItemSelected(DocBookModel item) {
		String title = item.getTitle();
		html.setHTML("<div class=\"docbook_screen\">\n"+item.getContentHTML()+"</div>");
	}
	
}
