/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Text strings about application, vendor, version, path to resources.
 * Include getters methods.
 * 
 */

package mpeshell.service;

public class About 
{

private final static String VERSION_NAME  = "v0.00.11";
private final static String VENDOR_NAME   = "(C)2019 IC Book Labs";
private final static String SHORT_NAME    = "Memory Benchmark " + VERSION_NAME;
private final static String LONG_NAME     = "Java " + SHORT_NAME;
private final static String WEB_SITE      = "http://icbook.com.ua";
private final static String RESOURCE_PATH = "/mpeshell/resources/";
private final static String VENDOR_ICON   = RESOURCE_PATH + "icbook.jpg";

public static String getVersionName()  { return VERSION_NAME;  }
public static String getVendorName()   { return VENDOR_NAME;   }
public static String getShortName()    { return SHORT_NAME;    }
public static String getLongName()     { return LONG_NAME;     }
public static String getWebSite()      { return WEB_SITE;      }
public static String getResourcePath() { return RESOURCE_PATH; }
public static String getVendorIcon()   { return VENDOR_ICON;   }

}
