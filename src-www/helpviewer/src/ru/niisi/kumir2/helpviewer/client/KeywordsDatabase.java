package ru.niisi.kumir2.helpviewer.client;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.google.gwt.user.client.ui.TreeItem;

public class KeywordsDatabase {
	private static Map<String, List<DocBookModel>> db = new HashMap<String, List<DocBookModel>>();
	
	public static void addEntry(final String keyword, final DocBookModel model)
	{
		if (db.containsKey(keyword)) {
			List<DocBookModel> lst = db.get(keyword);
			lst.add(model);
		}
		else {
			List<DocBookModel> lst = new LinkedList<DocBookModel>();
			lst.add(model);
			db.put(keyword, lst);
		}
	}
	
	public static List<DocBookModel> find(final String request)
	{
		List<DocBookModel> result = new LinkedList<DocBookModel>();
		if (db.containsKey(request)) {
			return db.get(request);
		}
		return result;
	}
}
