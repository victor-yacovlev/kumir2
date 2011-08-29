package ru.niisi.kumir2.helpviewer.client;

import com.google.gwt.event.shared.GwtEvent;

public class OpenNodeEvent extends GwtEvent<OpenNodeHandler> {

	@SuppressWarnings("rawtypes")
	private static final Type TYPE = new Type<OpenNodeHandler>();
	private DocBookModel item = null;
	
	public OpenNodeEvent(DocBookModel item) {
		super();
		this.item = item;
	}
	
	@SuppressWarnings("unchecked")
	public com.google.gwt.event.shared.GwtEvent.Type<OpenNodeHandler> getAssociatedType() {
		return TYPE;
	}
	
	@SuppressWarnings("rawtypes")
	public static Type getType() {
		return TYPE;
	}

	@Override
	protected void dispatch(OpenNodeHandler handler) {
		handler.onItemSelected(this.item);
		
	}
	
	
}
