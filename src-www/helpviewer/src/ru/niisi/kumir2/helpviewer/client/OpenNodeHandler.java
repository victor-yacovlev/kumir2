package ru.niisi.kumir2.helpviewer.client;

import com.google.gwt.event.shared.EventHandler;

public interface OpenNodeHandler extends EventHandler {

	void onItemSelected(DocBookModel item);

}
