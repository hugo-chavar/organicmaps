package app.organicmaps.search.tranqueras;

import android.content.Context;

import androidx.annotation.MainThread;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import app.organicmaps.Framework;
import app.organicmaps.search.NativeBookmarkSearchListener;
import app.organicmaps.search.NativeMapSearchListener;
import app.organicmaps.search.NativeSearchListener;
import app.organicmaps.search.SearchResult;
import app.organicmaps.util.Language;
import app.organicmaps.util.Listeners;
import app.organicmaps.util.concurrency.UiThread;
import app.organicmaps.util.log.Logger;

import java.nio.charset.StandardCharsets;

public enum TranquerasSearchEngine implements NativeSearchListener,
        NativeMapSearchListener,
        NativeBookmarkSearchListener
{
  INSTANCE;

  // Query, which results are shown on the map.
  @Nullable
  private String mQuery;
  private TranquerasSearch tranquerasSearch = new TranquerasSearch(this);

  @Override
  public void onResultsUpdate(@NonNull final SearchResult[] results, final long timestamp)
  {
    // Here we get the list with the results, the listeners are updated
    Logger.i("HUGO Tranqueras", "OK onResultsUpdate Hugo results: ");
    UiThread.run(
        () ->
        {
          for (NativeSearchListener listener : mListeners)
            listener.onResultsUpdate(results, timestamp);
          mListeners.finishIterate();
        });
  }

  @Override
  public void onResultsEnd(final long timestamp)
  {
    // the results are completed.. if the search is cancelled we don't need to do this
    Logger.i("HUGO Tranqueras", "onResultsEnd Hugo results: ");
    UiThread.run(
        () ->
        {
          for (NativeSearchListener listener : mListeners)
            listener.onResultsEnd(timestamp);
          mListeners.finishIterate();
        });
  }

  @Override
  public void onMapSearchResults(final NativeMapSearchListener.Result[] results, final long timestamp, final boolean isLast)
  {
    Logger.i("HUGO Tranqueras", "onMapSearchResults Hugo results: ");
    UiThread.run(
        () ->
        {
          for (NativeMapSearchListener listener : mMapListeners)
            listener.onMapSearchResults(results, timestamp, isLast);
          mMapListeners.finishIterate();
        });
  }

  @Override
  public void onBookmarkSearchResultsUpdate(@Nullable long[] bookmarkIds, long timestamp)
  {
    Logger.i("HUGO Tranqueras", "onBookmarkSearchResultsUpdate Hugo results: ");
    for (NativeBookmarkSearchListener listener : mBookmarkListeners)
      listener.onBookmarkSearchResultsUpdate(bookmarkIds, timestamp);
    mBookmarkListeners.finishIterate();
  }

  @Override
  public void onBookmarkSearchResultsEnd(@Nullable long[] bookmarkIds, long timestamp)
  {
    Logger.i("HUGO Tranqueras", "onBookmarkSearchResultsEnd Hugo results: ");
    for (NativeBookmarkSearchListener listener : mBookmarkListeners)
      listener.onBookmarkSearchResultsEnd(bookmarkIds, timestamp);
    mBookmarkListeners.finishIterate();
  }

  @NonNull
  private final Listeners<NativeSearchListener> mListeners = new Listeners<>();
  @NonNull
  private final Listeners<NativeMapSearchListener> mMapListeners = new Listeners<>();
  @NonNull
  private final Listeners<NativeBookmarkSearchListener> mBookmarkListeners = new Listeners<>();

  public void addListener(NativeSearchListener listener)
  {
    mListeners.register(listener);
  }

  public void removeListener(NativeSearchListener listener)
  {
    mListeners.unregister(listener);
  }

  public void addMapListener(NativeMapSearchListener listener)
  {
    mMapListeners.register(listener);
  }

  public void removeMapListener(NativeMapSearchListener listener)
  {
    mMapListeners.unregister(listener);
  }

  public void addBookmarkListener(NativeBookmarkSearchListener listener)
  {
    mBookmarkListeners.register(listener);
  }

  public void removeBookmarkListener(NativeBookmarkSearchListener listener)
  {
    mBookmarkListeners.unregister(listener);
  }

//  private native void nativeInit();

  /**
   *
   * @param context
   * @param timestamp Search results are filtered according to it after multiple requests.
   * @return whether search was actually started.
   */
  @MainThread
  public boolean search(@NonNull Context context, String query, boolean isCategory,
                        long timestamp, boolean hasLocation, double lat, double lon)
  {
    Logger.i("HUGO Tranqueras", "search Hugo query: " + query + ", lat: " + lat + ", lon: " + lon);
    // Change here HUGO => Write here the logic to pull from the api and filter, store data for offline search
    // Test if we are offline in that case only show results previusly stored here
    // we can save searchs in the profile or directly download and refresh whe user logs in
//    return nativeRunSearch(query.getBytes(StandardCharsets.UTF_8), isCategory,
//            Language.getKeyboardLocale(context), timestamp, hasLocation, lat, lon);
    
    // ??
    // if (not data is here) {
    //   load data
    // }
    
    // allways search in local database
    tranquerasSearch.search(context, query);
    return true;
  }

  @MainThread
  public void searchInteractive(@NonNull String query, boolean isCategory, @NonNull String locale,
                                long timestamp, boolean isMapAndTable, boolean hasLocation, double lat, double lon)
  {
    Logger.i("HUGO Tranqueras", "searchInteractive Hugo query: " + query + ", lat: " + lat + ", lon: " + lon);
//    nativeRunInteractiveSearch(query.getBytes(StandardCharsets.UTF_8), isCategory,
//            locale, timestamp, isMapAndTable, hasLocation, lat, lon);
  }

  @MainThread
  public void searchInteractive(@NonNull String query, boolean isCategory, @NonNull String locale,
                                long timestamp, boolean isMapAndTable)
  {
    searchInteractive(query, isCategory, locale, timestamp, isMapAndTable, false, 0, 0);
  }

  @MainThread
  public void searchInteractive(@NonNull Context context, @NonNull String query, boolean isCategory,
                                long timestamp, boolean isMapAndTable)
  {
    searchInteractive(query, isCategory, Language.getKeyboardLocale(context), timestamp, isMapAndTable, false, 0, 0);
  }

  @MainThread
  public static void searchMaps(@NonNull Context context, String query, long timestamp)
  {
    Logger.i("HUGO Tranqueras", "searchMaps Hugo query: " + query);
//    nativeRunSearchMaps(query.getBytes(StandardCharsets.UTF_8), Language.getKeyboardLocale(context),
//                        timestamp);
  }

  @MainThread
  public boolean searchInBookmarks(@NonNull String query, long categoryId, long timestamp)
  {
    Logger.i("HUGO Tranqueras", "searchInBookmarks Hugo query: " + query + ", categoryid: " + categoryId);
    return true;
//    return nativeRunSearchInBookmarks(query.getBytes(StandardCharsets.UTF_8), categoryId, timestamp);
  }

  public void setQuery(@Nullable String query)
  {
    mQuery = query;
  }

  @Nullable
  public String getQuery()
  {
    return mQuery;
  }

  @MainThread
  public void cancel()
  {
    cancelApiCall();
    cancelAllSearches();
  }

  @MainThread
  private static void cancelApiCall()
  {
    Logger.i("HUGO Tranqueras","Framework.nativeClearApiPoints");
//    Framework.nativeClearApiPoints();
  }

  @MainThread
  public void cancelInteractiveSearch()
  {
    mQuery = "";
    Logger.i("HUGO Tranqueras","nativeCancelInteractiveSearch");
//    nativeCancelInteractiveSearch();
  }

  @MainThread
  private void cancelAllSearches()
  {
    mQuery = "";
    Logger.i("HUGO Tranqueras","nativeCancelAllSearches");
//    nativeCancelAllSearches();
  }

  @MainThread
  public void showResult(int index)
  {
    mQuery = "";
    Logger.i("HUGO Tranqueras","nativeShowResult");
//    nativeShowResult(index);
  }

  public void initialize()
  {
    Logger.i("HUGO Tranqueras","nativeInit");
//    nativeInit();
  }

  /**
   * @param bytes utf-8 formatted bytes of query.
   */
//  private static native boolean nativeRunSearch(byte[] bytes, boolean isCategory,
//                                                String language, long timestamp, boolean hasLocation,
//                                                double lat, double lon);

  /**
   * @param bytes utf-8 formatted query bytes
   */
//  private static native void nativeRunInteractiveSearch(byte[] bytes, boolean isCategory,
//                                                        String language, long timestamp,
//                                                        boolean isMapAndTable, boolean hasLocation,
//                                                        double lat, double lon);

  /**
   * @param bytes utf-8 formatted query bytes
   */
//  private static native void nativeRunSearchMaps(byte[] bytes, String language, long timestamp);
//
//  private static native boolean nativeRunSearchInBookmarks(byte[] bytes, long categoryId, long timestamp);
//
//  private static native void nativeShowResult(int index);
//
//  private static native void nativeCancelInteractiveSearch();
//
//  private static native void nativeCancelEverywhereSearch();
//
//  private static native void nativeCancelAllSearches();
}
