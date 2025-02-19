package app.organicmaps.search.tranqueras.model;

import androidx.room.Entity;
import androidx.room.PrimaryKey;

@Entity(tableName = "actualizaciones")
public class Actualizacion {
    @PrimaryKey(autoGenerate = true)
    private int id;

    private long timestamp;

    public Actualizacion(long timestamp) {
        this.timestamp = timestamp;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public boolean isValid() {
        // Get the current time in milliseconds
        //TODO more modern:
        // Instant instant = Instant.now();
        // long timeStampMillis = instant.toEpochMilli();

        long currentTime = System.currentTimeMillis();
        
        // Calculate the difference in milliseconds
        long difference = currentTime - this.timestamp;
        
        // 24 hours in milliseconds
        long twentyFourHoursInMillis = 24 * 60 * 60 * 1000;
        
        // Return false if more than 24 hours have passed
        return difference <= twentyFourHoursInMillis;
    }
}