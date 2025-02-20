package app.organicmaps.search.tranqueras;

import android.content.Context;

import androidx.annotation.NonNull;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.io.IOException;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.stream.Collectors;

import app.organicmaps.bookmarks.data.FeatureId;
import app.organicmaps.search.NativeSearchListener;
import app.organicmaps.search.tranqueras.db.TranquerasDatabase;
import app.organicmaps.search.tranqueras.model.Actualizacion;
import app.organicmaps.search.tranqueras.model.Tranquera;
import app.organicmaps.util.Distance;
import app.organicmaps.util.log.Logger;

import app.organicmaps.search.SearchResult;
import app.organicmaps.search.Popularity;
import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.http.GET;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class TranquerasSearch {

    public interface TranquerasApiService {
        @GET("bjuarez/tranqueras")
        Call<ResponseBody> fetchData();
    }

    private final ExecutorService executorService; // Thread pool for background tasks
    // private final ApiResultListener listener;     // Listener to notify
    
    private NativeSearchListener listener;
    private TranquerasDatabase db;
    // private Context context;

    public TranquerasSearch(NativeSearchListener listener) {
        this.listener = listener;
        this.executorService = Executors.newFixedThreadPool(2);
    }

    public void search(@NonNull Context context, String query) {
        // Call API asyncro
        // when task ends
        // 1) create results => SearchResult[] results
        // 2) invoke listener.onResultsUpdate
        // public void onResultsUpdate(@NonNull final SearchResult[] results, final long timestamp)
        // 3) and listener.onResultsEnd
        // public void onResultsEnd(final long timestamp)
        // Submit the task to the background thread
        Logger.i("HUGO Tranqueras", "STARTS TRANQUERAS SEARCH");
        executorService.submit(() -> {
            try {
                TranquerasDatabase db = TranquerasDatabase.getInstance(context);
                Logger.i("HUGO Tranqueras", "Ok database");
                // check if the first actualizacion is valid
                List<Actualizacion> actualizaciones = db.actualizacionDao().getAll();
                Logger.i("HUGO Tranqueras", "actualizaciones");
                Instant instant = Instant.now();
                long timeStampMillis = instant.toEpochMilli();
                if (actualizaciones.isEmpty()) {
                    // this will not work the first time
                    // Put a flag "loading" to avoid duplicate call
                    Logger.i("HUGO Tranqueras", "actualizaciones empty");
                    fetchTranqueras(context);

                    // we could 1) send here and empty result to avoid the UI to hang
                    SearchResult[] results = {};
                    listener.onResultsUpdate(results, timeStampMillis);
                    listener.onResultsEnd(timeStampMillis);

                    // Also when it not isValid it will never work
                    // another option is to 2) submit 2 different threads 
                    // 1 for database update and the other for search => Sounds good
                    // Best => I think the best option is to 3) run the "load" when the user opens
                    // the search and the threads should wait until "loading is false"
                    // for now we are doing 1)
                } else if (actualizaciones.get(0).isValid()) {
                    Logger.i("HUGO Tranqueras", "actualizaciones.get(0).isValid");
                    List<Tranquera> tranqueras = db.tranqueraDao().getAll();

                    List<Tranquera> filteredTranqueras = tranqueras.stream()
                        .filter(tranquera -> tranquera.toString().contains(query))
                        .collect(Collectors.toList());
                    int resultsSize = filteredTranqueras.size();

                    SearchResult[] results = new SearchResult[resultsSize];

                    for (int i = 0; i < resultsSize; i++) {
                        results[i] = tranqueraToSearchResult(filteredTranqueras.get(i));
                    }
                    listener.onResultsUpdate(results, timeStampMillis);
                    listener.onResultsEnd(timeStampMillis);

                }
                Logger.i("HUGO Tranqueras", "fin");

            } catch (Exception e) {
                Logger.e("HUGO Tranqueras","HUGO Error1: " + e.getMessage());
            }
        });
    }

    private SearchResult tranqueraToSearchResult(Tranquera tranquera) {
        FeatureId featureId = FeatureId.EMPTY;
        Distance distance = Distance.EMPTY;
        Logger.i("HUGO Tranqueras", "tranqueraToSearchResult");
        SearchResult.Description desc = new SearchResult.Description(featureId, "Dummy_featureType", "Dummy_region", distance,
                                                tranquera.getDescription(), 0, 0, 0, false);
        // Description(FeatureId featureId, String featureType, String region, Distance distance,
        //                String description, int openNow, int minutesUntilOpen, int minutesUntilClosed,
        //                boolean hasPopularityHigherPriority)

        Popularity popularity = Popularity.defaultInstance();
        //   public SearchResult(String name, Description description, double lat, double lon, int[] highlightRanges,
        //               int[] descHighlightRanges, @NonNull Popularity popularity)

        return new SearchResult(tranquera.getName(), desc, tranquera.getLat(), tranquera.getLon(), new int[] {}, new int[] {}, popularity);

        
    }

    private void fetchTranqueras(@NonNull Context context) {
        Logger.i("HUGO Tranqueras", "fetchTranqueras");
        Retrofit retrofit = new Retrofit.Builder()
            .baseUrl("https://api.bordergis.com/")
            .addConverterFactory(GsonConverterFactory.create())
            .build();

        TranquerasApiService apiService = retrofit.create(TranquerasApiService.class);
        Call<ResponseBody> call = apiService.fetchData();
        Logger.i("HUGO Tranqueras", "call");
        call.enqueue(new Callback<ResponseBody>() {
            @Override
            public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {
                if (response.isSuccessful()) {
                    Logger.i("HUGO Tranqueras", "response.isSuccessful");
                    final List<Tranquera> tranqueras;
                    try {
                        tranqueras = parseResponse(response.body().string());
                        Logger.i("HUGO Tranqueras", "parseResponse ok");
                    } catch (IOException e) {
                        Logger.e("HUGO Tranqueras","HUGO Error0: " + e.getMessage());
                        return;
                    }
                    // runOnUiThread(() -> listener.onSuccess(parsedResult));
                    // listener.onSuccess(parsedResult);
                    executorService.submit(() -> {
                        TranquerasDatabase db = TranquerasDatabase.getInstance(context);
                        Logger.i("HUGO Tranqueras", "db.tranqueraDao().insert");

                        for (Tranquera t: tranqueras) {
                            db.tranqueraDao().insert(t);
                        }

                        Instant instant = Instant.now();
                        long timeStampMillis = instant.toEpochMilli();

                        db.actualizacionDao().insert(new Actualizacion(timeStampMillis));
                        Logger.i("HUGO Tranqueras", "db.actualizacionDao().insert");
                    });
                } else {
                    // runOnUiThread(() -> listener.onFailure("API call failed"));
                    // listener.onFailure("API call failed");
                    Logger.e("HUGO Tranqueras","HUGO Error2: " + response.toString());
                }
            }

            @Override
            public void onFailure(Call<ResponseBody> call, Throwable t) {
                // runOnUiThread(() -> listener.onFailure(t.getMessage()));
                // listener.onFailure(t.getMessage());
                Logger.e("HUGO Tranqueras","HUGO Error3: " + t.getMessage());
            }
        });
    }

    public List<Tranquera> parseResponse(String jsonResponse) {
        List<Tranquera> tranqueras = new ArrayList<>();
        Gson gson = new Gson();
        JsonParser parser = new JsonParser();
        JsonObject jsonObject = parser.parse(jsonResponse).getAsJsonObject();
        JsonArray features = jsonObject.getAsJsonArray("features");

        for (JsonElement featureElement : features) {
            JsonObject feature = featureElement.getAsJsonObject();
            int id = feature.get("id").getAsInt();

            JsonObject geometry = feature.getAsJsonObject("geometry");
            JsonArray coordinates = geometry.getAsJsonArray("coordinates");
            double lon = coordinates.get(0).getAsDouble();
            double lat = coordinates.get(1).getAsDouble();

            JsonObject properties = feature.getAsJsonObject("properties");
            String name = properties.get("estab").getAsString();
            String idt = properties.get("idt").getAsString();
            String camino = properties.get("camino").getAsString();
            String estado = properties.get("estado").getAsString();
            String description = idt + ", " + camino + ", " + estado;

            Tranquera tranquera = new Tranquera(id, name, description, lat, lon);
            tranqueras.add(tranquera);
        }

        return tranqueras;
    }
}
