package ru.niisi.kumir2.helpviewer.client;

import java.util.HashMap;
import java.util.Map;

class Link {
	String title;
	DocBookModel target;
}


public class XrefDatabase {
	private static Map<String, Link> db = new HashMap<String, Link>();
	
	public static void add(String documentId, String targetId, String title, DocBookModel target)
	{
		String key = documentId + "::" + targetId;
		Link l = new Link();
		l.title = title;
		l.target = target;
		db.put(key, l);
	}
	
	public static DocBookModel findTarget(String documentId, String targetId)
	{
		String key = documentId + "::" + targetId;
		if (db.containsKey(key)) {
			return db.get(key).target;
		}
		else {
			return null;
		}
	}
	
	public static String findTitle(String documentId, String targetId) {
		String key = documentId + "::" + targetId;
		if (db.containsKey(key)) {
			return db.get(key).title;
		}
		else {
			return null;
		}
	}
}
