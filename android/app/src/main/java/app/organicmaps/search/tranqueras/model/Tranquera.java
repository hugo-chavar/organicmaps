package app.organicmaps.search.tranqueras.model;

import androidx.room.Entity;
import androidx.room.PrimaryKey;

@Entity(tableName = "tranqueras")
public class Tranquera {
    @PrimaryKey(autoGenerate = false)
    private int id;

    private String name;
    private String description;
    private double lat;
    private double lon;

    public Tranquera(int id, String name, String description, double lat, double lon) {
        this.id = id;
        this.name = name;
        this.description = description;
        this.lat = lat;
        this.lon = lon;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public double getLat() {
        return lat;
    }

    public void setLat(int lat) {
        this.lat = lat;
    }

    public double getLon() {
        return lon;
    }

    public void setLon(double lon) {
        this.lon = lon;
    }
}