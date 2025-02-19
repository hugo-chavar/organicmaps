package app.organicmaps.search.tranqueras.db;

import androidx.room.Database;
import androidx.room.Room;
import androidx.room.RoomDatabase;
import android.content.Context;

import app.organicmaps.search.tranqueras.model.Actualizacion;
import app.organicmaps.search.tranqueras.model.Tranquera;

@Database(entities = {Tranquera.class, Actualizacion.class}, version = 2, exportSchema = false)
public abstract class TranquerasDatabase extends RoomDatabase {
    private static TranquerasDatabase instance;

    public abstract TranqueraDao tranqueraDao();
    public abstract ActualizacionDao actualizacionDao();

    public static synchronized TranquerasDatabase getInstance(Context context) {
        if (instance == null) {
            instance = Room.databaseBuilder(context.getApplicationContext(),
                            TranquerasDatabase.class, "tranqueras_db")
                    .fallbackToDestructiveMigration()
                    .build();
        }
        return instance;
    }
}