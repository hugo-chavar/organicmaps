package app.organicmaps.search.tranqueras.db;

import androidx.room.Dao;
import androidx.room.Insert;
import androidx.room.Query;

import java.util.List;

import app.organicmaps.search.tranqueras.model.Actualizacion;

@Dao
public interface ActualizacionDao {
    @Insert
    void insert(Actualizacion actualizacion);

    @Query("SELECT * FROM actualizaciones")
    List<Actualizacion> getAll();

    @Query("DELETE FROM actualizaciones")
    void deleteAll();
}