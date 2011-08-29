package ru.niisi.kumir2.helpviewer.client;

import com.extjs.gxt.ui.client.widget.ContentPanel;
import com.extjs.gxt.ui.client.widget.Label;
import com.extjs.gxt.ui.client.widget.form.TextField;


public class IndexPanel extends ContentPanel {

	private TextField<String> searchField = null;
	
	public IndexPanel() {
		super();
		Label searchLabel = new Label("Поиск:&nbsp;");
		searchField = new TextField<String>();
		getHeader().setStyleAttribute("padding-left", "10px");
		getHeader().setStyleAttribute("padding-right", "10px");
		getHeader().addTool(searchLabel);
		getHeader().addTool(searchField);
	}
	
}
