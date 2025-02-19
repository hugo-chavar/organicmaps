package app.organicmaps.search.tranqueras.db;

import androidx.room.Dao;
import androidx.room.Insert;
import androidx.room.Query;

import java.util.List;

import app.organicmaps.search.tranqueras.model.Tranquera;

@Dao
public interface TranqueraDao {
    @Insert
    void insert(Tranquera tranquera);

    @Query("SELECT * FROM tranqueras")
    List<Tranquera> getAll();

    @Query("DELETE FROM tranqueras")
    void deleteAll();
}