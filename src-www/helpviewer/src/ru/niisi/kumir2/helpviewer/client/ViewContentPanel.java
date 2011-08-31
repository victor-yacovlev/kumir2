package ru.niisi.kumir2.helpviewer.client;


import com.google.gwt.user.client.ui.NamedFrame;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.ScrollPanel;

public class ViewContentPanel extends ScrollPanel implements OpenNodeHandler {

	private HTML html = null;
	
	public ViewContentPanel() {
		super();
		exportStaticMethods();
		html = new HTML("");
		add(html);
	}

	@Override
	public void onItemSelected(DocBookModel item) {
		html.setHTML("<div class=\"docbook_screen\">\n"+item.getContentHTML(true, false)+"</div>");
		scrollToTop();
	}
	
	public static void doPrint(String documentId, String targetId) {
		DocBookModel item = XrefDatabase.findTarget(documentId, targetId);
		String printableHTML = item.getContentHTML(true, true);
		if (item.getDocBookType()==DocBookType.Book)
			printableHTML = "<div class=\"book\">\n"+printableHTML+"\n</div>\n";
		else if (item.getDocBookType()==DocBookType.Article)
			printableHTML = "<div class=\"article\">\n"+printableHTML+"\n</div>\n";
		setPrintFrameHTML(printableHTML);
		printIFrame();
	}
	
	public static native void exportStaticMethods() /*-{
		$wnd.printThis = $entry(@ru.niisi.kumir2.helpviewer.client.ViewContentPanel::doPrint(Ljava/lang/String;Ljava/lang/String;));
	}-*/;
	
	public static native void setPrintFrameHTML(String s) /*-{
		$wnd.setPrintFrameContents(s);
	}-*/;
	
	public static native void printIFrame() /*-{
		$wnd.doPrintIFrame();
	}-*/;
	
	
	
}
